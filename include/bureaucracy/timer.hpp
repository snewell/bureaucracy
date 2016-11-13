#ifndef BUREAUCRACY_TIMER_HPP
#define BUREAUCRACY_TIMER_HPP 1

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace bureaucracy
{
    /** \brief A class that triggers Events at certain times.
     *
     * A Timer manages a list of Events that should be fired at specific
     * times.  Timer will invoke an Event as close to the requested time as
     * possible but makes no guarantees regarding the level of precision or
     * delta.
     *
     * \warning The precision of Timer is dependent on the implementation of
     *          the chrono library.  This should be sufficient for most uses
     *          but projects that require precise timing should use a
     *          different timer implementation.
     */
    class Timer
    {
    public:
        /** \brief A function a Timer can invoke.
         *
         * \warning An Event should ideally be a very minimal function that
         *          cannot block to avoid delaying firing of other Events.  If
         *          an Event requires complex work consider using the Event to
         *          feed work into a Worker.
         */
        using Event = std::function<void () noexcept>;

        /** \brief A point in time when an Event can be invoked.
         */
        using Time = std::chrono::time_point<std::chrono::steady_clock>;

        /** \brief Construct a Timer
         */
        Timer();

        /** \brief Add an Event that fires at a specific time.
         *
         * Add \p event to the Timer and invoke it as close to \p due as
         * possible.
         *
         * \param [in] event
         *      an Event to fire when it's \p due
         *
         * \param [in] due
         *      an exact (not relative) time to invoke \p event
         *
         * \note If \p due is in the past \p event will be fired the next time
         *       the Timer executes Events.
         */
        void add(Event event,
                 Time  due);

        /** \brief Add an Event that fires at a specific time.
         *
         * Add \p event to the Timer and invoke it as close to \p due as
         * possible.  This is a helper function if \p due is not a Time.
         *
         * \param [in] event
         *      an Event to fire when it's \p due
         *
         * \param [in] due
         *      An exact (not relative) time to invoke \p event.  This will be
         *      converted to a Time.
         *
         * \note If \p due is in the past \p event will be fired the next time
         *       the Timer executes Events.
         */
        template <typename CLOCK>
        void add(Event                          event,
                 std::chrono::time_point<CLOCK> due);

        /** \brief Add an Event that fires after a delay.
         *
         * Add \p event that fires in \p delay.  This function is equivalent
         * to using the the Time version of add where \p due is `now() + due`.
         *
         * \param [in] event
         *      an Event to fire after \p delay time
         *
         * \param [in] delay
         *      a duration to wait
         *
         * \note If \p delay is negative \p event will be fired the next time
         *       the Timer executes Events.
         */
        template <typename ...ARGS>
        void add(Event                          event,
                 std::chrono::duration<ARGS...> delay);

        /** \brief Stop accepting Events and terminate the Timer thread
         *
         * \note Any scheduled Events will _not_ be called.
         *
         * \warning Calling stop from multiple threads is undefined behavior.
         *
         * \warning Calling stop from an Event invoked by that Timer is
         *          undefined behavior.
         */
        void stop();

        /** \brief Determine if this Timer is accepting new Events.
         *
         * \retval true this Timer is accepting new Events
         * \retval false this Timer is not accepting new Events
         */
        bool isAccepting() const noexcept;

        /** \brief Determine if this Timer's thread is running.
         *
         * \retval true this Timer's thread is running
         * \retval false this Timer's thread is not running
         */
        bool isRunning() const noexcept;

        /// \cond false
        ~Timer() noexcept;
        Timer(Timer const &) = delete;
        Timer(Timer &&) noexcept = delete;
        Timer& operator = (Timer const &) = delete;
        Timer& operator = (Timer &&) = delete;
        /// \endcond

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

    template <typename CLOCK>
    inline void Timer::add(Event                          event,
                           std::chrono::time_point<CLOCK> due)
    {
        auto const delay = due - CLOCK::now();
        add(std::move(event), delay);
    }

    template <typename ...ARGS>
    inline void Timer::add(Event                          event,
                           std::chrono::duration<ARGS...> delay)
    {
        add(std::move(event), std::chrono::steady_clock::now() + delay);
    }
}

#endif
