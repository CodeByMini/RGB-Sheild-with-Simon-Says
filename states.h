#ifndef STATES_H
#define STATES_H
/*Only load the file once*/

/*This is a struct. with two ints. thats it.*/
typedef struct{
    int stateSwitcher;
    int stateExcecuter;
} STATE;

/*show the rest of the world that there are FUNCTIONS!!!*/
void HandleInterrupt1();
void HandleInterrupt2();
void SetState(STATE *, int);

#endif
