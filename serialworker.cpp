#include <bureaucracy/serialworker.hpp>

using bureaucracy::SerialWorker;

SerialWorker::SerialWorker(Worker &worker)
  : my_worker{&worker},
    my_isAccepting{true},
    my_isRunning{true},
    my_workQueued{false}
{
}

SerialWorker::~SerialWorker() noexcept
{
    stop();
}

void SerialWorker::add(Work work)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_work.emplace_back(std::move(work));
        if(!my_workQueued)
        {
            my_worker->add([this]() {
                std::unique_lock<std::mutex> lock{my_mutex};
                while(!my_work.empty())
                {
                    auto nextItem = my_work.front();
                    my_work.erase(std::begin(my_work));
                    lock.unlock();
                    nextItem();
                    lock.lock();
                }
                my_workQueued = false;
                my_isEmpty.notify_one();
            });
            my_workQueued = true;
        }
    }
}

void SerialWorker::stop()
{
    std::unique_lock<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        my_isAccepting = false;
        if(my_workQueued)
        {
            my_isEmpty.wait(lock);
        }
        my_isRunning = false;
    }
}

bool SerialWorker::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool SerialWorker::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}
