#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/timer.hpp>

using bureaucracy::Timer;

TEST(Timer, test_ctor)
{
    Timer t;
    ASSERT_EQ(true, t.isAccepting());
    ASSERT_EQ(true, t.isRunning());
}

TEST(Timer, test_stop)
{
    Timer t;

    t.stop();
    ASSERT_EQ(false, t.isAccepting());
    ASSERT_EQ(false, t.isRunning());
}

TEST(Timer, test_addPostStop)
{
    Timer t;

    t.stop();
    auto addFn = [&t]() {
        t.add([]() { }, std::chrono::milliseconds(0));
    };
    ASSERT_THROW(addFn(), std::runtime_error);
}

TEST(Timer, test_addDue)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::steady_clock::now() + std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDueSystem)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::system_clock::now() + std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDueHighResolution)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::high_resolution_clock::now() +
              std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDelay)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); }, std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addSequence)
{
    Timer t;

    auto val = 0;
    t.add(
        [&val]() {
            ASSERT_EQ(0, val);
            val = 10;
        },
        std::chrono::milliseconds(100));

    std::promise<void> hit;
    t.add(
        [&val, &hit]() {
            ASSERT_EQ(10, val);
            val = 100;
            hit.set_value();
        },
        std::chrono::milliseconds(200));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addReverse)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add(
        [&val, &hit]() {
            ASSERT_EQ(10, val);
            val = 100;
            hit.set_value();
        },
        std::chrono::milliseconds(200));

    t.add(
        [&val]() {
            ASSERT_EQ(0, val);
            val = 10;
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addFiring)
{
    Timer t;

    std::promise<void> hit;
    t.add(
        [&t, &hit]() {
            t.add([&hit]() { hit.set_value(); },
                  std::chrono::milliseconds(100));
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addFiringSequence)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add(
        [&t, &val, &hit]() {
            t.add(
                [&val]() {
                    ASSERT_EQ(0, val);
                    val = 10;
                },
                std::chrono::milliseconds(100));

            t.add(
                [&val, &hit]() {
                    ASSERT_EQ(10, val);
                    val = 100;
                    hit.set_value();
                },
                std::chrono::milliseconds(200));
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addFiringReverse)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add(
        [&t, &val, &hit]() {
            t.add(
                [&val, &hit]() {
                    ASSERT_EQ(10, val);
                    val = 100;
                    hit.set_value();
                },
                std::chrono::milliseconds(200));

            t.add(
                [&val]() {
                    ASSERT_EQ(0, val);
                    val = 10;
                },
                std::chrono::milliseconds(100));
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_cancelNormal)
{
    Timer t;

    auto item = t.add([]() { }, std::chrono::milliseconds(100));
    ASSERT_EQ(Timer::Item::CancelStatus::CANCELLED, item.cancel());
}

TEST(Timer, test_cancelFailed)
{
    Timer t;

    std::promise<void> hit;
    auto item = t.add([&hit]() {
        hit.set_value();
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::FAILED, item.cancel());
}

TEST(Timer, test_cancelFiring)
{
    Timer t;

    auto item = t.add([]() { }, std::chrono::seconds(100));

    auto cancelStatus = Timer::Item::CancelStatus::FAILED;
    std::promise<void> hit;
    t.add([&hit, &item, &cancelStatus]() {
        cancelStatus = item.cancel();
        hit.set_value();
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::CANCELLED, cancelStatus);
}

TEST(Timer, test_cancelPending)
{
    Timer t;

    auto cancelStatus = Timer::Item::CancelStatus::FAILED;
    std::promise<void> hit;
    t.add([&hit, &t, &cancelStatus]() {
        auto item = t.add([]() { }, std::chrono::seconds(100));
        cancelStatus = item.cancel();
        hit.set_value();
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::CANCELLED, cancelStatus);
}
