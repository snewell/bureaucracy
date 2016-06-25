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

TEST(Timer, test_addDue)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() {
        hit.set_value();
    }, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addDelay)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() {
        hit.set_value();
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addSequence)
{
    Timer t;

    auto val = 0;
    t.add([&val]() {
        ASSERT_EQ(0, val);
        val = 10;
    }, std::chrono::milliseconds(100));

    std::promise<void> hit;
    t.add([&val, &hit]() {
        ASSERT_EQ(10, val);
        val = 100;
        hit.set_value();
    }, std::chrono::milliseconds(200));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addReverse)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add([&val, &hit]() {
        ASSERT_EQ(10, val);
        val = 100;
        hit.set_value();
    }, std::chrono::milliseconds(200));

    t.add([&val]() {
        ASSERT_EQ(0, val);
        val = 10;
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addFiring)
{
    Timer t;

    std::promise<void> hit;
    t.add([&t, &hit]() {
        t.add([&hit]() {
            hit.set_value();
        }, std::chrono::milliseconds(100));
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
}

TEST(Timer, test_addFiringSequence)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add([&t, &val, &hit]() {
        t.add([&val]() {
            ASSERT_EQ(0, val);
            val = 10;
        }, std::chrono::milliseconds(100));

        t.add([&val, &hit]() {
            ASSERT_EQ(10, val);
            val = 100;
            hit.set_value();
        }, std::chrono::milliseconds(200));
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}

TEST(Timer, test_addFiringReverse)
{
    Timer t;

    auto val = 0;
    std::promise<void> hit;
    t.add([&t, &val, &hit]() {
        t.add([&val, &hit]() {
            ASSERT_EQ(10, val);
            val = 100;
            hit.set_value();
        }, std::chrono::milliseconds(200));

        t.add([&val]() {
            ASSERT_EQ(0, val);
            val = 10;
        }, std::chrono::milliseconds(100));
    }, std::chrono::milliseconds(100));

    hit.get_future().get();
    ASSERT_EQ(100, val);
}
