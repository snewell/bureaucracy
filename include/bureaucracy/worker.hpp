#ifndef BUREAUCRACY_WORKER_HPP
#define BUREAUCRACY_WORKER_HPP 1

#include <functional>

namespace bureaucracy
{
    class Worker
    {
    public:
        virtual ~Worker() noexcept = default;

        using Work = std::function<void () noexcept>;

        virtual void add(Work work) = 0;

        virtual void stop() = 0;

        virtual bool isAccepting() const noexcept = 0;

        virtual bool isRunning() const noexcept = 0;
    };
}

#endif
