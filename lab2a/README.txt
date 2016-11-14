UCLA Fall 2016 CS111 Project 2A: Races and Synchronization Dau-Po Yu 504451468

------------------------------------------------------------------------------

Introduction
In this assignment, we encounter with a range of synchronization problems. 
We deal with conflicting read-modify-write operations on single variables 
and double linked list. The purpose of this project is to make sure we 
recognize critical sections and address them with a variety of different 
mechanisms, and demonstrate the existence of race conditions, and efficacy of 
the subsequent solutions.

------------------------------------------------------------------------------

Q&A
QUESTION 2.1.1 - causing conflicts:
Why does it take many iterations before errors are seen?
Why does a significantly smaller number of iterations so seldom fail?

	The possibility of conflicts increases as the number of iterations and 
threads increases. Therefore, it is more likely to see errors when we input a 
large number to the --iterations option. On the contrary, with a smaller numbers
of iterations, the possibility of failing the operations would be lower.

QUESTION 2.1.2 - cost of yielding:
Why are the --yield runs so much slower?  Where is the additional time going?  
Is it possible to get valid per-operation timings if we are using the --yield 
option?  
If so, explain how.  If not, explain why not.

	The pthread_yield function triggers context switch, which forces the current
thread to relinquish the CPU. This option makes the program to save the state of
the running thread and load another to run. This procedure takes more time than
a non-yield uninterrupted execution. It is possible to get a valid timing, if we
can approximate the time of a context switch. However, it is not easy to get the 
exact timing, since the time of a context switch varies from function to function.

QUESTION 2.1.3 - measurement errors:
Why does the average cost per operation drop with increasing iterations?
If the cost per iteration is a function of the number of iterations, how do we know 
how many iterations to run (or what the “correct” cost is)?

	Creating the iterations requires a fixed time overhead, which is larger than the 
operations inside the iterations. Therefore, as the number of iterations increases,
the fixed time overhead can be distributed to each operation, resulting in an average
lower cost. If we want to get the correct cost, we can create the thread first and then
turn on the timer.

QUESTION 2.1.4 - costs of serialization:
Why do all of the options perform similarly for low numbers of threads?
Why do the three protected operations slow down as the number of threads rises?
Why are spin-locks so expensive for large numbers of threads?

	If there is a conflict, the locking mechanism needs to be invoked. However, with low
numbers of threads, the possibility of conflicts is lower, so the protecting mechanism 
does not have to engage in the execution, which makes the output look similar. If the
number of threads increases, the possibility of conflicts increases as well. Therefore, 
the locking mechanism is more likely to be invoked, and the program has to spend more 
time not able to do anything, since the operations are being protected. Spin-locks are 
expensive, since they keep the CPU spinning until it is unlocked by another thread. This
feature of spin-locks make the average cost per operation larger, and it becomes very
obvious when dealing with a large number of threads.

QUESTION 2.2.1 - scalability of Mutex
Compare the variation in time per protected operation vs the number of threads (for 
mutex-protected operations) in Part-1 and Part-2, commenting on similarities/differences 
and offering explanations for them.

	In both parts, there are multiple threads entering critical sections. However, in part 1, 
the operations only include add 1 and add -1, while in part 2, the operations varies from
growing a double linked list, searching through a linked list, and deleting the whole 
data structure. This difference makes part 2 have a longer critical section, Therefore,
when executing lab2_list, there will be more conflicts probability, more threads blocked 
time, and less parallelism time. 


QUESTION 2.2.2 - scalability of spin locks
Compare the variation in time per protected operation vs the number of threads for Mutex vs 
Spin locks, commenting on similarities/differences and offering explanations for them.

	In part 1, time per protected operation decreases as the number of threads increases for
Mutex, while spin lock is not this case. In part 2, time per protected operation of both
mutex and spin lock increases as the number of threads increases. This may because the locked
time in part 2 is greater than that of part 1 due to longer critical section. Therefore, when
we call a protect mechanism, the time required to block the thread is way larger than that in
part 1. This results in the increasing number of time per protected operation in part 2 for 
spin lock and mutex.