#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include "SaleItem.h"

#define MAX_SIZE 20
#define BUFFER_SIZE 20

using namespace std;

SalesRecord * buff; 
pthread_t * producerThreads;
pthread_t * consumerThreads;

const int numProducers=4;
const int numConsumers=4;

//Shared variables
int Production_Count=0;                //keeps track of overall production
int Counter=0;                         //the number of "valid" items in the buffer
double AllStoreTotals[numProducers];   //array the size of number of producers to track store totals
double AllMonthTotals[12];             //array the size of number of months to track month totals
int WritePosition;                     //where the producer writes next
int ReadPosition;                      //where the consumer reads next

//function proto types
void ProduceConsume(int numProducers, int numConsumers);
void *ProducerFunction(void * producer_data);
void *ConsumerFunction(void * consumer_data);

struct ProducerData{
	int ID;
};

struct ConsumerData{
	int ID;
	double ConsumerStoreTotals[numProducers];//local statistics
	double ConsumerMonthTotals[12];//local statistics
};


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
	pthread_mutex_init(&producer_mutex, NULL);
	pthread_mutex_init(&consumer_mutex, NULL);
	pthread_cond_init(&producer_condition, NULL);
	pthread_cond_init(&consumer_condition, NULL);

	
	ProducerData	* producer_data;  //pointer to the ProducerData Struct
	ConsumerData	* consumer_data;  //pointer to the ConsumerData Struct

	producerThreads = new pthread_t[numProducers];  //array to hold the Producer Threads
	consumerThreads = new pthread_t[numConsumers];  //array to hold the Consumer Threads
	producer_data = new ProducerData[numProducers]; //array to hold the Producer Structs
	consumer_data = new ConsumerData[numConsumers]; //array to hold the Consumer Structs
	
	buff=new SalesRecord[BUFFER_SIZE];

	//create consumers first so that they are ready to consume when the first producer signals
	for (int i = 0; i < numConsumers; i++){		
		//creates the consumer thread "i" which uses consumer struct "i" as variable
		consumer_data[i].ID=i;
		pthread_create(&consumerThreads[i], NULL, ConsumerFunction, (void*) &consumer_data[i]);
	}

	//create producers and set their IDs
	for (int i = 0; i < numProducers; i++){
		//creates the producer thread "i" which uses producer struct "i" as variable
		producer_data[i].ID = i;
		pthread_create(&producerThreads[i], NULL, ProducerFunction, (void*) &producer_data[i]);
	}

	//wait for threads to finish
	for (int i = 0; i < numConsumers; i++)
		pthread_join(consumerThreads[i], NULL);
	for (int i = 0; i < numProducers; i++)
		pthread_join(producerThreads[i], NULL);

	//output of buffer - Debugging tool
	for (int i=0; i<BUFFER_SIZE; i++){
		cout<<"Index: "<<i<<"  ";
		buff[i].display();
	}

	//clean up
	delete[] producer_data;
	delete[] consumer_data;
	pthread_mutex_destroy(&producer_mutex);
	pthread_mutex_destroy(&consumer_mutex);
	pthread_cond_destroy(&producer_condition);
	pthread_cond_destroy(&consumer_condition);
}

void *ProducerFunction(void * producer_data){
	ProducerData * pd = (ProducerData*) producer_data; //make a pointer point to the correct producer_data struct

	//seed each thread by raising seed by the power of the thread/producer ID
	srand(time(NULL)^pd->ID);
	
	while(Production_Count<MAX_SIZE)
	{
		while(Counter==BUFFER_SIZE); //Do nothing (no room to write anything) - This can be changed to use mutex instead

		buff[WritePosition]=SalesRecord(pd->ID);
		WritePosition=(WritePosition+1)%BUFFER_SIZE;
				
		//pthread_mutex_lock(&producer_mutex); //lock the shared count
		Production_Count++;//This is the global production count
		Counter++;//This is the count of valid objects in buffer that cannot be overwritten
		//pthread_mutex_unlock(&producer_mutex); //unlock the shared count
	}

	pthread_exit(NULL);
	return 0;
}

void *ConsumerFunction(void * consumer_data){
	//not done
	pthread_exit(NULL);
	return 0;
}