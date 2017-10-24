#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/diligentworker.hpp>
#include <bureaucracy/threadpool.hpp>

using bureaucracy::DiligentWorker;
using bureaucracy::Threadpool;

TEST(DiligentWorker, test_ctor) // NOLINT
{
    Threadpool tp{4};
    DiligentWorker dw{tp, []() {}};

    ASSERT_EQ(true, dw.isAccepting());
    ASSERT_EQ(true, dw.isRunning());
}

TEST(DiligentWorker, test_stop) // NOLINT
{
    Threadpool tp{4};
    DiligentWorker dw{tp, []() {}};

    dw.stop();
    ASSERT_EQ(false, dw.isAccepting());
    ASSERT_EQ(false, dw.isRunning());
}

TEST(DiligentWorker, test_add) // NOLINT
{
    Threadpool tp{4};
    DiligentWorker dw{tp, []() {}};

    std::promise<void> hit;
    dw.add([&hit]() { hit.set_value(); });
    hit.get_future().get();
}

TEST(DiligentWorker, test_alert) // NOLINT
{
    Threadpool tp{4};

    std::condition_variable cond;
    std::mutex mtx;
    DiligentWorker dw{tp, [&cond, &mtx]() {
                          std::lock_guard<std::mutex> lock{mtx};
                          cond.notify_one();
                      }};

    std::unique_lock<std::mutex> lock{mtx};
    std::promise<void> hit;
    dw.add([&hit]() { hit.set_value(); });
    hit.get_future().get();

    cond.wait(lock);
}

TEST(NegativeDiligentWorker, test_addStopped) // NOLINT
{
    Threadpool tp{4};
    DiligentWorker dw{tp, []() {}};
    dw.stop();

    ASSERT_THROW(dw.add([]() {}), std::runtime_error);
}
