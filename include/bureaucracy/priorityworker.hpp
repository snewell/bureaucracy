#ifndef BUREAUCRACY_PRIORITYWORKER_HPP
#define BUREAUCRACY_PRIORITYWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    class PriorityWorker : public Worker
    {
    public:
        using Priority = unsigned int;

        PriorityWorker(Worker   &worker,
                       Priority  defaultPriority = 0);

        ~PriorityWorker() noexcept;

        void add(Work work) override;

        void add(Work     work,
                 Priority priority);

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

    private:
        struct PriorityWork
        {
            Priority priority;
            Work work;

            void operator() ();
        };

        WorkerCommon<PriorityWork> my_worker;

        Priority const my_defaultPriority;
    };

    inline void PriorityWorker::add(Work work)
    {
        add(std::move(work), my_defaultPriority);
    }

    inline void PriorityWorker::PriorityWork::operator() ()
    {
        work();
    }
}

#endif
