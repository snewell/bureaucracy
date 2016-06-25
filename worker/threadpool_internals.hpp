#ifndef THREADPOOL_INTERNALS_HPP
#define THREADPOOL_INTERNALS_HPP 1

namespace bureaucracy
{
    namespace internal
    {
        inline void threadWorker(bool                                   &accepting,
                                 std::condition_variable                &workReady,
                                 std::mutex                             &mutex,
                                 std::vector<bureaucracy::Worker::Work> &work)
        {
            std::unique_lock<std::mutex> lock{mutex};

            while(accepting)
            {
                while(!work.empty())
                {
                    auto nextItem = work.front();
                    work.erase(std::begin(work));
                    lock.unlock();
                    nextItem();
                    lock.lock();
                }
                if(accepting)
                {
                    // we may have stopped while calling the work functions, check
                    // before waiting
                    workReady.wait(lock);
                }
            }
        }
    }
}

#endif
