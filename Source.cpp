#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include "SaleItem.h"

#define MAX_SIZE 200
#define BUFF_SIZE 20

using namespace std;

SalesRecord * buff;
pthread_t * producerThreads;
pthread_t * consumerThreads;

int Production_Count;
int Section_Size;
int maxProducers=2;
int maxConsumers=4;

//function proto types
void ProduceConsume(int maxProducers, int maxConsumers);
void *ProducerFunction(void * producer_data);
void *ConsumerFunction(void * consumer_data);

struct ProducerData{
	int ID;
	int low; //keeps track of low index for producer
	int high; //keeps track of the high index for producer
	int writehere; //where the producer writes next
	int readhere; //where the consumer reads from next
	int count; //current count of items producer has in its buffer section
};

struct ConsumerData{
};

ProducerData	* producer_data;
ConsumerData	* consumer_data;

pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_cond_t producer_condition;
pthread_cond_t consumer_condition;

int main(){

	ProduceConsume(maxProducers, maxConsumers);
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
	buff = new SalesRecord[BUFF_SIZE];

	producerThreads = new pthread_t[maxProducers];
	consumerThreads = new pthread_t[maxConsumers];
	producer_data = new ProducerData[maxProducers];
	consumer_data = new ConsumerData[maxConsumers];

	Section_Size = BUFF_SIZE / maxProducers;

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

	//output of buffer - Debugging tool
	for (int i=0; i<BUFF_SIZE; i++){
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
	ProducerData * pd = (ProducerData*) producer_data;
	//seed each thread by raising seed by the power of the thread/producer ID
	srand(time(NULL)^pd->ID);
	//divide threads along the buffer
	pd->low = 0 + Section_Size * pd->ID;
	pd->high = pd->low + Section_Size - 1;
	if(pd->ID == (maxProducers - 1))
		pd->high = BUFF_SIZE - 1;

pd->count=0;
	//******************** Changed to track each section's start/stop ******************************

	for(int i = pd->low; (i <= pd->high) && (Production_Count < BUFF_SIZE); i++){
		
		pthread_mutex_lock(&producer_mutex); //lock the shared count
		Production_Count++; //This is the global production count NOT the producers 
		pthread_mutex_unlock(&producer_mutex); //unlock the shared count
		
		//if (Production_Count < MAX_SIZE)  //may use this condition if there is a race condition
		buff[i] = SalesRecord(pd->ID);
		pd->count=pd->count++;

				pthread_mutex_lock(&producer_mutex); //lock the shared count
		cout<<"Producer: "<<pd->ID<<" Current Count"<<pd->count<<endl;
				pthread_mutex_unlock(&producer_mutex); //unlock the shared count


	//*************************************************************************************************

	}

	pthread_exit(NULL);
	return 0;
}


void *ConsumerFunction(void * consumer_data){
	//not done
	pthread_exit(NULL);
	return 0;
}