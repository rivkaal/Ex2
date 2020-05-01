#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H
//-----------INCLUDES----------//
#include <stdio.h>
#include <setjmp.h>

//------------DEFINES----------//
#define STACK_SIZE 4096
enum State{RUNNING, BLOCKED, READY}; // todo notice : i changed waiting to ready..why waiting?

class Thread
{
private:

    State state;
    int idThread;
    int priority;
    int countQuantom;


public:

    Thread(int idThread, int priority, void (*f)(void));

    ~Thread();

    int getId();

    sigjmp_buf &getState() const;

    void setState(State state);

    int getCountQuantom();

    void raisinCountQuantom();


};

#endif //UNTITLED_THREAD_H