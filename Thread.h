#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H
//-----------INCLUDES----------//
#include <stdio.h>
#include <setjmp.h>

//------------DEFINES----------//
#define STACK_SIZE 4096
enum State{ RUNNIMG, BLOCKED, WAITING};


class Thread
{
private:

    State state;
    int idThread;
    int priority;
    int countQuantom;

public:

    Thread(int idThread, int priority);

    ~Thread();

    int getId();

    sigjmp_buf &getState() const;

    void setState(State state);


    int getCountQuantom();

    void raisinCountQuantom();

};

#endif //UNTITLED_THREAD_H