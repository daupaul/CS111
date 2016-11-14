UCLA Fall 2016 CS111 Project 2B: Lock Granularity and Performance Dau-Po Yu 504451468

------------------------------------------------------------------------------

Introduction
In this assignment, we want to further understand the synchronization problems
of mutex and spin-lock. We do performance instrumentation and measurement and
implement a new options to partition a list into sublists. The purpose of this 
project is to make sure we recognize the problem of synchronization and solve it
with partitioned list.

------------------------------------------------------------------------------

Q&A
QUESTION 2.3.1 - Cycles in the basic implementation:
Where do you believe most of the cycles are spent in the 1 and 2-thread tests (for both add and list)?  Why do you believe these to be the most expensive parts of the code?
Where do you believe most of the time/cycles are being spent in the high-thread spin-lock tests?
Where do you believe most of the time/cycles are being spent in the high-thread mutex tests?

	In 1 thread test, there will be no contention, since there is only one thread, which means no 
synchronization problem. Therefore, it is most likely that the time are being spent in overhead. As
for 2 thread test, even if there is a synchronization problem, its behavior is similar to 1 thread
test. In both spin-lock and mutex high-thread tests, most of the time is spent either spinning or 
waiting to auquire the lock, since only one thread can enter the critical section at the same time.

QUESTION 2.3.2 - Execution Profiling:
Where (what lines of code) are consuming most of the cycles when the spin-lock version of the list exerciser is run with a large number of threads?
Why does this operation become so expensive with large numbers of threads?

	"while (__sync_lock_test_and_set(&spin, 1))" is consuming most of the cycles. When dealing with 
large number of threads, spin-locks become expensive. They keep the CPU spinning until it is 
unlocked by another thread, and the program can do nothing but keep checking the lock until it is
allowed to enter critical section.

QUESTION 2.3.3 - Mutex Wait Time:
Look at the average time per operation (vs # threads) and the average wait-for-mutex time (vs #threads).  
Why does the average lock-wait time rise so dramatically with the number of contending threads?
Why does the completion time per operation rise (less dramatically) with the
number of contending threads?
How is it possible for the wait time per operation to go up faster (or higher) than the completion time per operation?

	As the thread number increases, the wait time for lock increases as well, since only 1 thread can 
enter critical section at the same time, regardless of thread number. Creating the threads requires 
a fixed time overhead. Therefore, as the number of threads increases, the fixed time overhead can be 
distributed to each operation, causing the increase of cost per operations smaller.	Wait for lock time
is CPU time while completion time is wall time. Therefore, they do not contradict and have nothing to
do with each other.

QUESTION 2.3.4 - Performance of Partitioned Lists
Explain the change in performance of the synchronized methods as a function of the number of lists.
Should the throughput continue increasing as the number of lists is further increased?  If not, explain why not.
It seems reasonable to suggest the throughput of an N-way partitioned list should be equivalent to the throughput of a single list with fewer (1/N) threads.  Does this appear to be true in the above curves?  If not, explain why not.

	The throughput should continue increasing as the number of lists is further increased. As the list
number increase, the possibility of contention drops, since each thread can work on different sublists.
This makes the wait for lock time before entering the critical section decrease. It is true. We can think
of each thread work on a sublist, so it makes the throughput the same as one thread works on a list.