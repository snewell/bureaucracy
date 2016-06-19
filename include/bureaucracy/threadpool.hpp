#ifndef BUREAUCRACY_THREADPOOL_HPP
#define BUREAUCRACY_THREADPOOL_HPP 1

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    class Threadpool : public Worker
    {
    public:
        Threadpool(std::size_t threads);

        ~Threadpool() noexcept;

        Threadpool(const Threadpool &) = delete;
        Threadpool(Threadpool &&) = delete;
        Threadpool& operator = (const Threadpool &) = delete;
        Threadpool& operator = (Threadpool &&) = delete;

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

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
