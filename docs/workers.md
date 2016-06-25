# Workers
A Worker is a class that's capable of executing work.  Worker implementations
are divided into two general categories:
1. Workers that manage threads
2. Workers that do not

A typical application will create one or more Workers that manage threads then
use Workers that provide additional guarantees or functionality

## Thread-Managing Workers
### Threadpool
Threadpool is a simple threadpool class.  It creates `n` threads when
constructed and leaves scheduling to the whims of the operating system.  This
is an excelling choice in most applications as adding Work is relatively cheap
since thread creation occurs at one time.

### ExpandingThreadpool
ExpandingThreadpool starts with a single thread and creates additional
threads when its backlog of work exceeds a certain level (assuming the maximum
number of threads have not been spawned).  This class is useful if an
application's load can fluctuate significantly from run to run but does
require more overhead when work is added (checking the load per thread is fast
but not free).

## Workers That Don't Manage Threads
### SerialWorker
A SerialWorker executes all its Work in order using a single thread.  This is
useful if you have a scenario where Work needs to complete before another
piece can start.

### PriorityWorker
A PriorityWorker executes Work with higher priority before executing Work with
lower priority.  Work can be distributed across multiple threads.

### DiligentWorker
A DiligentWorker calls a function when the last piece of Work completes.  This
is useful if you have a scenario where you need to know when all work is
completed but no guarantees are made regarding how Work is scheduled.
