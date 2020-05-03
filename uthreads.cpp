//-----------------INCLUDE-----------------//
#include "uthreads.h"
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "Thread.h"
#include <queue>
#include <iostream>
#include <deque>
#include <map>

//-----------------DEFINES-----------------//
#define LIBRARY_ERROR "thread library error: "
#define SYSTEM_ERROR "system error: "
#define FAILURE -1


int sizeOfQuantomArray;
int *quantumArray;
std::deque<Thread*> ReadyQueue;
std:: priority_queue<int, std::vector<int>, std::greater<int>> availibleIDs;
std::map <int, Thread*> threads;
Thread *runningThread;
struct sigaction sa;
struct itimerval timer;
sigset_t set;

//-----------------FUNCTIONS-----------------//

/**
 * blocks the signals
 */
void blockSignals()
{
    if (sigprocmask(SIG_BLOCK, &set, nullptr))
    {
        std::cerr << "system error: calling sigprocmask failed" << std::endl;
        exit(1);
    }
}


/**
 * unblocks signals
 */
void unblockSignals()
{
    if (sigprocmask(SIG_UNBLOCK, &set, nullptr))
    {
       std::cerr << "calling sigprocmask failed" << std::endl;
        exit(1);
    }
}


/**
 * function that will work each time thread will run out of time.
 */
void timer_handler()
{
    if(ReadyQueue.empty())
    {
        runningThread->raisinCountQuantom();
    } else{
       runningThread->setState(READY);
       ReadyQueue.push_back(runningThread);
    }
}


void round_robin()
{

}


void my_handler(int sig_num);


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * an array of the length of a quantum in micro-seconds for each priority.
 * It is an error to call this function with an array containing non-positive integer.
 * size - is the size of the array.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(const int *quantum_usecs, int size)
{
    if (size <= 0)
    {
        std::cerr << "thread library error: Invalid size of array\n" << std::endl;
        return -1;
    }

    quantumArray = new int[size];
    for(auto i = 0; i < size; ++i)
    {
        if(quantum_usecs[i] <= 0)
        {
            std::cerr << "thread library error: Invalid value in array\n" << std::endl;
            free(quantumArray);
            return -1;
        }
        quantumArray[i] = quantum_usecs[i];
    }
    sizeOfQuantomArray = size;

    for (int i = 1 ; i <= MAX_THREAD_NUM ; i++)
    {
        availibleIDs.push(i);
    }
    runningThread = new Thread(0, 0, nullptr);

    //TODO check if can call or return value isnt valid.
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &set, nullptr);

    sa = {0};
    sa.sa_handler = &my_handler;
    if (sigaction(SIGVTALRM, &sa, nullptr))
    {
        std::cerr << "to do err" << std::endl;
    }
    //TODO set timer

    return 0;
};

void sig_func()
{

}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * priority - The priority of the new thread.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void), int priority)
{
    blockSignals();
    int tidThread = availibleIDs.top();

    if (tidThread >= MAX_THREAD_NUM)
    {
        std::cerr << LIBRARY_ERROR "could not spawn more threads" << std::endl;
        unblockSignals();
        return FAILURE;
    }
    Thread *newThread = new Thread(tidThread, priority, f); // todo check if allocated stack for thread
    // todo check if need to do try and catch in case can't allocate space for thread etc..
    newThread->setState(State::READY); // todo check if this is necessary for later use
    availibleIDs.pop();
    ReadyQueue.push_back(newThread);
    threads[tidThread] = newThread;
    unblockSignals();
    return tidThread;
};

/*
 * Description: This function changes the priority of the thread with ID tid.
 * If this is the current running thread, the effect should take place only the
 * next time the thread gets scheduled.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_change_priority(int tid, int priority)
{
    Thread *threadToChange = threads[tid];
    if(threadToChange == nullptr)
    {
        return -1;
    }
    if(threadToChange->getState() == RUNNING)
    {
        // not sure if we are using priority while running
    }
    threadToChange->setPriority(priority);
    return 0;
}


int main()
{
    return  0;
}
