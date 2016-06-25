#include <bureaucracy/priorityworker.hpp>

#include <algorithm>

using bureaucracy::PriorityWorker;

PriorityWorker::PriorityWorker(Worker   &worker,
                               Priority  defaultPriority)
  : my_worker{worker},
    my_defaultPriority{defaultPriority} { }

/// \cond false
PriorityWorker::~PriorityWorker() noexcept
{
    stop();
}
/// \endcond

void PriorityWorker::add(Work     work,
                         Priority priority)
{
    my_worker.add([w = std::move(work), priority](auto &workQueue) {
        auto it = std::find_if(std::begin(workQueue), std::end(workQueue), [priority](auto const &p) {
            return priority < p.priority;
        });
        workQueue.emplace(it, PriorityWork{priority, std::move(w)});
    });
}

void PriorityWorker::stop()
{
    my_worker.stop();
}

bool PriorityWorker::isAccepting() const noexcept
{
    return my_worker.isAccepting();
}

bool PriorityWorker::isRunning() const noexcept
{
    return my_worker.isRunning();
}
