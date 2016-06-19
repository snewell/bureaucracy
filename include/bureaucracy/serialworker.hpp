#ifndef BUREAUCRACY_SERIALWORKER_HPP
#define BUREAUCRACY_SERIALWORKER_HPP 1

#include <condition_variable>
#include <mutex>
#include <vector>

#include <bureaucracy/worker.hpp>

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
        Worker * const my_worker;

        std::condition_variable my_isEmpty;
        mutable std::mutex my_mutex;

        std::vector<Work> my_work;

        bool my_isAccepting;
        bool my_isRunning;
        bool my_workQueued;
    };
}

#endif
