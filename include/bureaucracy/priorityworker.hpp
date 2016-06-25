#ifndef BUREAUCRACY_PRIORITYWORKER_HPP
#define BUREAUCRACY_PRIORITYWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    /** \brief A Worker that executes work based on the priority of the Work
     *         items.
     *
     * A PriorityWorker assigns a Priority to each piece of Work added and
     * ensures tasks with higher Priority are executed before low-Priority
     * tasks.  It does not guarantee high-Priority tasks complete first but
     * the Worker it leverages may offer additional guarantees about the order
     * of completion.
     *
     * \note PriorityWorker requires extra overhead (one function call) for
     *       each piece of Work executed.
     */
    class PriorityWorker : public Worker
    {
    public:
        /** \brief The priority of Work.
         */
        using Priority = unsigned int;

        /** \brief Construct a PriorityWorker.
         *
         * Work will be fed to \p worker for execution.  If the
         * single-argument version of add is used (e.g., the one provided as
         * part of Worker) the Work will be treated with priority \p
         * defaultPriority.
         *
         * \param [in] worker
         *      a Worker that will process Work items
         *
         * \param [in] defaultPriority
         *      the Priority to use if one is not specified in add
         */
        PriorityWorker(Worker   &worker,
                       Priority  defaultPriority = 0);

        void add(Work work) override;

        /** \brief Add Work with a Priority
         *
         * Add a piece of Work with a given Priority.  When scheduling tasks
         * smaller Priority values are considered more important than larger
         * Priority values (e.g., Priority 0 Work will execute before Priority
         * 1 Work).
         *
         * \param [in] work
         *      the Work to perform
         *
         * \param [in] priority
         *      the Priority of \p work
         */
        void add(Work     work,
                 Priority priority);

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        /// \cond false
        ~PriorityWorker() noexcept;
        /// \endcond

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
