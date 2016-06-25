#include <bureaucracy/serialworker.hpp>

using bureaucracy::SerialWorker;

SerialWorker::SerialWorker(Worker &worker)
  : my_worker{worker} { }

SerialWorker::~SerialWorker() noexcept
{
    stop();
}

void SerialWorker::add(Work work)
{
    my_worker.add([w = std::move(work), this](auto &work) {
        work.emplace_back(std::move(w));
        if(work.size() == 1)
        {
            my_worker.addDirect([this]() {
                my_worker.executeAll();
            });
        }
    });
}

void SerialWorker::stop()
{
    my_worker.stop();
}

bool SerialWorker::isAccepting() const noexcept
{
    return my_worker.isAccepting();
}

bool SerialWorker::isRunning() const noexcept
{
    return my_worker.isRunning();
}
