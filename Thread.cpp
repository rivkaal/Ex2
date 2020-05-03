//
// Created by Tamar on 28/04/2020.
//
#include "Thread.h"


Thread::Thread(int idThread ,int priority, void (*f)(void))
{
    this->_idThread = idThread;
    this -> _priority = priority;
    this -> _state = READY;
    this -> _countQuantom = 0;
}

int Thread::getId()
{
    return _idThread;
}

State Thread::getState() const
{
    return _state;
}

 void Thread::setState(State state)
{
    this -> _state =  state;
}

void Thread::setPriority(int prioriry)
{
    this -> _priority = prioriry;
}

int Thread::getCountQuantom()
{
    return _countQuantom;
}

void Thread::raisinCountQuantom()
{
    _countQuantom++;
}


void setFFunc()
{

}








