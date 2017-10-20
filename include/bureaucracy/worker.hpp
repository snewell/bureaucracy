#ifndef BUREAUCRACY_WORKER_HPP
#define BUREAUCRACY_WORKER_HPP 1

#include <functional>

namespace bureaucracy
{
    /** \brief a class to manage work
     */
    class Worker
    {
    public:
        /** \brief a piece of work to performed
         */
        using Work = std::function<void()>;

        /** \brief Queue Work for execution.
         *
         *  \param [in] work
         *      a function that will be called at a later time
         */
        virtual void add(Work work) = 0;

        /** \brief Stop accepting new work and wait for existing work to
         *         complete
         *
         * This function will prevent new work from being added to the Worker
         * and block until all queued work is completed.  This function will
         * do the following:
         * 1. Prevent new work from being added to the Worker
         * 2. Wait for existing work to complete
         * 3. Mark this Worker as stopped
         *
         * \warning stop should only be called from one thread at a time.  If
         *          called from multiple threads at the same time behavior is
         *          undefined.
         *
         * \note Calling stop on an already stopped Worker has no effect.
         */
        virtual void stop() = 0;

        /** \brief Determine if this Worker is accepting new work.
         */
        virtual bool isAccepting() const noexcept = 0;

        /** \brief Determine if this Worker is running.
         */
        virtual bool isRunning() const noexcept = 0;

        /// \cond false
        virtual ~Worker() noexcept = default;
        /// \endcond
    };
}

#endif
