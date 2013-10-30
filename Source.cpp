#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include "SaleItem.h"

#define MAX_SIZE 200

using namespace std;

SalesRecord * buff;
pthread_t * producerThreads;
pthread_t * consumerThreads;

int Production_Count;
int Section_Size;
int maxProducers;
int maxConsumers;

void ProduceConsume(int maxProdroducers, int maxConsumers);


struct ProducerData{
	int ID;
	int low;
	int high;
};

struct ConsumerData{
};

ProducerData	* producer_data;
ConsumerData	* consumer_data;

void *ProducerFunction(void * producer_data);
void *ConsumerFunction(void * consumer_data);

pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_cond_t producer_condition;
pthread_cond_t consumer_condition;

int main(){

	ProduceConsume(2, 4);
	cout << Production_Count << endl;

	system("pause");
	return 0;
}

void ProduceConsume(int maxProducers, int maxConsumers){
	pthread_mutex_init(&producer_mutex, NULL);
	pthread_mutex_init(&consumer_mutex, NULL);
	pthread_cond_init(&producer_condition, NULL);
	pthread_cond_init(&consumer_condition, NULL);

	Production_Count = 0;
	buff = new SalesRecord[MAX_SIZE];
	producerThreads = new pthread_t[maxProducers];
	consumerThreads = new pthread_t[maxConsumers];
	producer_data = new ProducerData[maxProducers];
	consumer_data = new ConsumerData[maxConsumers];

	Section_Size = MAX_SIZE / maxProducers;

	//create consumers first so that they are ready to consume when the first producer signals
	for (int i = 0; i < maxConsumers; i++){
		pthread_create(&consumerThreads[i], NULL, ConsumerFunction, (void*) &consumer_data[i]);
	}
	//create producers and set their IDs
	for (int i = 0; i < maxProducers; i++){
		producer_data[i].ID = i;
		pthread_create(&producerThreads[i], NULL, ProducerFunction, (void*) &producer_data[i]);
	}

	//wait for threads to finish
	for (int i = 0; i < maxConsumers; i++)
		pthread_join(consumerThreads[i], NULL);
	for (int i = 0; i < maxProducers; i++)
		pthread_join(producerThreads[i], NULL);
	
	//destroy arrays
	delete[] producer_data;
	delete[] consumer_data;

	pthread_mutex_destroy(&producer_mutex);
	pthread_mutex_destroy(&consumer_mutex);
	pthread_cond_destroy(&producer_condition);
	pthread_cond_destroy(&consumer_condition);
}

void *ProducerFunction(void * producer_data){
	ProducerData * pd = (ProducerData*) producer_data;

	//divide threads along the buffer
	pd->low = 0 + Section_Size * pd->ID;
	pd->high = pd->low + Section_Size - 1;
	if(pd->ID == maxProducers - 1)
		pd->high = MAX_SIZE - 1;

	for(int i = pd->low; i <= pd->high && Production_Count < MAX_SIZE; i++){
		pthread_mutex_lock(&producer_mutex); //lock the shared count
		Production_Count++;
		pthread_mutex_unlock(&producer_mutex); //unlock the shared count
		SalesRecord record = SalesRecord(pd->ID); //create a sales record assigning the threadID to the sales record's store ID
		//if (Production_Count < MAX_SIZE)  //may use this condition if there is a race condition
		buff[i] = record;
	}

	pthread_exit(NULL);
	return 0;
}


void *ConsumerFunction(void * consumer_data){
	//not done
	pthread_exit(NULL);
	return 0;
}