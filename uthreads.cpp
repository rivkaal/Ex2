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
#define SUCCESS 0


int sizeOfQuantomArray;
int *quantumArray;
int totalQuantums;
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
    totalQuantums ++;
    if(ReadyQueue.empty())
    {
        runningThread->raisinCountQuantom();
    } else{
       runningThread->setState(READY);
       ReadyQueue.push_back(runningThread);
    }
}


void robin_round_algo(bool amIBlocked)
{
    blockSignals();


//    if (runningThread->getState() == TERMINATED)
//    {
//        //todo delete stack and free etc.
//        uthread_terminate(runningThread->getId());
//    }


    int val = sigsetjmp(runningThread -> getEnv(), 1);
    if (val == 0) // switch threads
    {
//        if (runningThread -> getState() != BLOCKED) // if i'm not blocked - go to end of ready list
//        {
//            runningThread -> setState(READY);
//            ReadyQueue.push_back(runningThread);
//        } // handeled by time handler
        runningThread = ReadyQueue.front();
        runningThread -> setState(RUNNING);
        ReadyQueue.pop_front();
        siglongjmp(runningThread ->getEnv(), 1);
    }

    unblockSignals();
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


//void sig_func()
//{
//
//}

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
//    if(threadToChange->getState() == RUNNING)
//    {
//        // not sure if we are using priority while running
//    }
    threadToChange->setPriority(priority);
    return 0;
}

/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    blockSignals();

    if (tid <= 0 || tid > MAX_THREAD_NUM)
    {
        std::cerr << LIBRARY_ERROR "can't block thread with illegal tid" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    Thread *threadToBlock = threads.find(tid) -> second;
    if (threadToBlock == nullptr)
    {
        return FAILURE;
    }

    if (threadToBlock -> getState() == BLOCKED)
    {
        unblockSignals();
        return SUCCESS;
    }

    if (threadToBlock -> getState() == RUNNING) // todo check if could be ready also
    {
        if (sigsetjmp(threadToBlock -> getEnv(), 1))
        {
            unblockSignals();
            //todo err
        }

        threadToBlock = runningThread;
        runningThread -> setState(BLOCKED);
        my_handler(0);
//        }
        unblockSignals();
        return SUCCESS;
    }
}

/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    Thread *toTerminate = threads[tid];
    if(toTerminate == nullptr)
    {
        return FAILURE;
    }
    int id = toTerminate->getId();
    availibleIDs.push(id);
    threads.erase(id);
    delete(toTerminate);
    return  SUCCESS;
}

/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    return runningThread->getId();
}


/*
 * Description: This function returns the total number of quantums since
 * the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    return totalQuantums;
//    int total = 0;
//    for (auto const& x : threads)
//    {
//        total += x.second->getCountQuantom(); // not sure... maybe we need just a variable with all quantums?
//    }
//    return total;
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered an error.
 * Return value: On success, return the number of quantums of the thread with ID tid.
 * 			     On failure, return -1.
*/
int uthread_get_quantums(int tid)
{
    return threads[tid] -> getCountQuantom();
}

int main()
{
    return  0;
}
