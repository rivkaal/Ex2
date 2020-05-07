#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H
//-----------INCLUDES----------//
#include <stdio.h>
#include <csignal>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

//------------DEFINES----------//
#define STACK_SIZE 4096
enum State{RUNNING, BLOCKED, READY}; // todo notice : i changed waiting to ready..why waiting?

class Thread
{
private:

    State _state;
    int _idThread;
    int _priority;
    int _countQuantom;
    char _stack[STACK_SIZE];
    sigjmp_buf _env;


public:

    Thread(int idThread, int priority, void (*f)(void));

    ~Thread();

    int getId();

    State getState() const;

    void setState(State state);

    void setPriority(int prioriry);

    int getPriority();

    int getCountQuantom();

    void raisinCountQuantom();

    sigjmp_buf &getEnv();


};

#endif //UNTITLED_THREAD_H