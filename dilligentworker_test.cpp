#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/dilligentworker.hpp>
#include <bureaucracy/threadpool.hpp>

using bureaucracy::DilligentWorker;
using bureaucracy::Threadpool;

TEST(DilligentWorker, test_ctor)
{
    Threadpool tp{4};
    DilligentWorker dw{tp, []() { }};

    ASSERT_EQ(true, dw.isAccepting());
    ASSERT_EQ(true, dw.isRunning());
}

TEST(DilligentWorker, test_stop)
{
    Threadpool tp{4};
    DilligentWorker dw{tp, []() { }};

    dw.stop();
    ASSERT_EQ(false, dw.isAccepting());
    ASSERT_EQ(false, dw.isRunning());
}

TEST(DilligentWorker, test_add)
{
    Threadpool tp{4};
    DilligentWorker dw{tp, []() { }};

    std::promise<void> hit;
    dw.add([&hit]() {
        hit.set_value();
    });
    hit.get_future().get();
}

TEST(DilligentWorker, test_alert)
{
    Threadpool tp{4};

    std::condition_variable cond;
    std::mutex mtx;
    DilligentWorker dw{tp, [&cond, &mtx]() {
        std::lock_guard<std::mutex> lock{mtx};
        cond.notify_one();
    }};

    std::unique_lock<std::mutex> lock{mtx};
    std::promise<void> hit;
    dw.add([&hit]() {
        hit.set_value();
    });
    hit.get_future().get();

    cond.wait(lock);
}
