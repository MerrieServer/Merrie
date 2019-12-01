#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TICKER_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TICKER_HPP

#include <Commons/Commons.hpp>
#include <condition_variable>
#include <thread>
#include <mutex>

namespace Merrie {

    class Task; // forward declaration

    // ================================================================================
    // =  Type declarations                                                           =
    // ================================================================================

    /**
     * The basic unit of time used to calculating wait times.
     */
    using TimeStampDuration = std::chrono::nanoseconds;

    /**
     * Numerical type used to store timestamps.
     */
    using TimeStamp = int64_t;

    /**
     * Represents a numeric type used for storing current tick number.
     */
    using Tick = uint32_t;

    /**
     * Represents a function that can be used to create a task.
     */
    using TaskAction = std::function<void(const std::shared_ptr<Task>& thisTask)>;

    /**
     * Represents an average TPS for last 1, 5 and 15 minutes
     */
    struct RecentTps {
        /**
         * Represents an average TPS for the last 1 minute
         */
        double Last1Minute;

        /**
         * Represents an average TPS for the last 5 minutes
         */
        double Last5Minutes;

        /**
         * Represents an average TPS for the last 15 minutes
         */
        double Last15Minutes;
    };

    // ================================================================================
    // =  Constants                                                                   =
    // ================================================================================

    /**
     * The default TPS for a ticker.
     */
    constexpr const unsigned int DefaultTps = 100;

    /**
     * How much basic units of time (see TimeStampDuration)
     */
    constexpr const TimeStamp DurationsInSecond = std::chrono::duration_cast<TimeStampDuration>(std::chrono::seconds(1)).count();

    /**
     * Recent TPS sampling interval. This indicates every how many ticks a TPS sample for calculating recent TPS is taken.
     */
    constexpr const Tick TpsSampleInterval = DefaultTps * 5;


    // ================================================================================
    // =  Exceptions                                                                  =
    // ================================================================================

    /**
     * Base class for all ticking-related exceptions
     */
    M_DECLARE_EXCEPTION(TickingException);

    /**
     * Exception thrown when a method that required calling from main thread is called from non-main thread.
     */
    M_DECLARE_EXCEPTION_EX(NotInMainThreadException, TickingException);

    /**
     * Called when a wait is attempted in a main thread
     */
    M_DECLARE_EXCEPTION_EX(CannotWaitInMainThread, TickingException);

    // ================================================================================
    // =  Task                                                                   =
    // ================================================================================

    /**
     * Represents a task that was scheduled to a ticker.
     */
    class Task : public std::enable_shared_from_this<Task> {
        public: // Constructors & Destructors
            NON_COPYABLE(Task);
            NON_MOVEABLE(Task);

            /**
             * Constructs a new task
             * @param mainThread the main thread of the ticker scheduling the task
             * @param action action to perform
             * @param repeating whether or not this task is repeating
             */
            Task(std::thread::id mainThread, TaskAction action, bool repeating);

            /**
             * Deconstructs this task
             */
            ~Task();

        public: // Public methods
            /**
             * Checks if the task is repeating.
             *
             * @return
             *     Whether the task is repeating or not.
             */
            [[nodiscard]] bool IsRepeating() const noexcept;

            /**
             * Checks if the task has finished execution.
             *
             * @return
             *     Whether the task execution was finished or not.
             */
            [[nodiscard]] bool IsFinished() const noexcept;

            /**
             * Checks whether the task has finished successfully or it failed and an exception was thrown while executing.
             *
             * The behaviour is undefined when the task has not yet finished.
             *
             * @return
             *     Whether the task has finished successfully or not.
             */
            [[nodiscard]] bool IsSuccess() const noexcept;

            /**
             * Waits for the Task to finish if it has not yet finished.
             * Cannot be called from main thread since it would always dead lock the main thread if the task wasn't finished.
             *
             * When the Task is already finished this function does nothing.
             *
             * @throw CannotWaitInMainThread if called from main thread
             */
            void Wait();

            /**
             * Cancels the task if its execution was not yet started.
             * If the task is already done or is executing right now this function does nothing.
             * If the task is repeating it will cancel its future executions.
             */
            void Cancel() noexcept;

        private: // Private methods
            void Execute();

            void Finalize(bool success);

            void NotifyAboutFinish();

            void CancelNoNotify() noexcept;

