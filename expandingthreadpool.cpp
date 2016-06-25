#include <bureaucracy/expandingthreadpool.hpp>

#include <algorithm>

using bureaucracy::ExpandingThreadpool;

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
            if(accepting)
            {
                // we may have stopped while calling the work functions, check
                // before waiting
                workReady.wait(lock);
            }
        }
    }
}

ExpandingThreadpool::ExpandingThreadpool(std::size_t maxThreads,
                                         std::size_t maxBacklog)
  : my_maxBacklog{maxBacklog},
    my_isAccepting{true},
    my_isRunning{true}
{
    my_threads.reserve(maxThreads);
    expand();
}

ExpandingThreadpool::~ExpandingThreadpool() noexcept
{
    stop();
}

void ExpandingThreadpool::add(Work work)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_work.emplace_back(std::move(work));
        my_workReady.notify_one();
        auto backlog = my_work.size() / my_threads.size();
        if((backlog > my_maxBacklog) && (my_threads.size() < my_threads.capacity()))
        {
            expand();
        }
    }
    else
    {
        throw std::runtime_error{"ExpandingThreadpool has been stopped"};
    }
}

void ExpandingThreadpool::stop()
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

bool ExpandingThreadpool::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool ExpandingThreadpool::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}

std::size_t ExpandingThreadpool::maxThreads() const noexcept
{
    return my_threads.capacity();
}

std::size_t ExpandingThreadpool::spawnedThreads() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_threads.size();
}

void ExpandingThreadpool::expand()
{
    my_threads.emplace_back(std::thread{[this] () {
        threadWorker(my_isAccepting, my_workReady, my_mutex, my_work);
    }});
}
