#ifndef BUREAUCRACY_PRIORITYWORKER_HPP
#define BUREAUCRACY_PRIORITYWORKER_HPP 1

#include <condition_variable>
#include <mutex>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    class PriorityWorker : public Worker
    {
    public:
        using Priority = unsigned int;

        PriorityWorker(Worker   &worker,
                       Priority  defaultPriority = 0);

        virtual ~PriorityWorker() noexcept;

        void add(Work work) override;

        void add(Work     work,
                 Priority priority);

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

    private:
        Worker * const my_worker;

        std::condition_variable my_isEmpty;
        mutable std::mutex my_mutex;

        std::vector<std::pair<Priority, Work>> my_work;

        Priority const my_defaultPriority;

        bool my_isAccepting;
        bool my_isRunning;
        bool my_workQueued;
    };

    inline void PriorityWorker::add(Work work)
    {
        add(std::move(work), my_defaultPriority);
    }
}

#endif
