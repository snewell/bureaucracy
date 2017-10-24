#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/timer.hpp>

using bureaucracy::Timer;

TEST(Timer, test_ctor) // NOLINT
{
    Timer t;
    ASSERT_EQ(true, t.isAccepting());
    ASSERT_EQ(true, t.isRunning());
}

TEST(Timer, test_stop) // NOLINT
{
    Timer t;

    t.stop();
    ASSERT_EQ(false, t.isAccepting());
    ASSERT_EQ(false, t.isRunning());
}

TEST(Timer, test_addPostStop) // NOLINT
{
    Timer t;

    t.stop();
    auto addFn = [&t]() { t.add([]() {}, std::chrono::milliseconds(0)); };
    ASSERT_THROW(addFn(), std::runtime_error);
}

TEST(Timer, test_addDue) // NOLINT
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::steady_clock::now() + std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDueSystem) // NOLINT
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::system_clock::now() + std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDueHighResolution) // NOLINT
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); },
          std::chrono::high_resolution_clock::now() +
              std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDelay) // NOLINT
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() { hit.set_value(); }, std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addSequence) // NOLINT
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

TEST(Timer, test_addReverse) // NOLINT
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

TEST(Timer, test_addFiring) // NOLINT
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

TEST(Timer, test_addFiringSequence) // NOLINT
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

TEST(Timer, test_addFiringReverse) // NOLINT
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

TEST(Timer, test_cancelNormal) // NOLINT
{
    Timer t;

    auto item = t.add([]() {}, std::chrono::milliseconds(100));
    ASSERT_EQ(Timer::Item::CancelStatus::cancelled, t.cancel(item));
}

TEST(Timer, test_cancelFailed) // NOLINT
{
    Timer t;

    std::promise<void> hit;
    auto item =
        t.add([&hit]() { hit.set_value(); }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::failed, t.cancel(item));
}

TEST(Timer, test_cancelFiring) // NOLINT
{
    Timer t;

    auto item = t.add([]() {}, std::chrono::seconds(100));

    auto cancelStatus = Timer::Item::CancelStatus::failed;
    std::promise<void> hit;
    t.add(
        [&hit, &t, item, &cancelStatus]() {
            cancelStatus = t.cancel(item);
            hit.set_value();
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::cancelled, cancelStatus);
}

TEST(Timer, test_cancelPending) // NOLINT
{
    Timer t;

    auto cancelStatus = Timer::Item::CancelStatus::failed;
    std::promise<void> hit;
    t.add(
        [&hit, &t, &cancelStatus]() {
            auto item = t.add([]() {}, std::chrono::seconds(100));
            cancelStatus = t.cancel(item);
            hit.set_value();
        },
        std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(Timer::Item::CancelStatus::cancelled, cancelStatus);
}
