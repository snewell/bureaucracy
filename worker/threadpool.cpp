#include <bureaucracy/threadpool.hpp>

using bureaucracy::Threadpool;

Threadpool::Threadpool(std::size_t threads)
  : my_threadpool{threads}
{
    for(auto i = 0u; i < threads; ++i)
    {
        my_threadpool.addThread();
    }
}

/// \cond false
Threadpool::~Threadpool() noexcept
{
    Threadpool::stop();
}
/// \endcond

void Threadpool::add(Work work)
{
    my_threadpool.add(std::move(work));
}

void Threadpool::stop()
{
    my_threadpool.stop();
}

bool Threadpool::isAccepting() const noexcept
{
    return my_threadpool.isAccepting();
}

bool Threadpool::isRunning() const noexcept
{
    return my_threadpool.isRunning();
}
