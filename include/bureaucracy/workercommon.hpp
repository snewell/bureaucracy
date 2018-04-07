#ifndef BUREAUCRACY_WORKERCOMMON_HPP
#define BUREAUCRACY_WORKERCOMMON_HPP 1

#include <condition_variable>
#include <mutex>
#include <vector>

#include <bureaucracy/worker.hpp>

#include <houseguest/synchronize.hpp>

namespace bureaucracy
{
    template <typename DATA>
    /** \internal
     *
     * WorkerCommon contains common functionality required by a number of
     * Worker implementations; there are no guarantees about functionality.
     *
     * \cond false
     */
    class WorkerCommon
    {
    public:
        explicit WorkerCommon(Worker & worker);

        template <typename ADDFN>
        void add(ADDFN const & addFn);

        void addDirect(Worker::Work work);

        void executeAll() noexcept;

        void stop();

        void notifyIfEmpty() noexcept;

        DATA getNextItem() noexcept;

        bool isAccepting() const noexcept;

        bool isRunning() const noexcept;

        bool isWorkQueued() const noexcept;

    private:
        Worker * const my_worker;

        std::condition_variable my_isEmpty;
        mutable std::mutex my_mutex;

        using WorkQueue = std::vector<DATA>;
        WorkQueue my_work;

        bool my_isAccepting;
        bool my_isRunning;
    };

    template <typename DATA>
    inline WorkerCommon<DATA>::WorkerCommon(Worker & worker)
      : my_worker{&worker}
      , my_isAccepting{true}
      , my_isRunning{true}
    {
    }

    template <typename DATA>
    template <typename ADDFN>
    inline void WorkerCommon<DATA>::add(ADDFN const & addFn)
    {
        houseguest::synchronize(my_mutex, [this, &addFn]() {
            if(my_isAccepting)
            {
                addFn(my_work);
            }
            else
            {
                throw std::runtime_error{"Not accepting work"};
            }
        });
    }

    template <typename DATA>
    inline void WorkerCommon<DATA>::addDirect(Worker::Work work)
    {
        // should be locked
        if(my_isAccepting)
        {
            my_worker->add(std::move(work));
        }
        else
        {
            throw std::runtime_error{"Not accepting work"};
        }
    }

    template <typename DATA>
    inline void WorkerCommon<DATA>::executeAll() noexcept
    {
        houseguest::synchronize_unique(my_mutex, [this](auto lock) {
            while(!my_work.empty())
            {
                auto nextItem = my_work.front();
                my_work.erase(std::begin(my_work));
                lock.unlock();
                nextItem();
                lock.lock();
            }
            my_isEmpty.notify_one();
        });
    }

    template <typename DATA>
    inline void WorkerCommon<DATA>::stop()
    {
        houseguest::synchronize_unique(my_mutex, [this](auto lock) {
            if(my_isAccepting)
            {
                my_isAccepting = false;
                if(!my_work.empty())
                {
                    my_isEmpty.wait(lock);
                }
                my_isRunning = false;
            }
        });
    }

    template <typename DATA>
    inline void WorkerCommon<DATA>::notifyIfEmpty() noexcept
    {
        houseguest::synchronize(my_mutex, [this]() {
            if(my_work.empty())
            {
                my_isEmpty.notify_all();
            }
        });
    }

    template <typename DATA>
    inline DATA WorkerCommon<DATA>::getNextItem() noexcept
    {
        return houseguest::synchronize(my_mutex, [this]() {
            auto ret = my_work.front();
            my_work.erase(std::begin(my_work));
            return ret;
        });
    }

    template <typename DATA>
    inline bool WorkerCommon<DATA>::isAccepting() const noexcept
    {
        return houseguest::synchronize(my_mutex, [this]() {
            return my_isAccepting;
        });
    }

    template <typename DATA>
    inline bool WorkerCommon<DATA>::isRunning() const noexcept
    {
        return houseguest::synchronize(my_mutex, [this]() {
            return my_isRunning;
        });
    }

    template <typename DATA>
    inline bool WorkerCommon<DATA>::isWorkQueued() const noexcept
    {
        return houseguest::synchronize(my_mutex, [this]() {
            return !(my_work.empty());
        });
    }
    /// \endcond
}

#endif
