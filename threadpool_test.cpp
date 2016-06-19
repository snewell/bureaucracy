#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/threadpool.hpp>

TEST(ThreadpoolTest, test_ctor)
{
    bureaucracy::Threadpool tp{4};

    ASSERT_EQ(true, tp.isAccepting());
    ASSERT_EQ(true, tp.isRunning());
}

TEST(ThreadpoolTest, test_stop)
{
    bureaucracy::Threadpool tp{4};

    tp.stop();
    ASSERT_EQ(false, tp.isAccepting());
    ASSERT_EQ(false, tp.isRunning());
}

TEST(ThreadpoolTest, test_work)
{
    bureaucracy::Threadpool tp{4};

    std::promise<int> promise;
    auto result = promise.get_future();

    tp.add([&promise]() {
        promise.set_value(10);
    });

    ASSERT_EQ(10, result.get());
}
