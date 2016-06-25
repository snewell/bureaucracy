#ifndef BUREAUCRACY_WORKERCOMMON_HPP
#define BUREAUCRACY_WORKERCOMMON_HPP 1

#include <condition_variable>
#include <mutex>
#include <vector>

#include <bureaucracy/worker.hpp>

namespace bureaucracy
{
    template <typename ...DATA>
    class WorkerCommon
    {
    public:
        WorkerCommon(Worker &worker);

        template <typename ADDFN>
        void add(ADDFN const &addFn);

        void addDirect(Worker::Work work);

        void stop();

        void notifyIfEmpty() noexcept;

        bool isAccepting() const noexcept;

        bool isRunning() const noexcept;

        bool isWorkQueued() const noexcept;

    private:
        Worker * const my_worker;

        std::condition_variable my_isEmpty;
        mutable std::mutex my_mutex;

        using WorkQueue = std::vector<DATA...>;
        WorkQueue my_work;

        bool my_isAccepting;
        bool my_isRunning;
        bool my_workQueued;
    };

    template <typename ...DATA>
    inline WorkerCommon<DATA...>::WorkerCommon(Worker &worker)
      : my_worker{&worker},
        my_isAccepting{true},
        my_isRunning{true},
        my_workQueued{false} { }

    template <typename ...DATA>
    template <typename ADDFN>
    inline void WorkerCommon<DATA...>::add(ADDFN const &addFn)
    {
        std::lock_guard<std::mutex> lock{my_mutex};

        if(my_isAccepting)
        {
            addFn(my_work);
            if(!my_workQueued)
            {
                my_worker->add([this]() {
                    std::unique_lock<std::mutex> lock{my_mutex};
                    while(!my_work.empty())
                    {
                        auto nextItem = my_work.front();
                        my_work.erase(std::begin(my_work));
                        lock.unlock();
                        nextItem();
                        lock.lock();
                    }
                    my_workQueued = false;
                    my_isEmpty.notify_one();
                });
                my_workQueued = true;
            }
        }
    }

    template <typename ...DATA>
    inline void WorkerCommon<DATA...>::addDirect(Worker::Work work)
    {
        my_worker->add(std::move(work));
    }

    template <typename ...DATA>
    inline void WorkerCommon<DATA...>::stop()
    {
        std::unique_lock<std::mutex> lock{my_mutex};

        if(my_isAccepting)
        {
            my_isAccepting = false;
            if(my_workQueued)
            {
                my_isEmpty.wait(lock);
            }
            my_isRunning = false;
        }
    }

    template <typename ...DATA>
    inline void WorkerCommon<DATA...>::notifyIfEmpty() noexcept
    {
        std::unique_lock<std::mutex> lock{my_mutex};
        if(!my_workQueued)
        {
            my_isEmpty.notify_all();
        }
    }

    template <typename ...DATA>
    inline bool WorkerCommon<DATA...>::isAccepting() const noexcept
    {
        std::lock_guard<std::mutex> lock{my_mutex};
        return my_isAccepting;
    }

    template <typename ...DATA>
    inline bool WorkerCommon<DATA...>::isRunning() const noexcept
    {
        std::lock_guard<std::mutex> lock{my_mutex};
        return my_isRunning;
    }

    template <typename ...DATA>
    inline bool WorkerCommon<DATA...>::isWorkQueued() const noexcept
    {
        std::lock_guard<std::mutex> lock{my_mutex};
        return my_workQueued;
    }
}

#endif
