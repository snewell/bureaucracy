#ifndef BUREAUCRACY_EXPANDINTHREADPOOL_HPP
#define BUREAUCRACY_EXPANDINTHREADPOOL_HPP 1

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    /** \brief A threadpool that spawns threads based on load.
     *
     * An ExpandingThreadpool will distribute work among threads, spawning
     * additional workers as necessary.
     */
    class ExpandingThreadpool : public Worker
    {
    public:
        /** \brief Construct an ExpandingThreadpool.
         *
         * \param [in] maxThreads
         *      the maximum number of threads the ExpandingThreadpool will
         *      ever spawn
         *
         * \param [in] maxBacklog
         *      the maximum backlog of Work per thread before a new thread is
         *      spawned
         *
         * \exception std:invalid_argument
         *      either \p maxThreads or \p maxBacklog is an invalid value
         *
         * \exception std::exception
         *      an exception was emitted by the standard library
         */
        ExpandingThreadpool(std::size_t maxThreads,
                            std::size_t maxBacklog);

        /** \brief Add Work to the ExpandingThreadpool
         *
         * This function may result in a new thread being spawned based on
         * current load.  A new thread is spawned if both of the following
         * conditions are true:
         *   - there is more than \p maxBacklog work queued for each spawned
         *     thread
         *   - fewer than \p maxThreads threads have been spawned
         *
         * The work will be queued if possible even if a thread cannot be
         * spawned.
         *
         * \param [in] work
         *      a piece of Work
         *
         * \exception std::runtime_error
         *      the ExpandingThreadpool is not accepting Work
         *
         * \exception std::exception
         *      an exception was emitted by the standard library
         */
        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        /** \brief Retrieve the maximum number of threads in this
         *         ExpandingThreadpool.
         *
         * \return the maximum number of threads this ExpandingThreadpool can
         *         spawn
         */
        std::size_t maxThreads() const noexcept;

        /** \brief Retrieve the number of threads spawned by this
         *         ExpandingThreadpool.
         *
         * \return the current number of threads spawned by this
         *         ExpandingThreadpool
         */
        std::size_t spawnedThreads() const noexcept;

        /// \cond false
        ~ExpandingThreadpool() noexcept;
        ExpandingThreadpool(ExpandingThreadpool const &) = delete;
        ExpandingThreadpool(ExpandingThreadpool &&) noexcept = delete;
        ExpandingThreadpool& operator = (ExpandingThreadpool const &) = delete;
        ExpandingThreadpool& operator = (ExpandingThreadpool &&) noexcept = delete;
        /// \endcond

    private:
        /** \internal
         */
        void expand();

        std::vector<std::thread> my_threads;
        std::vector<Work> my_work;

        std::condition_variable my_workReady;
        mutable std::mutex my_mutex;

        std::size_t const my_maxBacklog;

        bool my_isAccepting;
        bool my_isRunning;
    };
}

#endif
