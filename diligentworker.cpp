#include <bureaucracy/diligentworker.hpp>

using bureaucracy::DiligentWorker;

DiligentWorker::DiligentWorker(Worker &worker,
                                Alert   alert)
  : my_worker{worker},
    my_alert{std::move(alert)} { }

DiligentWorker::~DiligentWorker() noexcept
{
    stop();
}

void DiligentWorker::add(Work work)
{
    my_worker.addDirect([w = std::move(work), this]() {
        w();
        if(!my_worker.isWorkQueued())
        {
            my_alert();
            my_worker.notifyIfEmpty();
        }
    });
}

void DiligentWorker::stop()
{
    my_worker.stop();
}

bool DiligentWorker::isAccepting() const noexcept
{
    return my_worker.isAccepting();
}

bool DiligentWorker::isRunning() const noexcept
{
    return my_worker.isRunning();
}
