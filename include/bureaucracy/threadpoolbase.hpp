#ifndef BUREAUCRACY_THREADPOOLBASE_HPP
#define BUREAUCRACY_THREADPOOLBASE_HPP 1

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <bureaucracy/worker.hpp>

#include <houseguest/synchronize.hpp>

namespace bureaucracy
{
    /** \internal
     *
     * ThreadpoolBase contains common functionality required by the threadpool
     * classes.  It's designed for internal use only and there are no
     * guarantees about long term API stability.
     *
     * \cond false
     */
    class ThreadpoolBase
    {
    public:
        explicit ThreadpoolBase(std::size_t maxThreads);

        void add(Worker::Work work);

        void stop();

        bool isAccepting() const noexcept;

        bool isRunning() const noexcept;

        void addThread();

        template <typename PREDICATE>
        void addThreadIf(PREDICATE const & pred);

        std::size_t getMaxThreads() const noexcept;

        std::size_t getAllocatedThreads() const noexcept;

        ~ThreadpoolBase() noexcept;
        ThreadpoolBase(ThreadpoolBase const &);
        ThreadpoolBase(ThreadpoolBase &&) noexcept;
        ThreadpoolBase & operator=(ThreadpoolBase const &);
        ThreadpoolBase & operator=(ThreadpoolBase &&) noexcept;

    private:
        std::vector<std::thread> my_threads;
        std::vector<Worker::Work> my_work;

        std::condition_variable my_workReady;
        mutable std::mutex my_mutex;

        bool my_isAccepting;
        bool my_isRunning;
    };

    template <typename PREDICATE>
    inline void ThreadpoolBase::addThreadIf(PREDICATE const & pred)
    {
        houseguest::synchronize(my_mutex, [this, &pred]() {
            if(pred(my_work, my_threads))
            {
                addThread();
            }
        });
    }
    /// \endcond
} // namespace bureaucracy

#endif
