#ifndef WORKER_DILLIGENTWORKER_HPP
#define WORKER_DILLIGENTWORKER_HPP 1

#include <condition_variable>
#include <functional>
#include <mutex>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    class DilligentWorker : public Worker
    {
    public:
        using Alert = std::function<void () noexcept>;

        DilligentWorker(Worker &worker,
                        Alert   alert);

        ~DilligentWorker() noexcept;

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

    private:
        Worker * const my_worker;

        Alert my_alert;

        mutable std::mutex my_mutex;
        std::condition_variable my_isEmpty;
        unsigned int my_workQueued;

        bool my_isAccepting;
        bool my_isRunning;
    };
}

#endif
