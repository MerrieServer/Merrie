#include "Commons/Ticker.hpp"
#include <cmath>

namespace Merrie {

    // ================================================================================
    // =  Task                                                                   =
    // ================================================================================

    Task::Task(std::thread::id mainThread, TaskAction action, bool repeating)
            : m_mainThread(mainThread), m_action(std::move(action)), m_repeating(repeating) {
    }

    Task::~Task() {
        m_cancelled = true;
    }

    bool Task::IsRepeating() const noexcept {
        return m_repeating;
    }

    bool Task::IsFinished() const noexcept {
        std::scoped_lock lock(m_waitMutex);
        return m_finished;
    }

    bool Task::IsSuccess() const noexcept {
        std::scoped_lock lock(m_waitMutex);
        return m_success;
    }

    bool Task::ShouldWait() const {
        std::scoped_lock lock(m_waitMutex);
        return !m_finished && !m_cancelled;
    }

    void Task::Wait() {
        if (IsFinished()) {
            return;
        }

        if (std::this_thread::get_id() == m_mainThread) {
            throw CannotWaitInMainThread("waiting for unfinished tasks in the main thread will always cause a deadlock");
        }

        std::unique_lock<std::mutex> lock(m_notifyMutex);

        while (ShouldWait()) {
            m_conditionVariable.wait(lock);
        }
    }

    void Task::Execute() {
        m_action(shared_from_this());
    }

    void Task::Finalize(bool success) {
        std::scoped_lock lock(m_waitMutex);
        m_finished = true;
        m_success = success;
    }

    void Task::NotifyAboutFinish() {
        std::scoped_lock lock(m_notifyMutex);
        m_conditionVariable.notify_all();
    }

    void Task::Cancel() noexcept {
        CancelNoNotify();

        if (!IsFinished()) {
            NotifyAboutFinish();
        }
    }

    void Task::CancelNoNotify() noexcept {
        std::scoped_lock lock(m_waitMutex);
        m_cancelled = true;
    }

    // ================================================================================
    // =  Ticker                                                                      =
    // ================================================================================

    Ticker::Ticker() {
        m_mainThread = std::this_thread::get_id();
        m_tps = DefaultTps;
        ResetAll();
    }

    Ticker::~Ticker() noexcept = default;

    TimeStamp Ticker::TimeNow() {
        using namespace std::chrono;

        return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    void Ticker::ResetAll() {
        EnsureInMainThread();

        std::scoped_lock taskGuard(m_taskMutex);

        m_currentTick = 0;
        m_lastTick = m_tickSection = Ticker::TimeNow();
        m_catchupTime = 0;
        m_tasks.clear();
        SetTps(GetTps());
    }

    const std::thread::id& Ticker::GetMainThread() const noexcept {
        return m_mainThread;
    }

    bool Ticker::IsInMainThread() const noexcept {
        return std::this_thread::get_id() == m_mainThread;
    }

    void Ticker::EnsureInMainThread() const {
        if (IsInMainThread()) {
            return;
        }

        throw NotInMainThreadException("not in main thread");
    }

    unsigned int Ticker::GetTps() const {
        return m_tps;
    }

    [[nodiscard]] static inline double _ExponentFor(double sampleIntervalSeconds, int minutes) noexcept {
        return 1.0 / std::exp(sampleIntervalSeconds / std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes(minutes)).count());
    }

    [[nodiscard]] static inline double _CalcTps(double avg, double exp, double tps) noexcept {
        return (avg * exp) + (tps * (1.0 - exp));
    }

    void Ticker::SetTps(unsigned int tps) {
        m_tps = tps;
        m_waitTime = DurationsInSecond / tps;

        const double sampleIntervalSeconds = (TpsSampleInterval / (double) tps);

        m_exponents[0] = _ExponentFor(sampleIntervalSeconds, 1);
        m_exponents[1] = _ExponentFor(sampleIntervalSeconds, 5);
        m_exponents[2] = _ExponentFor(sampleIntervalSeconds, 15);

        for (double& recentTps : m_recentTps) {
            recentTps = tps;
        }
    }

    RecentTps Ticker::GetRecentTps() const {
        return RecentTps{
                m_recentTps[0],
                m_recentTps[1],
                m_recentTps[2],
        };
    }

    Tick Ticker::GetCurrentTick() const {
        EnsureInMainThread();
        return m_currentTick;
    }

    std::shared_ptr<Task> Ticker::DoInMainThread(TaskAction action, bool repeat) {
        auto task = std::make_shared<Task>(GetMainThread(), std::move(action), repeat);

        // Execute on the spot if in main thread
        if (IsInMainThread()) {
            task->Execute();

            // If not repeating don't even bother adding it to the task list, it was already done
            if (!repeat) {
                return task;
            }
        }

        std::lock_guard<std::mutex> lock(m_taskMutex);
        return m_tasks.emplace_back(task);
    }

    void Ticker::DoTick() {
        EnsureInMainThread();

        const TimeStamp currentTime = Ticker::TimeNow();
        const TimeStamp waitTime = m_waitTime - (currentTime - m_lastTick) - m_catchupTime;

        if (waitTime > 0L) {
            std::this_thread::sleep_for(TimeStampDuration(waitTime));
            m_catchupTime = 0;
        } else {
            m_catchupTime = std::min(DurationsInSecond, std::abs(waitTime));

            if ((m_currentTick++) % TpsSampleInterval == 0) {
                const double currentTps = std::min((double) m_tps, ((double) DurationsInSecond) / static_cast<double>(currentTime - m_tickSection) * TpsSampleInterval);

                for (size_t i = 0; i < sizeof(m_recentTps) / sizeof(m_recentTps[0]); i++) {
                    m_recentTps[i] = _CalcTps(m_recentTps[i], m_exponents[i], currentTps);
                }

                m_tickSection = currentTime;
            }

            m_lastTick = currentTime;
            RunTasks();
        }
    }

    void Ticker::RunTasks() {
        std::vector<std::shared_ptr<Task>> tasksToDo;

        {
            std::scoped_lock lock(m_taskMutex);
            tasksToDo.insert(end(tasksToDo), begin(m_tasks), std::end(m_tasks));

            if (m_tasks.empty()) {
                return;
            }

            // we already have a lock so why not remove cancelled tasks here
            m_tasks.erase(
                    std::remove_if(begin(m_tasks), end(m_tasks), [](const std::shared_ptr<Task>& task) { return task->m_cancelled; }),
                    end(m_tasks));

            // copy task vector, it only makes copies the shared_ptr's so the overhead is very small
            tasksToDo = m_tasks;
        }

        for (const std::shared_ptr<Task>& task : tasksToDo) {
            bool success = true;

            try {
                task->Execute();
            }
            catch (const std::exception&) {
                success = false;
            }

            if (!task->IsRepeating()) {
                task->CancelNoNotify();
            }

            task->Finalize(success);
            task->NotifyAboutFinish();
        }
    }
}