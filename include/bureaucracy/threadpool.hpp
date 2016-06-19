#ifndef BUREAUCRACY_THREADPOOL_HPP
#define BUREAUCRACY_THREADPOOL_HPP 1

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace bureaucracy
{
    class Threadpool
    {
    public:
        Threadpool(std::size_t threads);

        ~Threadpool() noexcept;

        Threadpool(const Threadpool &) = delete;
        Threadpool(Threadpool &&) = delete;
        Threadpool& operator = (const Threadpool &) = delete;
        Threadpool& operator = (Threadpool &&) = delete;

        using Work = std::function<void () noexcept>;

        void add(Work work);

        void stop();

        bool isAccepting() const noexcept;

        bool isRunning() const noexcept;

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
