#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/priorityworker.hpp>
#include <bureaucracy/threadpool.hpp>

#include <houseguest/synchronize.hpp>

using bureaucracy::PriorityWorker;
using bureaucracy::Threadpool;

TEST(PriorityWorker, test_ctor) // NOLINT
{
    Threadpool tp{4};
    PriorityWorker pw{tp};

    ASSERT_EQ(true, pw.isAccepting());
    ASSERT_EQ(true, pw.isRunning());
}

TEST(PriorityWorker, test_stop) // NOLINT
{
    Threadpool tp{4};
    PriorityWorker pw{tp};

    pw.stop();
    ASSERT_EQ(false, pw.isAccepting());
    ASSERT_EQ(false, pw.isRunning());
}

TEST(PriorityWorker, test_add) // NOLINT
{
    Threadpool tp{4};
    PriorityWorker pw{tp};

    auto hit = false;
    pw.add([&hit]() { hit = true; });
    pw.stop();
    ASSERT_EQ(true, hit);
}

TEST(PriorityWorker, test_distribute) // NOLINT
{
    Threadpool tp{2};
    PriorityWorker pw{tp};

    std::promise<void> hit1;
    std::promise<void> hit2;

    pw.add([&hit1, &hit2]() {
        hit2.get_future().get();
        hit1.set_value();
    });
    pw.add([&hit2]() { hit2.set_value(); });

    hit1.get_future().get();
}

TEST(PriorityWorker, test_addPriority) // NOLINT
{
    // we're going to use a Threadpool with one thread so we can control when
    // things fire
    Threadpool tp{1};
    PriorityWorker pw{tp};

    auto value = 0;
    std::mutex m;
    houseguest::synchronize(m, [&value, &pw, &m]() {
        pw.add(houseguest::make_synchronize(m, []() {}));
        pw.add(
            [&value]() {
                ASSERT_EQ(5, value);
                value = 10;
            },
            15);
        pw.add(
            [&value]() {
                ASSERT_EQ(0, value);
                value = 5;
            },
            10);
    });
    pw.stop();
    ASSERT_EQ(10, value);
}

TEST(NegativePriorityWorker, test_addStopped) // NOLINT
{
    Threadpool tp{4};
    PriorityWorker pw{tp};

    pw.stop();
    ASSERT_THROW(pw.add([]() {}), std::runtime_error);
}
