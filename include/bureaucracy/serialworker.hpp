#ifndef BUREAUCRACY_SERIALWORKER_HPP
#define BUREAUCRACY_SERIALWORKER_HPP 1

#include <bureaucracy/worker.hpp>
#include <bureaucracy/workercommon.hpp>

namespace bureaucracy
{
    /** \brief A Worker that ensures a piece of Work completes before the next
     *         Work is executed.
     *
     * A SerialWorker executes all its work in order using a single thread.
     * This ensures Work will not be executed until _all previous_ Work in the
     * SerialWorker completes.
     *
     * \warning It's very easy for a SerialWorker to cause starvation if it
     *          has a large amount of Work queued or more Work is added.
     */
    class SerialWorker : public Worker
    {
    public:
        /** \brief Construct a SerialWorker
         *
         * \param [in] worker
         *      the Worker to feed Work to
         */
        explicit SerialWorker(Worker & worker);

        void add(Work work) override;

        void stop() override;

        bool isAccepting() const noexcept override;

        bool isRunning() const noexcept override;

        /// \cond false
        ~SerialWorker() noexcept override;
        /// \endcond

    private:
        WorkerCommon<Work> my_worker;
    };
}

#endif
