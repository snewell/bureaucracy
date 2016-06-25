#include <bureaucracy/timer.hpp>

#include <algorithm>

using bureaucracy::Timer;

Timer::Timer()
  : my_isAccepting{true},
    my_isRunning{true},
    my_isFiring{false}
{
    my_timerThread = std::thread{[this]() {
        std::unique_lock<std::mutex> lock{my_mutex};

        while(my_isAccepting)
        {
            if(my_futureEvents.empty())
            {
                my_wakeup.wait(lock);
            }
            else
            {
                auto const now = std::chrono::high_resolution_clock::now();
                auto begin = std::begin(my_futureEvents);
                auto const end = std::end(my_futureEvents);
                auto last = std::find_if(begin, end, [now](auto const &event) {
                    return now < event.due;
                });
                if(begin != last)
                {
                    my_isFiring = true;
                    lock.unlock();
                    std::for_each(begin, last, [](auto &event) {
                        event.event();
                    });
                    lock.lock();
                    my_isFiring = false;
                    my_futureEvents.erase(begin, last);
                    std::for_each(std::begin(my_pendingEvents), std::end(my_pendingEvents), [this](auto &event) {
                        auto it = std::find_if(std::begin(my_futureEvents), std::end(my_futureEvents), [&event](auto const &futureEvent) {
                            return event.due < futureEvent.due;
                        });
                        my_futureEvents.emplace(it, std::move(event));
                    });
                    my_pendingEvents.erase(std::begin(my_pendingEvents), std::end(my_pendingEvents));
                }
                else
                {
                    my_wakeup.wait_until(lock, (*begin).due);
                }
            }
        }
    }};
}

Timer::~Timer() noexcept
{
    stop();
}

void Timer::add(Event event,
                Time  due)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        auto addFn = [this](std::vector<TimerEvent> &events, Event event, Time due) {
            auto it = std::find_if(std::begin(events), std::end(events), [due](auto const &event) {
                return due < event.due;
            });
            events.emplace(it, TimerEvent{std::move(event), due});
        };

        if(my_isFiring)
        {
            addFn(my_pendingEvents, std::move(event), due);
        }
        else
        {
            addFn(my_futureEvents, std::move(event), due);
            my_wakeup.notify_one();
        }
    }
}

void Timer::stop()
{
    std::unique_lock<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_isAccepting = false;
        my_wakeup.notify_one();
        lock.unlock();
        my_timerThread.join();
        lock.lock();
        my_isRunning = false;
    }
}

bool Timer::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool Timer::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}