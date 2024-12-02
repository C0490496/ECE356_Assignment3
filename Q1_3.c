#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_AIRPLANES 5
#define NUM_RUNWAYS 3
#define MAX_QUEUE 5

//Semaphores and Mutex Resources.
sem_t runways;		//Semaphores for  the 3 runways.
sem_t queue;		//Semaphore for the airplane queue.
pthread_mutex_t control_tower;	//Mutext for the single control tower.
int airplane_progress[NUM_AIRPLANES] = {0}; //This tracks the airplanes progress.
int running_threads = NUM_AIRPLANES;	    //This tracks the active threads. 
pthread_mutex_t progress_lock = PTHREAD_MUTEX_INITIALIZER;
int airplanes_in_system = 0;		//Track the ariplanes in the system.
pthread_mutex_t monitor_lock; 	//mutex for monitoring shared variables

void perform_operation(const char *operation, int id){
	printf("Airplane %d: %s...\n", id, operation);
	sleep(rand() % 2 + 1);
}

//Starvation Prevention Airplane Thread.
void *airplane_starvation_free(void* arg){
	int id = *(int *)arg;
	while(1){
		printf("Airplane %d: Joining the wait queue. \n", id);
		sem_wait(&queue);
		printf("Airplane %d: In queue. \n", id);

		//Register the airplane to the system.
		pthread_mutex_lock(&monitor_lock);
		airplanes_in_system++;
		pthread_mutex_unlock(&monitor_lock);

		//Request clearance from the control tower.
		printf("Airplane %d: Am I clear to enter Control Tower?\n", id);
		pthread_mutex_lock(&control_tower);	//Locking the control tower using mutex.
		printf("Control Tower: Airplane %d proceed.\n", id);

		printf("Airplane %d: Requesting runway. \n", id);
		sem_wait(&runways);
		printf("Airplane %d: Acquired runway.\n", id);

		//Simulate Airplane action to runway.
		printf("Airplane %d: Copy now landing/taking off, \n", id);
		sleep(1);

		pthread_mutex_lock(&monitor_lock);
		airplanes_in_system--;
		pthread_mutex_unlock(&monitor_lock);

		//Free up or release the resources.
		sem_post(&runways);
		printf("Airplane %d: Released runway. \n", id);
		pthread_mutex_unlock(&control_tower);
		printf("Airplane %d: Released control tower. \n", id);
		sem_post(&queue);
		sleep(10);
	}

	pthread_mutex_lock(&progress_lock);
	running_threads--;
	pthread_mutex_unlock(&progress_lock);
}
//This detects if any deadlocks has occured
void *deadlock_det(void *arg){
	while(1){
	sleep(4);			//Monitor every 4 seconds.

	pthread_mutex_lock(&monitor_lock);
	if(airplanes_in_system > 0 && airplanes_in_system == NUM_AIRPLANES){
		printf("Deadlock Detected! All airplanes are waiting.\n");
		printf("Fixing problem.....\n");
		sem_post(&queue);		//Free a spot in the queue to get out of  deadlock.
		printf("Freed one spot from the queue.\n");
	}
	pthread_mutex_unlock(&monitor_lock);
}
}


int main(){
	pthread_t airplanes[NUM_AIRPLANES];
	int airplane_ids[NUM_AIRPLANES];
	pthread_t deadlock_det_thread;

	//Initialize semaphores and mutex.
	sem_init(&runways, 0, NUM_RUNWAYS);
	sem_init(&queue, 0 , MAX_QUEUE);
	pthread_mutex_init(&control_tower, NULL);
	pthread_mutex_init(&monitor_lock,NULL);

	//Create the airplane threads.
	for(int i=0; i<NUM_AIRPLANES; i++){
		airplane_ids[i] = i+1;
		pthread_create(&airplanes[i], NULL, airplane_starvation_free, &airplane_ids[i]);
	}

	pthread_create(&deadlock_det_thread, NULL, deadlock_det, NULL);

	//Join the airplane threads.
	for(int i = 0; i<NUM_AIRPLANES; i++){
		pthread_join(airplanes[i], NULL);
	}
	//Join the deadlock detection thread.
	pthread_join(deadlock_det_thread, NULL);

	//Clear the semaphores and mutex.
	sem_destroy(&runways);
	sem_destroy(&queue);
	pthread_mutex_destroy(&control_tower);
	pthread_mutex_destroy(&monitor_lock);
	return 0;
}


