//
// Created by Tamar on 28/04/2020.
//
#include "Thread.h"


Thread::Thread(int idThread ,int priority, void (*f)(void))
{
    this->idThread = idThread;
    this -> priority = priority;
//    this -> state = WAITING; //why? there's no waiting state. we only have blocked, ready and running. i dont think we need this.
}

int Thread::getId()
{
    return idThread;
}

sigjmp_buf &Thread::getState() const
{

}

 void Thread::setState(State state)
{
    this -> state =  state;
}

int Thread::getCountQuantom()
{
    return countQuantom;
}

void Thread::raisinCountQuantom()
{
    countQuantom++;
}


void setFFunc()
{

}








