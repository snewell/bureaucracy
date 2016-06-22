#include <bureaucracy/dilligentworker.hpp>

using bureaucracy::DilligentWorker;

DilligentWorker::DilligentWorker(Worker &worker,
                                 Alert   alert)
  : my_worker{&worker},
    my_alert{std::move(alert)},
    my_workQueued{0u},
    my_isAccepting{true},
    my_isRunning{true}
{
}

DilligentWorker::~DilligentWorker() noexcept
{
    stop();
}

void DilligentWorker::add(Work work)
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

void DilligentWorker::stop()
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

bool DilligentWorker::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool DilligentWorker::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}
