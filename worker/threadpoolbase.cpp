#include <bureaucracy/threadpoolbase.hpp>

#include <algorithm>

using bureaucracy::ThreadpoolBase;

namespace
{
    void threadWorker(bool & accepting, std::condition_variable & workReady,
                      std::mutex & mutex,
                      std::vector<bureaucracy::Worker::Work> & work)
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

/// \cond false
ThreadpoolBase::ThreadpoolBase(std::size_t maxThreads)
  : my_isAccepting{true}
  , my_isRunning{true}
{
    if(maxThreads == 0)
    {
        throw std::invalid_argument{"Invalid thread count"};
    }
    my_threads.reserve(maxThreads);
}

/// \cond false
ThreadpoolBase::~ThreadpoolBase() noexcept
{
    stop();
}
/// \endcond

void ThreadpoolBase::add(Worker::Work work)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_work.emplace_back(std::move(work));
        my_workReady.notify_one();
    }
    else
    {
        throw std::runtime_error{"Not accepting work"};
    }
}

void ThreadpoolBase::stop()
{
    std::unique_lock<std::mutex> lock{my_mutex};
    if(my_isAccepting)
    {
        my_isAccepting = false;
        my_workReady.notify_all();
        lock.unlock();
        std::for_each(std::begin(my_threads), std::end(my_threads),
                      [](auto & thread) { thread.join(); });
        lock.lock();
        my_isRunning = false;
    }
}

bool ThreadpoolBase::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool ThreadpoolBase::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}

std::size_t ThreadpoolBase::getMaxThreads() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_threads.capacity();
}

std::size_t ThreadpoolBase::getAllocatedThreads() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_threads.size();
}

void ThreadpoolBase::addThread()
{
    // assumes it's safe to add a thread here
    if(my_threads.size() != my_threads.capacity())
    {
        my_threads.emplace_back(std::thread{[this]() {
            threadWorker(my_isAccepting, my_workReady, my_mutex, my_work);
        }});
    }
    else
    {
        throw std::runtime_error{"threads are at capacity"};
    }
}
/// \endcond
