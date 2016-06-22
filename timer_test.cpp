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

TEST(Timer, test_add)
{
    Timer t;

    std::promise<void> hit;
    t.add([&hit]() {
        hit.set_value();
    }, std::chrono::high_resolution_clock::now() + std::chrono::seconds(2) );

    hit.get_future().get();
}

TEST(Timer, test_addFiring)
{
    Timer t;

    std::promise<void> hit;
    t.add([&t, &hit]() {
        t.add([&hit]() {
            hit.set_value();
        }, std::chrono::high_resolution_clock::now() + std::chrono::seconds(2) );
    }, std::chrono::high_resolution_clock::now() + std::chrono::seconds(2) );

    hit.get_future().get();
}
