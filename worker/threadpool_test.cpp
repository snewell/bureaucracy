#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/threadpool.hpp>

using bureaucracy::Threadpool;

TEST(Threadpool, test_ctor) // NOLINT
{
    Threadpool tp{4};

    ASSERT_EQ(true, tp.isAccepting());
    ASSERT_EQ(true, tp.isRunning());
}

TEST(Threadpool, test_stop) // NOLINT
{
    Threadpool tp{4};

    tp.stop();
    ASSERT_EQ(false, tp.isAccepting());
    ASSERT_EQ(false, tp.isRunning());
}

TEST(Threadpool, test_work) // NOLINT
{
    Threadpool tp{4};

    std::promise<int> promise;
    auto result = promise.get_future();

    tp.add([&promise]() { promise.set_value(10); });

    ASSERT_EQ(10, result.get());
}

TEST(NegativeThreadpool, test_invalidThreadCount) // NOLINT
{
    ASSERT_THROW(Threadpool{0}, std::invalid_argument);
}

TEST(NegativeThreadpool, test_addStopped) // NOLINT
{
    Threadpool tp{4};
    tp.stop();
    ASSERT_THROW(tp.add([]() {}), std::runtime_error);
}
