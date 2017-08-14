#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/expandingthreadpool.hpp>

using bureaucracy::ExpandingThreadpool;

TEST(ExpandingThreadpool, test_ctor)
{
    ExpandingThreadpool tp{4, 4};

    ASSERT_EQ(true, tp.isAccepting());
    ASSERT_EQ(true, tp.isRunning());
    ASSERT_EQ(1, tp.spawnedThreads());
    ASSERT_EQ(4, tp.maxThreads());
}

TEST(ExpandingThreadpool, test_stop)
{
    ExpandingThreadpool tp{4, 4};

    tp.stop();
    ASSERT_EQ(false, tp.isAccepting());
    ASSERT_EQ(false, tp.isRunning());
}

TEST(ExpandingThreadpool, test_work)
{
    ExpandingThreadpool tp{4, 4};

    std::promise<int> promise;
    auto result = promise.get_future();

    tp.add([&promise]() { promise.set_value(10); });

    ASSERT_EQ(10, result.get());
}

TEST(ExpandingThreadpool, test_expand)
{
    ExpandingThreadpool tp{2, 2};

    std::promise<void> hit;
    tp.add([&tp, &hit]() {
        // backlog = 0
        tp.add([]() {});
        // backlog = 1
        tp.add([]() {});
        // backlog = 2
        ASSERT_EQ(1, tp.spawnedThreads());
        tp.add([]() {});
        // backlog = 3
        ASSERT_EQ(2, tp.spawnedThreads());

        hit.set_value();
    });

    hit.get_future().get();
    tp.stop();
}

TEST(ExpandingThreadpool, test_expandFail)
{
    ExpandingThreadpool tp{1, 2};

    std::promise<void> hit;
    tp.add([&tp, &hit]() {
        // backlog = 0
        tp.add([]() {});
        // backlog = 1
        tp.add([]() {});
        // backlog = 2
        ASSERT_EQ(1, tp.spawnedThreads());
        tp.add([]() {});
        // backlog = 3
        ASSERT_EQ(1, tp.spawnedThreads());

        hit.set_value();
    });

    hit.get_future().get();
    tp.stop();
}

TEST(NegativeExpandingThreadpool, test_invalidThreadCount)
{
    try
    {
        ExpandingThreadpool{10, 0};
        FAIL();
    }
    catch(std::invalid_argument)
    {
        SUCCEED();
    }
    catch(...)
    {
        FAIL();
    }
}

TEST(NegativeExpandingThreadpool, test_invalidBacklog)
{
    try
    {
        ExpandingThreadpool{0, 10};
        FAIL();
    }
    catch(std::invalid_argument)
    {
        SUCCEED();
    }
    catch(...)
    {
        FAIL();
    }
}

TEST(NegativeExpandingThreadpool, test_addStopped)
{
    ExpandingThreadpool tp{4, 4};
    tp.stop();
    ASSERT_THROW(tp.add([]() {}), std::runtime_error);
}
