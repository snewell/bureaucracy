#ifndef WORKER_DILIGENTWORKER_HPP
#define WORKER_DILIGENTWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    /** \brief A Worker that alerts when it's out of work.
     *
     * A DiligentWorker executes a callback when it's queue of work is
     * depleted.  It offers no guarantees about execution order or completion,
     * deferring those details to another Worker.
     *
     * \note DiligentWorker requires checking whether all Work is completed
     *       after each piece of Work completes.  This adds a slight
     *       perforamnce penalty to each piece of Work processed by a
     *       DiligentWorker.
     */
    class DiligentWorker : public Worker
    {
    public:
        /** \brief A function that's called when A DiligentWorker completes
         *         all it's work.
         */
        using Alert = std::function<void () noexcept>;

        /** \brief Construct a DiligentWorker
         *
         * DiligentWorker will feed all Work to \p worker.  Each time the last
         * piece of Work queued in the DiligentWorker is completed \p alert
         * will be called, from the same thread, _once._
         *
         * \param [in] worker
         *      a Worker to schedule and execute units of Work
         *
         * \param [in] alert
         *      the Alert to call when all work is completed
         */
        DiligentWorker(Worker &worker,
                        Alert   alert);

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        /// \cond false
        ~DiligentWorker() noexcept;
        /// \endcond

    private:
        WorkerCommon<int> my_worker;

        Alert my_alert;
    };
}

#endif
