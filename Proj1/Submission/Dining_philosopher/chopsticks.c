#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>

#include "philosopher.h"

typedef enum  {left, right} direction_t;
pthread_mutex_t chopstick_mutex[5];

int phil_to_chopstick(int phil_id, direction_t d){
  return (phil_id + d) % 5;      
}

/*
 * Performs necessary initialization of mutexes.
 */
void chopsticks_init(){
	int i;
	
	for(i = 0; i < 5; i++)
    pthread_mutex_init(&chopstick_mutex[i], NULL);
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){
	int i;
	
	for(i = 0; i < 5; i++)
    pthread_mutex_destroy(&chopstick_mutex[i]);
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void pickup_chopsticks(int phil_id){
	if(phil_to_chopstick(phil_id, left) < phil_to_chopstick(phil_id, right))
    {
      pthread_mutex_lock(&chopstick_mutex[phil_to_chopstick(phil_id, left)]);
      pickup_left_chopstick(phil_id);
      pthread_mutex_lock(&chopstick_mutex[phil_to_chopstick(phil_id, right)]);
      pickup_right_chopstick(phil_id);
    }
    else
    {
      pthread_mutex_lock(&chopstick_mutex[phil_to_chopstick(phil_id, right)]);
      pickup_right_chopstick(phil_id);
      pthread_mutex_lock(&chopstick_mutex[phil_to_chopstick(phil_id, left)]);   
      pickup_left_chopstick(phil_id);  
    }
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void putdown_chopsticks(int phil_id){
	putdown_left_chopstick(phil_id);
	pthread_mutex_unlock(&chopstick_mutex[phil_to_chopstick(phil_id, left)]);
  	putdown_right_chopstick(phil_id);
  	pthread_mutex_unlock(&chopstick_mutex[phil_to_chopstick(phil_id, right)]);
}
