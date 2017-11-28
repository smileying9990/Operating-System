/* p3helper.c
 * Yingxue Zhu
 * Program 3
 * CS570
 * John Carroll
 * SDSU
 * 2016/10/25
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
static sem_t gym; 
static sem_t jmutex; 
static sem_t smutex; 

int joggercount=0;
int shootercount=0;


/* General documentation for the following functions is in p3.h
 * Here you supply the costatic sem_t smutex; de:
 */
void initstudentstuff(void) {
sem_init(&gym,LOCAL,1);
sem_init(&jmutex,LOCAL,1);
sem_init(&smutex,LOCAL,1);
}

void prolog(int kind) {
	if(kind==JOGGER){
//jmutex is the semaphore protect joggercount
       sem_wait(&jmutex);	
/*if jogger comes before shooter, joggers have access to gym
shooters have to wait until the last jogger left*/	
		if(joggercount==0){sem_wait(&gym);}
				
		joggercount++;
		sem_post(&jmutex);		
	
	}
	if(kind==SHOOTER){
	//smutex is the semaphore protect shootercount
        sem_wait(&smutex);		
/*if shooter comes before jogger, shooters have access to gym
 joggers have to wait until the last shooter left*/ 
		if(shootercount==0){sem_wait(&gym);}	
		
		shootercount++;
		sem_post(&smutex);	
	}
}

void epilog(int kind) {
	
	if(kind==JOGGER){
		
	    sem_wait(&jmutex);
		joggercount--;
//the last jogger gives gym back		
		if(joggercount==0){sem_post(&gym);}
		sem_post(&jmutex);
				
	}
	if(kind==SHOOTER){
	
	    sem_wait(&smutex);
		shootercount--;
//the last shooter gives gym back	
		if(shootercount==0){sem_post(&gym);}
		sem_post(&smutex);
					
	}
	
}  

