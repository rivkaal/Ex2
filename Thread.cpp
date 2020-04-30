//
// Created by Tamar on 28/04/2020.
//
#include "Thread.h"


Thread::Thread(int idThread, int priority)
{
    this->idThread = idThread;
    this -> priority = priority;
    this -> state = WAITING;
}

int Thread::getId()
{
    return idThread;
}

sigjmp_buf &Thread::getState() const
{

}

sigjmp_buf &getState();

int Thread::getCountQuantom()
{
    return countQuantom;
}

void Thread::raisinCountQuantom()
{
    countQuantom++;
}






