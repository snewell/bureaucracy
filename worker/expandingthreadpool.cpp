#include <bureaucracy/expandingthreadpool.hpp>

using bureaucracy::ExpandingThreadpool;

ExpandingThreadpool::ExpandingThreadpool(std::size_t maxThreads,
                                         std::size_t maxBacklog)
  : my_threadpool{maxThreads}
  , my_maxBacklog{maxBacklog}
{
    if(maxBacklog == 0)
    {
        throw std::invalid_argument{"Invalid value for maxBacklog"};
    }
    my_threadpool.addThread();
}

/// \cond false
ExpandingThreadpool::~ExpandingThreadpool() noexcept
{
    ExpandingThreadpool::stop();
}
/// \endcond

void ExpandingThreadpool::add(Work work)
{
    my_threadpool.add(std::move(work));
    my_threadpool.addThreadIf(
        [this](auto const & queuedWork, auto const & threads) {
            if(threads.size() < threads.capacity())
            {
                auto const backlog = queuedWork.size() / threads.size();
                return backlog > my_maxBacklog;
            }
            return false;
        });
}

void ExpandingThreadpool::stop()
{
    my_threadpool.stop();
}

bool ExpandingThreadpool::isAccepting() const noexcept
{
    return my_threadpool.isAccepting();
}

bool ExpandingThreadpool::isRunning() const noexcept
{
    return my_threadpool.isRunning();
}

std::size_t ExpandingThreadpool::maxThreads() const noexcept
{
    return my_threadpool.getMaxThreads();
}

std::size_t ExpandingThreadpool::spawnedThreads() const noexcept
{
    return my_threadpool.getAllocatedThreads();
}
