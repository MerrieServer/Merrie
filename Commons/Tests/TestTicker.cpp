#include <gtest/gtest.h>
#include <Commons/Ticker.hpp>

using namespace Merrie;

TEST(TickerTest, TestThreads)
{
    Ticker ticker;

    EXPECT_EQ(std::this_thread::get_id(), ticker.GetMainThread()) << "Ticker's main thread was not set to the thread that called the constructor";

    std::thread testThread1([&ticker]() {
        EXPECT_NE(std::this_thread::get_id(), ticker.GetMainThread()) << "Ticker reported being in main thread from invalid thread";
    });
    testThread1.join();

    EXPECT_NO_THROW(ticker.EnsureInMainThread())
                        << "Ticker threw NotInMainThreadException in the main thread";

    std::thread testThread2([&ticker]() {
        EXPECT_THROW(ticker.EnsureInMainThread(), NotInMainThreadException) << "Ticker did not throw NotInMainThreadException in a non-main thread";
    });

    testThread2.join();
}

TEST(TickerTest, TestTPSSet)
{
    Ticker ticker;
    EXPECT_EQ(DefaultTps, ticker.GetTps()) << "Ticker did not set the default TPS when constructed";

    constexpr const unsigned int newTps = 187;
    ticker.SetTps(newTps);
    EXPECT_EQ(newTps, ticker.GetTps()) << "Ticker's target TPS did not change";

    RecentTps recentTps = ticker.GetRecentTps();
    EXPECT_TRUE(recentTps.Last1Minute == newTps && recentTps.Last5Minutes == newTps && recentTps.Last15Minutes == newTps) << "Ticker's recent TPS were not reset";
}

TEST(TickerTest, TestTasks)
{
    Ticker ticker;
    bool complete = false;

    std::thread otherThread([&]() {
        EXPECT_FALSE(ticker.IsInMainThread()) << "Ticker reported being in main thread from invalid thread";

        ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            EXPECT_TRUE(ticker.IsInMainThread()) << "Ticker reported being in non-main from a task";

            complete = true;
        }, false);
    });

    if (otherThread.joinable())
    {
        otherThread.join();
    }

    while (!complete)
    {
        ticker.DoTick();
    }
}


TEST(TickerTest, TestTicks)
{
    Ticker ticker;
    EXPECT_EQ(0u, ticker.GetCurrentTick()) << "Ticker current tick was not initialised to 0";

    int tickCount = 0;
    const std::shared_ptr<Task> countingTask = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
        tickCount++;
    }, true);
    tickCount = 0;

    while (ticker.GetCurrentTick() != 1)
    {
        ticker.DoTick();
    }

    EXPECT_EQ(1, tickCount) << "Ticker does not counts ticks correctly";

    constexpr const size_t repetitions = 45;

    tickCount = 0;
    while (ticker.GetCurrentTick() < repetitions + 1)
    {
        ticker.DoTick();
    }

    EXPECT_EQ(repetitions, tickCount) << "Ticker does not counts ticks correctly";
}

TEST(TickerTest, TestWaitingForTasks)
{
    bool end = false;
    Ticker ticker;

    std::thread otherThread([&]() {
        const std::shared_ptr<Task> task = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            end = true;
        }, false);

        task->Wait();

        EXPECT_TRUE(task->IsFinished()) << "Wait finished until task has completed";
        EXPECT_TRUE(end) << "Wait finished until the task has completed";
    });

    while (!end)
    {
        ticker.DoTick();
    }

    if (otherThread.joinable())
    {
        otherThread.join();
    }
}

TEST(TickerTest, TestTaskSuccess)
{
    bool finished = false;
    Ticker ticker;

    std::thread otherThread([&]() {
        const std::shared_ptr<Task> successTask = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }, false);

        const std::shared_ptr<Task> failTask = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            throw std::exception();
        }, false);

        successTask->Wait();
        failTask->Wait();

        EXPECT_TRUE(successTask->IsSuccess()) << "Task that was to supposed to success failed";
        EXPECT_FALSE(failTask->IsSuccess()) << "Tasks that was supposed to fail finished successfully";

        finished = true;
    });
    otherThread.detach();

    while (!finished)
    {
        ticker.DoTick();
    }
}

TEST(TickerTest, TestCancel)
{
    Ticker ticker;

    std::shared_ptr<Task> task;
    bool done = false;

    std::thread otherThread([&]() {
        task = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            done = true;
        }, false);

        task->Cancel();
    });

    if (otherThread.joinable())
    {
        otherThread.join();
    }

    ticker.DoTick();

    EXPECT_FALSE(done) << "Task was called even though it was cancelled";
    EXPECT_FALSE(task->IsFinished()) << "Task was marked as finished";
}

TEST(TickerTest, TestRepeats)
{
    Ticker ticker;

    std::shared_ptr<Task> repeatingTask;
    int nonRepeatingCounter = 0;
    int repeatingCounter = 0;

    std::thread otherThread([&]() {
        const std::shared_ptr<Task> normalTask = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            nonRepeatingCounter++;
        }, false);

        EXPECT_FALSE(normalTask->IsRepeating()) << "Normal task was marked as repeating";

        repeatingTask = ticker.DoInMainThread([&](const std::shared_ptr<Task>&) {
            repeatingCounter++;
        }, true);

        EXPECT_TRUE(repeatingTask->IsRepeating()) << "Repeating task was marked as non-repeating";
    });

    if (otherThread.joinable())
    {
        otherThread.join();
    }

    constexpr const size_t repetitions = 15;

    while (ticker.GetCurrentTick() != repetitions)
    {
        ticker.DoTick();
    }

    EXPECT_EQ(1, nonRepeatingCounter) << "Non-repeating task called more than once (or never)";
    EXPECT_EQ(repetitions, repeatingCounter) << "Repeating task was not called every tick";

    repeatingTask->Cancel();
    ticker.DoTick();
    ticker.DoTick();

    EXPECT_EQ(repetitions, repeatingCounter) << "Repeating task was called after being cancelled";
}