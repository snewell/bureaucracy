#include <gtest/gtest.h>

#include <bureaucracy/serialworker.hpp>
#include <bureaucracy/threadpool.hpp>

TEST(SerialWorkerTest, test_ctor)
{
    bureaucracy::Threadpool tp{4};
    bureaucracy::SerialWorker sw{tp};

    ASSERT_EQ(true, sw.isAccepting());
    ASSERT_EQ(true, sw.isRunning());
}

TEST(SerialWorkerTest, test_stop)
{
    bureaucracy::Threadpool tp{4};
    bureaucracy::SerialWorker sw{tp};

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

TEST(SerialWorkerTest, test_workOrder)
{
    bureaucracy::Threadpool tp{4};
    bureaucracy::SerialWorker sw{tp};

    auto val = 0;

    sw.add(buildExpected(val, 0));
    sw.add(buildExpected(val, 1));
    sw.add(buildExpected(val, 2));
    sw.add(buildExpected(val, 3));
}
