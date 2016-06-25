#ifndef BUREAUCRACY_THREADPOOL_HPP
#define BUREAUCRACY_THREADPOOL_HPP 1

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    /** \brief A traditional threadpool
     *
     * Threadpool distributes Work among a set of threads.  Work is guaranteed
     * to be invoked in same order it was added to the Threadpool but does not
     * guarantee Work will complete in any particular order.
     */
    class Threadpool : public Worker
    {
    public:
        /** \brief Construct a Threadpool with \p threads threads
         *
         * \param [in] threads
         *      the number of threads to use
         *
         * \exception std::invalid_argument
         *      an invalid size was provided for \p threads
         *
         * \exception std::exception
         *      an exception was emitted from the standard libray
         */
        Threadpool(std::size_t threads);

        /** \brief Add Work to the end of the queue
         *
         * \param [in] work
         *      a piece of Work to execute
         *
         * \exception std::runtime_error
         *      the Threadpool is not accepting Work
         *
         * \exception std::exception
         *      The stanard library may emit exceptions.
         */
        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        /// \cond false
        ~Threadpool() noexcept;

        Threadpool(const Threadpool &) = delete;
        Threadpool(Threadpool &&) = delete;
        Threadpool& operator = (const Threadpool &) = delete;
        Threadpool& operator = (Threadpool &&) = delete;
        /// @endcond

    private:
        std::vector<std::thread> my_threads;
        std::vector<Work> my_work;

        std::condition_variable my_workReady;
        mutable std::mutex my_mutex;

        bool my_isAccepting;
        bool my_isRunning;
    };
}

#endif
