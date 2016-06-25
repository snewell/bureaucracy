#ifndef BUREAUCRACY_SERIALWORKER_HPP
#define BUREAUCRACY_SERIALWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    class SerialWorker : public Worker
    {
    public:
        SerialWorker(Worker &worker);

        ~SerialWorker() noexcept;

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

    private:
        WorkerCommon<Work> my_worker;
    };
}

#endif
