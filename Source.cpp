#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include "SaleItem.h"

#define MAX_SIZE 7
#define BUFF_SIZE 7

using namespace std;

//arrays to hold buffer, producers, and consumers
SalesRecord * buff;
pthread_t * producerThreads;
pthread_t * consumerThreads;

//Global variables so the functions need not to pass them
int Production_Count;
int Section_Size;
int numProducers=2;
int numConsumers=2;

//called in main
void ProduceConsume(int maxProdroducers, int numConsumers);

//location and Id data for producer threads
struct ProducerData{
	int ID;
	int low;
	int high;
};

//consumer data, empty
struct ConsumerData{
};

//pointers to the structs
ProducerData	* producer_data;
ConsumerData * consumer_data;

//called when we create a thread
void *ProducerFunction(void * producer_data);
void *ConsumerFunction(void * consumer_data);

//create mutexes and condition variables
pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_cond_t producer_condition;
pthread_cond_t consumer_condition;

int main(){



	ProduceConsume(numProducers, numConsumers);
	cout << Production_Count << endl;

	system("pause");
	return 0;
}

void ProduceConsume(int numProducers, int numConsumers){
	//initialize the mutexes and conditional variables
	pthread_mutex_init(&producer_mutex, NULL);
	pthread_mutex_init(&consumer_mutex, NULL);
	pthread_cond_init(&producer_condition, NULL);
	pthread_cond_init(&consumer_condition, NULL);

	//Begin tracking how many items are produced
	Production_Count = 0;
	//Dynamically allocated array
	buff = new SalesRecord[BUFF_SIZE];
	//Producer thread array
	producerThreads = new pthread_t[numProducers];
	//Consumer thread array
	consumerThreads = new pthread_t[numConsumers];
	//Producer data array
	producer_data = new ProducerData[numProducers];
	//Consumer data array, empty data
	consumer_data = new ConsumerData[numConsumers];

	//determine section of array assigned to each thread
	Section_Size = BUFF_SIZE / numProducers;

	//create consumers first so that they are ready to consume when the first producer signals
	for (int i = 0; i < numConsumers; i++){
		pthread_create(&consumerThreads[i], NULL, ConsumerFunction, (void*) &consumer_data[i]);
	}
	//create producers and set their IDs
	for (int i = 0; i < numProducers; i++){
		producer_data[i].ID = i;
		pthread_create(&producerThreads[i], NULL, ProducerFunction, (void*) &producer_data[i]);
	}

	//wait for threads to finish
	for (int i = 0; i < numConsumers; i++)
		pthread_join(consumerThreads[i], NULL);
	for (int i = 0; i < numProducers; i++)
		pthread_join(producerThreads[i], NULL);

	//print the buffer for debugging
	for(int i = 0; i < BUFF_SIZE; i++){
		buff[i].display();
		cout <<endl;
	}



	//destroy arrays
	delete[] producer_data;
	delete[] consumer_data;

	//destroy mutexes and condition variables
	pthread_mutex_destroy(&producer_mutex);
	pthread_mutex_destroy(&consumer_mutex);
	pthread_cond_destroy(&producer_condition);
	pthread_cond_destroy(&consumer_condition);
}

void *ProducerFunction(void * producer_data){
	srand(1020);

	//pointer to producer data
	ProducerData * pd = (ProducerData*) producer_data;

	//divide threads along the buffer
	pd->low = Section_Size * pd->ID;
	pd->high = pd->low + Section_Size - 1;
	if(pd->ID == numProducers - 1)
		pd->high = BUFF_SIZE - 1;
	
	for(int i = pd->low; i <= pd->high && Production_Count <= MAX_SIZE; ++i){
		pthread_mutex_lock(&producer_mutex); //lock the shared count
		Production_Count++;
		pthread_mutex_unlock(&producer_mutex); //unlock the shared count
		//if (Production_Count < MAX_SIZE)  //may use this condition if there is a race condition
		buff[i]= SalesRecord(pd->ID);
	}

	pthread_exit(NULL);
	return 0;
}


void *ConsumerFunction(void * consumer_data){
	//not done
	pthread_exit(NULL);
	return 0;
}