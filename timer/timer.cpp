#include <bureaucracy/timer.hpp>

#include <algorithm>
#include <cassert>

using bureaucracy::Timer;

Timer::Timer()
  : my_nextFuture{my_futureEvents.end()}
  , my_nextId{0}
  , my_isAccepting{true}
  , my_isRunning{true}
  , my_isFiring{false}
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
                auto const now = std::chrono::steady_clock::now();
                auto begin = std::begin(my_futureEvents);
                auto const end = std::end(my_futureEvents);
                auto last = std::find_if(begin, end, [now](auto const & event) {
                    return now < event.due;
                });
                if(begin != last)
                {
                    my_isFiring = true;
                    my_nextFuture = last;
                    lock.unlock();
                    std::for_each(begin, last, [this](auto & futureEvent) {
                        auto it = my_events.find(futureEvent.event);
                        assert(it != my_events.end());
                        it->second();
                    });
                    lock.lock();
                    my_isFiring = false;
                    my_futureEvents.erase(begin, last);
                    my_nextFuture = my_futureEvents.begin();
                    std::for_each(
                        std::begin(my_pendingEvents),
                        std::end(my_pendingEvents), [this](auto & event) {
                            auto it =
                                std::find_if(std::begin(my_futureEvents),
                                             std::end(my_futureEvents),
                                             [& event](auto const & futureEvent) {
                                return event.due < futureEvent.due;
                            });
                            my_futureEvents.emplace(it,
                                                    FutureEvent{event.event,
                                                                event.due});
                            my_events.emplace(event.event, std::move(event.fn));
                        });
                    my_pendingEvents.erase(std::begin(my_pendingEvents),
                                           std::end(my_pendingEvents));
                }
                else
                {
                    auto alarm = (*begin).due;
                    my_wakeup.wait_until(lock, alarm);
                }
            }
        }
    }};
}

/// \cond false
Timer::~Timer() noexcept
{
    stop();
}
/// \endcond

Timer::Item Timer::add(Event event, Time due)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        auto id = [this]() {
            auto ret = my_nextId;
            auto it = my_events.find(ret);
            while(it != my_events.end())
            {
                ++ret;
                it = my_events.find(ret);
            }
            my_nextId = ret + 1;
            return ret;
        }();

        if(my_isFiring)
        {
            my_pendingEvents.emplace_back(PendingEvent{id, due,
                                                       std::move(event)});
        }
        else
        {
            auto it = std::find_if(std::begin(my_futureEvents),
                                   std::end(my_futureEvents),
                                   [due](auto const & currentEvent) {
                return due < currentEvent.due;
            });
            my_futureEvents.emplace(it, FutureEvent{id, due});
            my_events.emplace(id, std::move(event));
            my_nextFuture = std::begin(my_futureEvents);
            my_wakeup.notify_one();
        }
        return Item{this, id};
    }
    else
    {
        throw std::runtime_error{"Not accepting"};
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

Timer::Item::CancelStatus Timer::cancel(Timer::Item::Id id)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    // If we're firing, check the pending events since they haven't been merged
    // in yet.
    if(my_isFiring)
    {
        auto const pendingEnd = std::end(my_pendingEvents);
        auto const pendingIt = std::find_if(std::begin(my_pendingEvents),
                                            pendingEnd,
                                            [id](auto const & pendingEvent) {
            return pendingEvent.event == id;
        });
        if(pendingIt != pendingEnd)
        {
            my_pendingEvents.erase(pendingIt);
            return Timer::Item::CancelStatus::CANCELLED;
        }
    }

    // We need to the check the future events whether we're firing or not.
    auto const end = std::end(my_futureEvents);
    auto futureIt = std::find_if(my_nextFuture, end,
                                 [id](auto const & futureEvent) {
        return futureEvent.event == id;
    });
    if(futureIt != end)
    {
        my_futureEvents.erase(futureIt);
        my_events.erase(id);
        return Timer::Item::CancelStatus::CANCELLED;
    }

    // At this point, we didn't find the item in either queue.  It's either an
    // invalid id or queued to fire.
    return Timer::Item::CancelStatus::FAILED;
}

Timer::Timer::Item::Item(Timer * const timer, Id id)
  : my_timer{timer}
  , my_id{id} {}
