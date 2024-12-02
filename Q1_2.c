#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_AIRPLANES 5
#define NUM_RUNWAYS 3

//Semaphores and Mutex Resources.
sem_t runways;		//Semaphores for  the 3 runways.
pthread_mutex_t control_tower;	//Mutext for the single control tower.
int airplane_progress[NUM_AIRPLANES] = {0}; //This tracks the airplanes progress.
int running_threads = NUM_AIRPLANES;	    //This tracks the active threads. 
pthread_mutex_t progress_lock = PTHREAD_MUTEX_INITIALIZER;

void perform_operation(const char *operation, int id){
	printf("Airplane %d: %s...\n", id, operation);
	sleep(rand() % 2 + 1);
}

void *airplane_deadlock_free(void* arg){
	int id = *(int *)arg;
	while(1){
		//Request clearance from the control tower.
		printf("Airplane %d: Am I clear to enter Control Tower?\n", id);
		pthread_mutex_lock(&control_tower);	//Locking the control tower using mutex.
		printf("Control Tower: Airplane %d proceed.\n", id);

		printf("Airplane %d: Requesting runway. \n", id);
		sem_wait(&runways);
		printf("Airplane %d: Acquired runway.\n", id);

		pthread_mutex_lock(&progress_lock);
		airplane_progress[id - 1] = 1;  //Mark the progress.
		pthread_mutex_unlock(&progress_lock);

		//Simulate Airplane action to runway.
		printf("Airplane %d: Copy now landing/taking off, \n", id);
		sleep(1);

		pthread_mutex_lock(&progress_lock);
		airplane_progress[id - 1] = 0;	//Mark idle
		pthread_mutex_unlock(&progress_lock);

		//Free up or release the resources.
		sem_post(&runways);
		printf("Airplane %d: Released runway. \n", id);
		pthread_mutex_unlock(&control_tower);
		printf("Airplane %d: Released control tower. \n", id);
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
	int inactive_count = 0;
	pthread_mutex_lock(&progress_lock);
	for(int i = 0; i < NUM_AIRPLANES; i++){
		if(airplane_progress[i] == 0){
			inactive_count++;
		}
	}
	pthread_mutex_unlock(&progress_lock);

	if(inactive_count == NUM_AIRPLANES && running_threads > 0){
		printf("\nDEADLOCK! All airplanes are now waiting forever. \n");
		exit(1);
	}

}
	return NULL;
}


int main(){
	pthread_t airplanes[NUM_AIRPLANES];
	int airplane_ids[NUM_AIRPLANES];
	pthread_t deadlock_det_thread;

	//Initialize semaphores and mutex.
	sem_init(&runways, 0, NUM_RUNWAYS);
	pthread_mutex_init(&control_tower, NULL);

	pthread_create(&deadlock_det_thread, NULL, deadlock_det, NULL);

	//Create the airplane threads.
	for(int i=0; i<NUM_AIRPLANES; i++){
		airplane_ids[i] = i+1;
		pthread_create(&airplanes[i], NULL, airplane_deadlock_free, &airplane_ids[i]);
	}
	//Join the airplane threads.
	for(int i = 0; i<NUM_AIRPLANES; i++){
		pthread_join(airplanes[i], NULL);
	}
	//Join the deadlock detection thread.
	pthread_join(deadlock_det_thread, NULL);

	//Clear the semaphores and mutex.
	sem_destroy(&runways);
	pthread_mutex_destroy(&control_tower);

	return 0;
}


