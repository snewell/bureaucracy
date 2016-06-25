#include <gtest/gtest.h>

#include <future>

#include <bureaucracy/serialworker.hpp>
#include <bureaucracy/threadpool.hpp>

using bureaucracy::SerialWorker;
using bureaucracy::Threadpool;

TEST(SerialWorker, test_ctor)
{
    Threadpool tp{4};
    SerialWorker sw{tp};

    ASSERT_EQ(true, sw.isAccepting());
    ASSERT_EQ(true, sw.isRunning());
}

TEST(SerialWorker, test_stop)
{
    Threadpool tp{4};
    SerialWorker sw{tp};

    sw.stop();
    ASSERT_EQ(false, sw.isAccepting());
    ASSERT_EQ(false, sw.isRunning());
}

namespace
{
    auto buildExpected(int &val, int expected)
    {
        return [&val, expected]() {
            ASSERT_EQ(expected, val);
            ++val;
        };
    }
}

TEST(SerialWorker, test_workOrder)
{
    Threadpool tp{4};
    SerialWorker sw{tp};

    auto val = 0;

    sw.add(buildExpected(val, 0));
    sw.add(buildExpected(val, 1));
    sw.add(buildExpected(val, 2));
    sw.add(buildExpected(val, 3));

    std::promise<void> hit;
    sw.add([&val, &hit] () {
        ASSERT_EQ(4, val);
        hit.set_value();
    });

    hit.get_future().get();
}

TEST(SerialWorker, test_sequencing)
{
    Threadpool tp{4};
    SerialWorker sw{tp};

    std::promise<void> seqHit;
    std::promise<void> normHit;

    sw.add([&normHit] () {
        normHit.get_future().get();
    });
    sw.add([&seqHit] () {
        seqHit.set_value();
    });

    // sleep for a bit to give Work a chance to run
    auto future = seqHit.get_future();
    auto result = future.wait_for(std::chrono::milliseconds(200));
    ASSERT_EQ(std::future_status::timeout, result);

    tp.add([&normHit]() {
        normHit.set_value();
    });

    future.get();
}

TEST(NegativeSerialWorker, test_addStopped)
{
    Threadpool tp{4};
    SerialWorker sw{tp};

    sw.stop();
    ASSERT_THROW(sw.add([]() { }), std::runtime_error);
}
