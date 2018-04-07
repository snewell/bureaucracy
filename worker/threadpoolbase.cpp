#include <bureaucracy/threadpoolbase.hpp>

#include <algorithm>

#include <houseguest/synchronize.hpp>

using bureaucracy::ThreadpoolBase;

namespace
{
    void threadWorker(bool & accepting, std::condition_variable & workReady,
                      std::mutex & mutex,
                      std::vector<bureaucracy::Worker::Work> & work)
    {
        houseguest::synchronize_unique(
            mutex, [&accepting, &workReady, &work](auto lock) {
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
                        // we may have stopped while calling the work functions,
                        // check before waiting
                        workReady.wait(lock);
                    }
                }
            });
    }
} // namespace

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
    houseguest::synchronize(my_mutex, [this, &work]() {
        if(my_isAccepting)
        {
            my_work.emplace_back(std::move(work));
            my_workReady.notify_one();
        }
        else
        {
            throw std::runtime_error{"Not accepting work"};
        }
    });
}

void ThreadpoolBase::stop()
{
    houseguest::synchronize_unique(my_mutex, [this](auto lock) {
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
    });
}

bool ThreadpoolBase::isAccepting() const noexcept
{
    return houseguest::synchronize(my_mutex,
                                   [this]() { return my_isAccepting; });
}

bool ThreadpoolBase::isRunning() const noexcept
{
    return houseguest::synchronize(my_mutex, [this]() { return my_isRunning; });
}

std::size_t ThreadpoolBase::getMaxThreads() const noexcept
{
    return houseguest::synchronize(my_mutex,
                                   [this]() { return my_threads.capacity(); });
}

std::size_t ThreadpoolBase::getAllocatedThreads() const noexcept
{
    return houseguest::synchronize(my_mutex,
                                   [this]() { return my_threads.size(); });
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
