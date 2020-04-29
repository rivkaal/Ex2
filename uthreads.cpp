//-----------INCLUDES----------//
#include "uthreads.h"
#include "Thread.h"
#include <queue>

int sizeOfQuantomArray;
std::vector<int> queueThreads;
std::deque<Thread*> ReadyQueue;
std:: priority_queue<int, std::vector<int>, std::greater<int>> availibleIDs;
Thread *runningThread;

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
    if (size <= 0)
    {
        std:cerr << "thread library error: Invalid size of array\n" << std:endl;
        return -1;
    }
    for(auto i = 0; i < size; ++i)
    {
        if(quantum_usecs <= 0)
        {
            std:cerr << "thread library error: Invalid value in array\n" << std:endl;
            return -1;
        }
    }
    sizeOfQuantomArray = size;

    for (int i = 1 ; i <= MAX_THREAD_NUM ; i++)
    {
        availibleIDs.push(i);
    }
    runningThread = new Thread(0, 0);
    return 0;
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

};