            [[nodiscard]] bool ShouldWait() const;

        private: // Friends declaration
            friend class Ticker;

        private: // Private fields
            const std::thread::id m_mainThread;
            const TaskAction m_action;
            const bool m_repeating;
            bool m_finished = false;
            bool m_success = false;
            bool m_cancelled = false;
            mutable std::mutex m_waitMutex{};
            std::mutex m_notifyMutex{};
            std::condition_variable m_conditionVariable{};
    };


    // ================================================================================
    // =  Ticker                                                                   =
    // ================================================================================

    /**
     * Represents a Ticker.
     *
     * This is the default implementation of the ITicker interface.
     */
    class Ticker {
        public: // Constructors & Destructors
            NON_COPYABLE(Ticker);
            NON_MOVEABLE(Ticker);

            /**
             * Constructs a new Ticker
             */
            Ticker();

            /**
             * Destructs the ticker
             */
            virtual ~Ticker() noexcept;

        public: // Static methods

            /**
             * Returns a TimeStamp with the current time
             */
            [[nodiscard]] static TimeStamp TimeNow();

        public: // Public methods

            /**
             * Resets all ticker data, including current ticks, all wait data, recent tps and exponents, all pending tasks.
             *
             * Can be called only from the main thread.
             */
            void ResetAll();

            /**
             * Returns the main ticker thread.
             *
             * Some operation may be restricted to only be done from the main thread.
             *
             * \return
             *     The main ticker thread id
             */
            [[nodiscard]] const std::thread::id& GetMainThread() const noexcept;

            /**
             * Checks whether the current thread is the main thread.
             *
             * \return
             *     If the current thread is the main thread.
             */
            [[nodiscard]] bool IsInMainThread() const noexcept;

            /**
             * Throws an exception when the current thread is not the main thread
             *
             * \throw NotInMainThreadException
             */
            void EnsureInMainThread() const;

            /**
             * Gets the TPS that the ticker is supposed to keep.
             *
             * \return
             *     The TPS that the ticker is supposed to keep.
             */
            [[nodiscard]] unsigned int GetTps() const;

            /**
             * Sets the TPS that the ticker is supposed to keep.
             * This also resets the recent TPS and recalculates the sample intervals and exponents.
             *
             * @param[in] tps
             *     The TPS that the ticker is supposed to keep.
             */
            void SetTps(unsigned int tps);

            /**
             * Gets the current tick of the ticker.
             *
             * Can be called only from the main thread.
             *
             * \return
             *     The current tick of the ticker.
             */
            [[nodiscard]] Tick GetCurrentTick() const;

            /**
             * Gets the recent average TPS
             *
             * \see ITicker::RecentTps
             *
             * \return
             *     Recent average TPS
             */
            [[nodiscard]] RecentTps GetRecentTps() const;

            /**
             * Schedules an action to be done in the main thread.
             *
             * When called from a non-main thread this will schedule a task that will be called from the main thread in the next ticker tick.
             *
             * When called from the main thread this will simply call 'action' and return empty pointer.
             *
             * @param[in] action
             *     Function to be called in the main thread. The 'thisTask' argument is the task object, the same that will be returned by the function.
             *
             * @param[in] repeat
             *     Should this task be repeated.
             *     If this is set to true the task will be executed every tick.
             *     If this is set to false the task will be executed only in the next tick.
             *
             * @return
             *     A pointer to the newly created Task or empty pointer if it was called from the main thread.
             *     This task can be used to wait for the action to be completed.
             */
            std::shared_ptr<Task> DoInMainThread(TaskAction action, bool repeat);

            /**
             * Process one tick or sleeps if it is not time yet.
             *
             * Must be called from the main thread.
             */
            void DoTick();

        private: // Private methods
            void RunTasks();

        private: // Private variables
            std::thread::id m_mainThread{};
            std::mutex m_taskMutex{};
            std::vector<std::shared_ptr<Task>> m_tasks{};

            unsigned int m_tps = 0;
            double m_exponents[3] = {0.0};
            double m_recentTps[3] = {0.0};

            TimeStamp m_waitTime = 0;
            TimeStamp m_lastTick = 0;
            TimeStamp m_catchupTime = 0;
            TimeStamp m_tickSection = 0;
            Tick m_currentTick = 0;
    };

}

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TICKER_HPP
