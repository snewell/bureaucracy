#include <bureaucracy/priorityworker.hpp>

#include <algorithm>

using bureaucracy::PriorityWorker;

PriorityWorker::PriorityWorker(Worker   &worker,
                               Priority  defaultPriority)
  : my_worker{&worker},
    my_defaultPriority{defaultPriority},
    my_isAccepting{true},
    my_isRunning{true},
    my_workQueued{false}
{
}

PriorityWorker::~PriorityWorker() noexcept
{
    stop();
}

void PriorityWorker::add(Work     work,
                         Priority priority)
{
    std::lock_guard<std::mutex> lock{my_mutex};

    if(my_isAccepting)
    {
        auto it = std::find_if(std::begin(my_work), std::end(my_work), [priority](auto const &p) {
            return priority < p.first;
        });
        my_work.emplace(it, std::make_pair(priority, std::move(work)));
        my_worker->add([this] () {
            std::unique_lock<std::mutex> lock{my_mutex};
            auto nextItem = my_work.front();
            my_work.erase(std::begin(my_work));
            lock.unlock();
            nextItem.second();
            lock.lock();
            if(my_work.empty())
            {
                my_isEmpty.notify_one();
                my_workQueued = false;
            }
        });
        my_workQueued = true;
    }
}

void PriorityWorker::stop()
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

bool PriorityWorker::isAccepting() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isAccepting;
}

bool PriorityWorker::isRunning() const noexcept
{
    std::lock_guard<std::mutex> lock{my_mutex};
    return my_isRunning;
}
