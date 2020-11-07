/* p3helper.c
 * Nathan Azoulay
 * Program 3
 * CS570
 * John Carroll
 * SDSU
 * 11/3/2020
 *
 * This is the only file you may change. (In fact, the other files should
 * be symbolic links to:
 *   ~cs570/Three/p3main.c
 *   ~cs570/Three/p3.h
 *   ~cs570/Three/Makefile
 *   ~cs570/Three/CHK.h    )
 *
 */
#include "p3.h"

/* You may put any semaphore (or other global) declarations/definitions here: */
static int jc,sc; // jogger and shooter count
sem_t shoot,gym,jog; // 2 semaphores (jogger and shooter) and 1 mutex gym



/* General documentation for the following functions is in p3.h
 * Here you supply the code:
 */

// Method creates semaphores needed (jogger, shooter, and gym)
void initstudentstuff(void) {
    CHK(sem_init(&shoot,LOCAL,1)); // initialize shooter and jogger semaphore
    CHK(sem_init(&jog,LOCAL,1));  
    CHK(sem_init(&gym,LOCAL,1));  // initialize gym mutex that they will access
}

// prolog() gives access to the gym to either shooter or jogger by 
// waiting until there are no more until the other type can enter
void prolog(int kind) {
    if(kind == SHOOTER){             
        CHK(sem_wait(&shoot));          // give shooter access to sc
        sc = sc + 1;                    // now have one more shooter
        if(sc == 1)                     
            CHK(sem_wait(&gym));        // first shooter in gym
        CHK(sem_post(&shoot));          // shooter can no longer enter gym
       
    }
    else if(kind == JOGGER){
        CHK(sem_wait(&jog));            // give jogger access to jc
        jc = jc+1;                      // now have jogger coming in
        if(jc == 1)                     
            CHK(sem_wait(&gym));        // first jogger in gym
        CHK(sem_post(&jog));            // jogger can no longer enter gym
    }
    else{
        fprintf(stderr,"Unknown type, expecting SHOOTER or READER\n");
    }
}

// epilog() lets out either the shooters or joggers by 
// revoking their access to the gym
void epilog(int kind) {
    if(kind == SHOOTER){
            CHK(sem_wait(&shoot));      // get shooter access to sc
            sc = sc - 1;                // one less shooter out
            if(sc == 0)
                CHK(sem_post(&gym));    // last shooter in the gym
            CHK(sem_post(&shoot));      // shooter no longer can access gym
    }
    else if(kind == JOGGER){
            CHK(sem_wait(&jog));        // get jogger access to jc
            jc = jc - 1;                // one more jogger out the gym
            if(jc == 0)         
                CHK(sem_post(&gym));    // last jogger to leave gym
            CHK(sem_post(&jog));        // jogger no longer can access gym
    }
    else{
        fprintf(stderr,"Unknown type, expecting SHOOTER or READER\n");
    }
}  

