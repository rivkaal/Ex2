
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
#include <cmath>
#include <string>

//-----------------DEFINES-----------------//
#define LIBRARY_ERROR "thread library error: "
#define SYSTEM_ERROR "system error: "
#define FAILURE -1
#define SUCCESS 0
#define SECOND 1000000
#define MAIN_THREAD 0


int sizeOfQuantomArray;
int *quantumArray;
int totalQuantums = 0;
std::deque <Thread*> ReadyQueue;
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
        std::cerr << SYSTEM_ERROR << " calling sigprocmask failed" << std::endl;
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
        std::cerr << SYSTEM_ERROR<<"calling sigprocmask failed" << std::endl;
        exit(1);
    }
}


void setTimer()
{
    int sec =  quantumArray[runningThread->getPriority()] / SECOND;
    int msec =  quantumArray[runningThread->getPriority()] % SECOND;
    timer.it_value.tv_sec = sec;		// first time interval, seconds part
    timer.it_value.tv_usec = msec;	// first time interval, microseconds part

    // same after that.
    timer.it_interval.tv_sec = sec;	// following time intervals, seconds part
    timer.it_interval.tv_usec = msec;	// following time intervals, microseconds part

    if(setitimer (ITIMER_VIRTUAL, &timer, nullptr))
    {
        std::cerr << SYSTEM_ERROR << std::endl;
        exit(1);
    }

}


void setThreadToRun()
{

    totalQuantums++;
    int val = sigsetjmp(runningThread -> getEnv(), 1);
    if (val != 0)
    {
        return;
    }

    runningThread = ReadyQueue.front();
    runningThread->setState(RUNNING);
    runningThread->raisinCountQuantom();
    ReadyQueue.pop_front();
    setTimer();
    siglongjmp(runningThread->getEnv(), 1);

}


/**
 * function that will work each time thread will run out of time.
 */
