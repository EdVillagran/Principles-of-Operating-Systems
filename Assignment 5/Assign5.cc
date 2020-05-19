#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <vector>
using namespace std;
/**************************************************
 * CSCI 480 - Assignment 5 - Semester Spring 2020
 * Programmer: Edgar Villagran
 * Due Date: 04/06/2020
 * Section: 1
 * TA: Jingwan Li
 * Program Summary: Use POSIX thread, Semaphores 
 * and a mutex to display the producer/consumer 
 * problem.
 *
 ***********************************************/

//Variables given by Assignment sheet
#define P_NUMBER 7 //number of producers
#define C_NUMBER 5 //number of consumers
#define BUFFER_SIZE 35 //maximum size of the buffer
#define P_STEPS 5 //the number of iterations for each producer thread
#define C_STEPS 7 //the number of iterations for each consumer thread

//total counter for number of widgets
int totalWidgets = 0;

//counts current widget, starts at -1 while none
int widget = -1;

//pair vector used to hold Producer #, Widget #
vector<pair<int, int>> Producers_And_Widgets;

sem_t notFull; //not full semaphore
sem_t notEmpty; //not emtpy semaphore

pthread_mutex_t mutex; //mutex 

/******Functions to create specified in Assignment sheet*********/

//Insert function works by having the producer insert items or widgets
//for consumer to remove as long as long as there is any available.
//It takes the thread id as a paramater
void Insert(int TID) {

	//Lock the Mutex
	int lockedMutex = pthread_mutex_lock(&mutex);	

	if (lockedMutex == 0) {
		//add 1 to the total now counter
		totalWidgets++;

		//Print out producer number, total items
		cerr << "\nProducer " << TID << " inserted one item.  Total is now " << totalWidgets << ". ";

		//If producer 0 then add to widget
		if (TID == 0)
			widget++;
		//insert producer number and widget to the pair vector
		Producers_And_Widgets.push_back(pair<int, int>(TID, widget));

		//if buffer not empty then display contents

		cerr << "Buffer now contains: ";
		for (unsigned i = 0; i<Producers_And_Widgets.size(); i++) {

		cout << "P" << Producers_And_Widgets[i].first << "W" << Producers_And_Widgets[i].second << " ";

		}
	}
	//if there was an error then display lock exit -1
	else {
		cerr << "Producer " << TID << " Error in lock" << endl;	//quit
		exit(-1);
	}
	//Unlock the mutex
	lockedMutex = pthread_mutex_unlock(&mutex);	

	//if there was an error then display lock exit -1
	if (lockedMutex != 0) {
		cerr << "Producer " << TID << " Error in Unlock" << endl;
		exit(-1);
	}
}

//Remove Function allows the consumer to take an item out of buffer
//as long as any available. Then It locks it and then removes
//the lock. 
//It takes the thread id as a paramater
void Remove(int TID) {

	//lock the mutex
	int lockedMutex = pthread_mutex_lock(&mutex);	

	if (lockedMutex == 0) {	//If there is a problem quit
		//remove one item from buffer
		totalWidgets--;

		//display one item removed, total.
		cerr << "\nConsumer " << TID << " removed one item.   Total is now " << totalWidgets << ". ";

		//remove item from the front of the vector.
		Producers_And_Widgets.erase(Producers_And_Widgets.begin());

		//if empty after then display buffer empty
		if (Producers_And_Widgets.size() == 0)
			cout << "Buffer is now empty.";

		//If not empty then display contents
		else {
			cout << "Buffer now contains: ";
			for (unsigned i = 0; i<Producers_And_Widgets.size(); i++) {
				//producer number is first, widget second in the vector
				cout << "P" << Producers_And_Widgets[i].first << "W" << Producers_And_Widgets[i].second << " ";
			}
		}

	}
	//If problem in lock exit -1
	else {
		cerr << "Consumer " << TID << ": Failure in Remove: lock" << endl;
		exit(-1);
	}
	//Unlock the mutex
	lockedMutex = pthread_mutex_unlock(&mutex);	//unlock the mutex

	//If problem in Unlock exit -1
	if (lockedMutex != 0) {
		cerr << "Consumer : " << TID << ": Failure in Remove: Unlock" << endl;
		exit(-1);
	}
}

//Function that runs each thread to
//insert into the widgets buffer.
void * Produce(void * TID) {

	//store TID as a long like simple example posted 
	long temp = (long)TID;	

	//loop for all the number of iterations for produce
	//needs to insert P_STEP times
	for (int i = 0; i < P_STEPS; i++) {
		sem_wait(&notFull);	//Semaphore waits on Notfull
		Insert(temp);//try and add a widged
		sem_post(&notEmpty);//Post on notEmpty
		sleep(1);//wait 1 sec
	}
	//kill thread after all iterations
	pthread_exit(NULL);
}

//Function that illustrated COnsumers removing from buffer.
void * Consume(void * TID) {
	long temp = (long)TID;	//conver to int

	//calls Remove  C_STEPS times
	for (int i = 0; i < C_STEPS; i++) {
		sem_wait(&notEmpty);//Semaphore waits on Notfull
		Remove(temp);	//try and remove 1 element
		sem_post(&notFull);//single out
		sleep(1);//sleep
	}
	pthread_exit(NULL);	//end thread
}
//Main able to take comand line arguments
int main(int argc, char *argv[])
{

 	//semphore for maximum amount
	sem_init(&notFull, 0, BUFFER_SIZE);
	//set up semphore for notEmpty
	sem_init(&notEmpty, 0, 0);

	// Correct Should be P_NUMBER * P_STEPS = C_NUMBER * C_STEPS.
	pthread_t Consumer[C_NUMBER*C_STEPS];	//array for consumer threads
	pthread_t Producer[P_NUMBER*P_STEPS];	//array for producer threads

	cout << "Simulation of Producer and Consumers" << endl;
	cout << "The semaphores and mutex have been initialized." << endl;

    pthread_mutex_init(&mutex, NULL);	//setup the mutex

	int returned; //return code from the pthread created

  //loop and make all of the cousumer thread
	for (long i = 0; i < C_NUMBER; i++) {
		//create the threads
		returned = pthread_create(&Consumer[i], NULL, Consume, (void *)i);

		//if there is an error exit with value of -1
		if (returned) {
			cerr << "Error in the consumer create. " << endl;
			exit(-1);
		}
	}

    //for loop to make all the producer threads
	for (long i = 0; i < P_NUMBER; i++) {
		returned = pthread_create(&Producer[i], NULL, Produce, (void *)i);
		if (returned) {	//If there is an error exit
			cout << "Error in the producer create." << endl;
			exit(-1);
		}
	}
    //at the end join delete the threads
	for (int i = 0; i < C_NUMBER; i++) {
		pthread_join(Consumer[i], NULL);
	}

	for (int i = 0; i < P_NUMBER; i++) {	//delete the threads
		pthread_join(Producer[i], NULL);
	}

	cout << "\nAll the producer and consumer threads have been closed." << endl;
	cout << "The semaphores and mutex have been deleted." << endl;

	//Get rid and terminate the mutex
	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);

	//Get rid of the semaphores created
	sem_destroy(&notFull);	
	sem_destroy(&notEmpty);	

	return 0;
}
