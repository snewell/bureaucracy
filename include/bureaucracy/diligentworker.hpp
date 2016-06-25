#ifndef WORKER_DILIGENTWORKER_HPP
#define WORKER_DILIGENTWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    class DiligentWorker : public Worker
    {
    public:
        using Alert = std::function<void () noexcept>;

        DiligentWorker(Worker &worker,
                        Alert   alert);

        ~DiligentWorker() noexcept;

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

    private:
        WorkerCommon<int> my_worker;

        Alert my_alert;
    };
}

#endif
