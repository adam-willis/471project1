#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include "SaleItem.h"

#define MAX_SIZE 20

using namespace std;

SalesRecord * buff;
pthread_t * producerThreads;
pthread_t * consumerThreads;

void ProduceConsume(int maxProdroducers, int maxConsumers);


struct ProducerData{
	int ID;
	int low;
	int high;
};
ProducerData * producer_data;

struct ConsumerData{
};

ConsumerData * consumer_data;

void *ProducerFunction(void * producer_data);
void *ConsumerFunction(void * consumer_data);

int main(){
	

	//for (int i = 0; i < MAX_SIZE - 1; i++){
	//	SalesRecord record = SalesRecord(10);
	//	buff[i] = record;
	//}

	

	for (int i = 0; i < MAX_SIZE - 1; i++){
		buff[i].display();
		cout << endl;
	}

	system("pause");
	return 0;
}

void ProduceConsume(int maxProducers, int maxConsumers){
	buff = new SalesRecord[MAX_SIZE];
	producerThreads = new pthread_t[maxProducers];
	consumerThreads = new pthread_t[maxConsumers];
	producer_data = new ProducerData[maxProducers];
	consumer_data = new ConsumerData[maxConsumers];

	for (int i = 0; i < maxConsumers; i++){
		pthread_create(&consumerThreads[i], NULL, ConsumerFunction, (void*) &consumer_data[i]);
	}

	for (int i = 0; i < maxProducers; i++){
		producer_data[i].ID = i;
		pthread_create(&producerThreads[i], NULL, ProducerFunction, (void *) &producer_data[i]);
	}


	for (int i = 0; i < maxConsumers; i++){
		pthread_join(consumerThreads[i], NULL);
	}

	for (int i = 0; i < maxProducers; i++){
		pthread_join(producerThreads[i], NULL);
	}

	delete(producer_data);
	delete(consumer_data);

}

void *ProducerFunction(void * producer_data){

	return 0;
}


void *ConsumerFunction(void * consumer_data){

	return 0;
}