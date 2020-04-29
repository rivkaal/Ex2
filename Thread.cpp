//
// Created by Tamar on 28/04/2020.
//
#include "Thread.h"


Thread::Thread(int idThread)
{
    this->idThread = idThread;
}

int Thread::getId()
{
    return idThread;
}

sigjmp_buf &Thread::getState();

sigjmp_buf &getState();

int Thread::getCountQuantom()
{
    return countQuantom;
}

void Thread::raisinCountQuantom()
{
    countQuantom++;
}