void timer_handler(int sig)
{
    blockSignals();
    if(ReadyQueue.empty())
    {
        runningThread->raisinCountQuantom();
        totalQuantums++;
    }
    else
    {
        runningThread->setState(READY);
        ReadyQueue.push_back(runningThread);
        setThreadToRun();
    }
    unblockSignals();
}


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * an array of the length of a quantum in micro-seconds for each priority.
 * It is an error to call this function with an array containing non-positive integer.
 * size - is the size of the array.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int *quantum_usecs, int size)
{
    blockSignals();
    if (size <= 0)
    {
        std::cerr << LIBRARY_ERROR << " Invalid size"<< std::endl;
        unblockSignals();
        return FAILURE;
    }

    quantumArray = new int[size];
    for(auto i = 0; i < size; ++i)
    {
        if(quantum_usecs[i] <= 0)
        {
            std::cerr << LIBRARY_ERROR<<" Invalid value in array" << std::endl;
            free(quantumArray);
            unblockSignals();
            return FAILURE;
        }
        quantumArray[i] = quantum_usecs[i];
    }
    sizeOfQuantomArray = size;

    for (int i = 1 ; i < MAX_THREAD_NUM ; ++i)
    {
        availibleIDs.push(i);
    }
    totalQuantums ++;

    try
    {
        runningThread = new Thread(0, 0, nullptr);
    }
    catch (std::exception &e)
    {
        std::cerr << LIBRARY_ERROR << "could not spawn thread" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    runningThread->setState(RUNNING); //TODO i've added this
    runningThread->raisinCountQuantom();
    threads[0] = runningThread;

    if (sigemptyset(&set))
    {
        std::cerr << SYSTEM_ERROR << "failed to call sigemptyset" << std::endl;
        exit(1);
    }

//    if (sigemptyset(&sa.sa_mask)) //TODO check do if we need this? no effect on tests.
//    {
//        std::cerr << SYSTEM_ERROR << "failed to call sigemptyset" << std::endl;
//        exit(1);
//    }

    sa.sa_flags = 0;
    setTimer();
    sa.sa_handler = &timer_handler;

    if (sigaddset(&set, SIGVTALRM))
    {
        std::cerr << SYSTEM_ERROR << "failed to call sig add set" << std::endl;
        exit(1);
    }

    if (sigaction(SIGVTALRM, &sa, nullptr))
    {
        std::cerr << SYSTEM_ERROR<< " sigaction failure" << std::endl;
        exit(1);
    }
    unblockSignals();

    return SUCCESS;
};


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

    if (threads.size() >= MAX_THREAD_NUM) //there couldn't be numbers bigger then max thread num
    {
        std::cerr << LIBRARY_ERROR << " could not spawn more threads" << std::endl;
        unblockSignals();
        return FAILURE;
    }
    if (priority >= sizeOfQuantomArray || priority < 0) //TODO CHECK THIS
    {
        std::cerr << LIBRARY_ERROR << " priority is invalid" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    int tidThread = availibleIDs.top();
    Thread *newThread;

    try
    {
        newThread = new Thread(tidThread, priority, f);
    }

    catch(std::bad_alloc &e)
    {
        std::cerr << SYSTEM_ERROR <<"bad alloc" <<std::endl;
        unblockSignals();
        exit(1);
    }

    availibleIDs.pop();

    ReadyQueue.push_back(newThread);
    threads[tidThread] = newThread;
    unblockSignals();
    return tidThread;
};


bool check_if_legal(int tid, const std::string& msg, const std::string& anotherMsg)
{
    if (tid < 0 || tid >= MAX_THREAD_NUM)
    {
        std::cerr << LIBRARY_ERROR << msg << std::endl;
        unblockSignals();
        return FAILURE;
    }
    if (threads.find(tid) == threads.end())
    {
        std::cerr << LIBRARY_ERROR <<anotherMsg << std::endl;
        unblockSignals();
        return FAILURE;
    }
    return SUCCESS;
}

/*
 * Description: This function changes the priority of the thread with ID tid.
 * If this is the current running thread, the effect should take place only the
 * next time the thread gets scheduled.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_change_priority(int tid, int priority)
{
    blockSignals(); //TODO CHECK IF NEED TO ADD THIS, had no effect on tests
    if (check_if_legal(tid, "illegal tid", "non existing thread"))
    {
        return FAILURE;
    }
    Thread *threadToChange = threads.find(tid)->second;
    if (threadToChange == nullptr)
    {
        std::cerr << LIBRARY_ERROR << " cant create thread" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    threadToChange->setPriority(priority);
    unblockSignals();
    return SUCCESS;
}


void remove_thread_from_ready_queue(int tid)
{
    std::deque<Thread*>::iterator th;
    for(th = ReadyQueue.begin(); th != ReadyQueue.end(); ++th)
    {
        if((*th)->getId() == tid)
        {
            ReadyQueue.erase(th);
            return;
        }
    }
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
    bool jump = false;

    if (check_if_legal(tid, "can't block thread with illegal tid", "can't block non existed thread"))
    {
        unblockSignals();
        return FAILURE;
    }

    Thread *threadToBlock = threads.find(tid) -> second;
    if (threadToBlock == nullptr || tid == 0)
    {
        std::cerr << LIBRARY_ERROR << "can't block this thread" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    if (threadToBlock -> getState() == BLOCKED)
    {
        unblockSignals();
        return SUCCESS;
    }
    if (threadToBlock -> getState() == RUNNING)
    {
        jump = true;
    }

    else //ready state
    {
        remove_thread_from_ready_queue(tid);
    }
    threadToBlock -> setState(BLOCKED);
    if(jump)
    {
        setThreadToRun();
    }

    unblockSignals();
    return SUCCESS;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    blockSignals();

    if (check_if_legal(tid,  "can't resume thread with illegal tid", "can't resume non existed thread"))
    {
        return FAILURE;
    }

    Thread *threadToResume = threads.find(tid) -> second;
    if (threadToResume == nullptr)
    {
        std::cerr << LIBRARY_ERROR <<"can't resume non existed thread" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    if (threadToResume -> getState() != BLOCKED)
    {
        unblockSignals();
        return SUCCESS;
    }

    threadToResume -> setState(READY);
    ReadyQueue.push_back(threadToResume);
    unblockSignals();
    return SUCCESS;
}


void terminate_main_thread()
{
    for (auto & thread:threads )
    {
        delete(thread.second);
        thread.second = nullptr;
    }

    ReadyQueue.clear();
    threads.clear();

    while(!availibleIDs.empty())
    {
        availibleIDs.pop();
    }

    unblockSignals();
    exit(EXIT_SUCCESS);
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
    blockSignals();

    if (check_if_legal(tid, "invalid id number", "can't terminate non existed thread" ))
    {
        unblockSignals();
        return FAILURE;
    }

    if (tid == MAIN_THREAD)
    {
        terminate_main_thread();
    }


    Thread *toTerminate = threads.find(tid)->second;
    if(toTerminate == nullptr) // todo i think this is redundant ?
    {
        std::cerr << LIBRARY_ERROR << " there is no thread with this id" << std::endl;
        std::cout << "shouldn't happened" << std::endl;
        unblockSignals();
        return FAILURE;
    }

    availibleIDs.push(tid);
    threads.erase(tid);

    if (toTerminate ->getState() == READY)
    {
        remove_thread_from_ready_queue(tid);
    }
    else if (toTerminate -> getState() == RUNNING)
    {
        if(ReadyQueue.empty())
        {
            std::cout << "shouldn't be" << std::endl;
        }
        setThreadToRun();
    }

    delete(toTerminate);
    toTerminate = nullptr;

    unblockSignals();
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
    blockSignals();
    if (threads.find(tid) != threads.end())
    {
        int quantums = threads.find(tid)->second -> getCountQuantom();
        unblockSignals();
        return quantums;
    }
    std::cerr << LIBRARY_ERROR << "invalid id" <<std::endl;
    unblockSignals();
    return FAILURE;
}

