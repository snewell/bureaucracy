#ifndef BUREAUCRACY_TIMER_HPP
#define BUREAUCRACY_TIMER_HPP 1

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace bureaucracy
{
    class Timer
    {
    public:
        Timer();

        ~Timer() noexcept;

        using Event = std::function<void () noexcept>;

        using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;

        void add(Event event,
                 Time  due);

        void stop();

        bool isAccepting() const noexcept;

        bool isRunning() const noexcept;

    private:
        std::thread my_timerThread;
        mutable std::mutex my_mutex;
        std::condition_variable my_wakeup;

        struct TimerEvent
        {
            Event event;
            Time due;
        };

        std::vector<TimerEvent> my_futureEvents;
        std::vector<TimerEvent> my_pendingEvents;

        bool my_isAccepting;
        bool my_isRunning;
        bool my_isFiring;
    };
}

#endif
