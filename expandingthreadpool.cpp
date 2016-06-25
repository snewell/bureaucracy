#include <bureaucracy/expandingthreadpool.hpp>

#include <algorithm>

#include "threadpool_internals.hpp"

using bureaucracy::ExpandingThreadpool;

ExpandingThreadpool::ExpandingThreadpool(std::size_t maxThreads,
                                         std::size_t maxBacklog)
  : my_maxBacklog{maxBacklog},
    my_isAccepting{true},
    my_isRunning{true}
{
    if(maxThreads == 0)
    {
        throw std::invalid_argument{"maxThreads must be non-zero"};
    }
    if(maxBacklog == 0)
    {
        throw std::invalid_argument{"maxBacklog must be non-zero"};
    }
    my_threads.reserve(maxThreads);
    expand();
}

/// \cond false
ExpandingThreadpool::~ExpandingThreadpool() noexcept
{
    stop();
}
/// \endcond

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
        throw std::runtime_error{"ExpandingThreadpool is not accepting work"};
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
        bureaucracy::internal::threadWorker(my_isAccepting, my_workReady, my_mutex, my_work);
    }});
}
