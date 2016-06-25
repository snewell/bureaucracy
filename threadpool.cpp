#include <bureaucracy/threadpool.hpp>

#include <algorithm>

using bureaucracy::Threadpool;

namespace
{
    void threadWorker(bool                                   &accepting,
                      std::condition_variable                &workReady,
                      std::mutex                             &mutex,
                      std::vector<bureaucracy::Worker::Work> &work)
    {
        std::unique_lock<std::mutex> lock{mutex};

        while(accepting)
        {
            while(!work.empty())
            {
                auto nextItem = work.front();
                work.erase(std::begin(work));
                lock.unlock();
                nextItem();
                lock.lock();
            }
            workReady.wait(lock);
        }
    }
}

Threadpool::Threadpool(std::size_t threads)
  : my_isAccepting{true},
    my_isRunning{true}
{
    my_threads.reserve(threads);
    for(auto i = 0u; i < threads; ++i)
    {
        my_threads.emplace_back(std::thread{[this] () {
            threadWorker(my_isAccepting, my_workReady, my_mutex, my_work);
        }});
    }
}

Threadpool::~Threadpool() noexcept
{
    stop();
}

void Threadpool::add(Work work)
{
    std::lock_guard<std::mutex> lock{my_mutex};
    my_work.emplace_back(std::move(work));
    my_workReady.notify_one();
}

void Threadpool::stop()
{
    std::unique_lock<std::mutex> lock{my_mutex};
    if(my_isAccepting)
    {
        my_isAccepting = false;
        my_workReady.notify_all();
        lock.unlock();
        std::for_each(std::begin(my_threads), std::end(my_threads), [](auto &thread) {
            thread.join();
        });
        lock.lock();
        my_isRunning = false;
    }
}

bool Threadpool::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool Threadpool::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}
