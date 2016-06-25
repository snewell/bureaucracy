#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/threadpool.hpp>

using bureaucracy::Threadpool;

TEST(Threadpool, test_ctor)
{
    Threadpool tp{4};

    ASSERT_EQ(true, tp.isAccepting());
    ASSERT_EQ(true, tp.isRunning());
}

TEST(Threadpool, test_stop)
{
    Threadpool tp{4};

    tp.stop();
    ASSERT_EQ(false, tp.isAccepting());
    ASSERT_EQ(false, tp.isRunning());
}

TEST(Threadpool, test_work)
{
    Threadpool tp{4};

    std::promise<int> promise;
    auto result = promise.get_future();

    tp.add([&promise]() {
        promise.set_value(10);
    });

    ASSERT_EQ(10, result.get());
}

TEST(ThreadpoolNegative, test_invalidThreadCount)
{
    ASSERT_THROW(Threadpool tp{0}, std::invalid_argument);
}

TEST(ThreadpoolNegative, test_addStopped)
{
    Threadpool tp{4};
    tp.stop();
    ASSERT_THROW(tp.add([]() { }), std::runtime_error);
}
