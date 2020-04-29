//-----------INCLUDES----------//
#include <stdio.h>
#include <setjmp.h>

//------------DEFINES----------//
#ifndef UNTITLED_THREAD_H
#define UNTITLED_THREAD_H

class Thread
{
private:

    State state;
    int idThread;
    int countQuantom;
    //TODO define stack size. how much size?

public:

    Thread(int idThread);

    //Todo do i need a destructor?
     // i think we'd need a destructor maybe to delete the stack of the thread when it finishes

    int getId(){};

    sigjmp_buf &getState() const;

    void setState(State state){};


    int getCountQuantom();;

    void raisinCountQuantom();;

};

#endif //UNTITLED_THREAD_H