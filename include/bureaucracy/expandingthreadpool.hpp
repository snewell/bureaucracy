#ifndef BUREAUCRACY_EXPANDINTHREADPOOL_HPP
#define BUREAUCRACY_EXPANDINTHREADPOOL_HPP 1

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    class ExpandingThreadpool : public Worker
    {
    public:
        ExpandingThreadpool(std::size_t maxThreads,
                            std::size_t maxBacklog);

        ~ExpandingThreadpool() noexcept;

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        std::size_t maxThreads() const noexcept;

        std::size_t spawnedThreads() const noexcept;

    private:
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
