#include <bureaucracy/diligentworker.hpp>

using bureaucracy::DiligentWorker;

DiligentWorker::DiligentWorker(Worker &worker,
                                Alert   alert)
  : my_worker{&worker},
    my_alert{std::move(alert)},
    my_workQueued{0u},
    my_isAccepting{true},
    my_isRunning{true}
{
}

DiligentWorker::~DiligentWorker() noexcept
{
    stop();
}

void DiligentWorker::add(Work work)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_worker->add([w = std::move(work), this]() {
            w();
            std::unique_lock<std::mutex> lock{my_mutex};
            --my_workQueued;
            if(my_workQueued == 0)
            {
                lock.unlock();
                my_alert();
                lock.lock();
                if(my_workQueued == 0)
                {
                    my_isEmpty.notify_one();
                }
            }
        });
        ++my_workQueued;
    }
}

void DiligentWorker::stop()
{
    std::unique_lock<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_isAccepting = false;
        if(my_workQueued > 0)
        {
            my_isEmpty.wait(lock);
        }
        my_isRunning = false;
    }
}

bool DiligentWorker::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool DiligentWorker::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}
