#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include "SaleItem.h"

#define NUM_PRODUCERS 2				//Number of Producers might change to dynamic if time
#define NUM_CONSUMERS 2					//Number of Consumers might change to dynamic if time
#define BSIZE 10						//Size of buffer shared by consumers and producers
#define NUM_ITEMS  10					//determines how many will be created

//shared variables
int ProductionCount=0;					//track the total SalesRecords produced
int ConsumptionCount=0;					//track the total SalesRecords consumed
int ReadPosition=0;						//track the buffer read position
int WritePosition=0;					//track the buffer write position
int ReadyItems=0;						//number of items in buffer - will not need after semaphore
double MonthTotals[13];					//each index holds a corresponding month total (index based on month 1-12 used)
double StoreTotals[NUM_PRODUCERS];		//each index holds a corresponding store total (index based on producer number)
SalesRecord Buffer[BSIZE];				//Used to hold StoreItems Produced Until Consumed

struct ConsumerData
{
	int ID;									//consumer thread ID 
	double ConMonthTotals[13];				//monthly totals for each consumer (index based on month 1-12 used)
	double ConStoreTotals[NUM_PRODUCERS];	//store totals for each consumer (index based on producer number)
};
ConsumerData CData[NUM_CONSUMERS];			//array to hold ConsumeData structs

//Pointers to arrays for producerThreads, ConsumerThreads
pthread_t * ProducerThreads;
pthread_t * ConsumerThreads;

//function protos
void *ProducerFunction (void *);
void *ConsumerFunction (void *);
void DisplayLocalConsumerData();
void DisplayGlobalStatistics();

pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_cond_t producer_condition;
pthread_cond_t consumer_condition;

int main()
{
	int rc1, rc2; //return codes if threads fail to create

	//Create the arrays to hold the producer and consumer threads
	ProducerThreads=new pthread_t[NUM_PRODUCERS];
	ConsumerThreads=new pthread_t[NUM_CONSUMERS];
	
	//create the consumers
	for (int t=0; t<NUM_CONSUMERS; t++)
	{
		cout<<"Creating consumer thread: "<<t<<endl;
		rc1=pthread_create(&ConsumerThreads[t], NULL, ConsumerFunction, (void*) t);
		if (rc1)
		{
			cout<<"Error, return code from pthread_create is: "<<rc1<<endl;
			return 1;
		}
	}

	//create the producers
	for (int t=0; t<NUM_PRODUCERS; t++)
	{
		cout<<"Creating producer thread: "<<t<<endl;
		rc2=pthread_create(&ProducerThreads[t], NULL, ProducerFunction, (void*) t);
		if (rc2)
		{
			cout<<"Error, return code from pthread_create is: "<<rc2<<endl;
			return 1;
		}
	}

	pthread_mutex_init(&producer_mutex, NULL);
	pthread_mutex_init(&consumer_mutex, NULL);
	pthread_cond_init(&producer_condition, NULL);
	pthread_cond_init(&consumer_condition, NULL);

	//wait for threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++)
		pthread_join(ConsumerThreads[i], NULL);
    for (int i = 0; i < NUM_PRODUCERS; i++)
		pthread_join(ProducerThreads[i], NULL);

	//print the buffer for debugging
    for(int i = 0; i < BSIZE; i++)
	{
		Buffer[i].display();
        cout <<endl;
	}

	cout<<"Production Count: "<<ProductionCount<<endl;

	//Display the Local Consumer Data
	DisplayLocalConsumerData();

	//Display the Global Statistics
	DisplayGlobalStatistics();

	system("pause");
	return 0;
}

void *ProducerFunction (void *t)
{
	int ProducerID=(long)t;
	cout<<"Producer: "<<ProducerID<<" started"<<endl;
	srand((int)time(NULL)^(int)ProducerID);//pick a new random seed for each thread

	while (ProductionCount<NUM_ITEMS)  //Keep trying to produce until NUM_ITEMS has been produced
	{
		while (ReadyItems==BSIZE || ReadyItems>BSIZE); //do nothing if there is no room to write - will be controlled by semaphore

		//** No longer waiting, time to produce **//
		pthread_mutex_lock(&producer_mutex);
		Buffer[WritePosition]=SalesRecord(ProducerID);
		cout<<"Index: "<<WritePosition<<" being written to by producer: "<<ProducerID<<endl;
		WritePosition=(WritePosition+1)%BSIZE;	//advance write head
		ReadyItems++;							//increment items ready for consumption
		ProductionCount++;						//increment overall production count
		pthread_mutex_unlock(&producer_mutex);
	}
	cout<<"Producer: "<<ProducerID<<" done"<<endl;
	pthread_exit(0);
	return 0;
}

void *ConsumerFunction (void *t)
{
	int ConsumerID=(long)t;
	cout<<"Consumer: "<<ConsumerID<<" started"<<endl;

	while (ConsumptionCount<NUM_ITEMS)  //Keep trying to consume until NUM_ITEMS has been consumed
	{
		while (ReadyItems==0 || ReadyItems<0);  //do nothing if there is nothing to consume - will be controlled by semaphore

		//** No longer waiting, time to consume **//
		pthread_mutex_lock(&consumer_mutex);
		cout<<"Index: "<<ReadPosition<<" being read by consumer: "<<ConsumerID<<endl;
		//**This gets really ugly and cryptic, a different container perhaps?  Debating a consumer class.
		//**The calculations work with 1 producer, 1 consumer.
		CData[ConsumerID].ID=ConsumerID;
		CData[ConsumerID].ConMonthTotals[Buffer[ReadPosition].getMonth()]+=Buffer[ReadPosition].getSaleAmount();
		CData[ConsumerID].ConStoreTotals[Buffer[ReadPosition].getStoreID()]+=Buffer[ReadPosition].getSaleAmount();
		ReadPosition=(ReadPosition+1)%BSIZE;		//advance read head
		ReadyItems--;								//decrement items ready for consumption
		ConsumptionCount++;							//increment overall consumption count
		pthread_mutex_unlock(&consumer_mutex);
	}

	cout<<"Consumer: "<<ConsumerID<<" done"<<endl;
	pthread_exit(0);
	return 0;
}

void DisplayLocalConsumerData()
{
	for (int i=0; i<NUM_CONSUMERS; i++)
	{
		cout<<"========================================================"<<endl;
		cout<<"Local Consumer Data Report - Consumer: "<<CData[i].ID<<endl;
		cout<<"========================================================"<<endl;
		cout<<"Monthly Totals:"<<endl;
		cout<<"========================================================"<<endl;
		cout<<"Jan = $"<<CData[i].ConMonthTotals[1]<<endl;
		cout<<"Feb = $"<<CData[i].ConMonthTotals[2]<<endl;
		cout<<"Mar = $"<<CData[i].ConMonthTotals[3]<<endl;
		cout<<"Apr = $"<<CData[i].ConMonthTotals[4]<<endl;
		cout<<"May = $"<<CData[i].ConMonthTotals[5]<<endl;
		cout<<"Jun = $"<<CData[i].ConMonthTotals[6]<<endl;
		cout<<"Jul = $"<<CData[i].ConMonthTotals[7]<<endl;
		cout<<"Aug = $"<<CData[i].ConMonthTotals[8]<<endl;
		cout<<"Sep = $"<<CData[i].ConMonthTotals[9]<<endl;
		cout<<"Oct = $"<<CData[i].ConMonthTotals[10]<<endl;
		cout<<"Nov = $"<<CData[i].ConMonthTotals[11]<<endl;
		cout<<"Dec = $"<<CData[i].ConMonthTotals[12]<<endl<<endl;
		cout<<"========================================================"<<endl;
		cout<<"Store Totals:"<<endl;
		cout<<"========================================================"<<endl;
		for (int j=0; j<NUM_PRODUCERS; j++)
		{
			cout<<"Store "<<j<<" = $"<<CData[i].ConStoreTotals[j]<<endl;
		}
		cout<<endl<<endl;
	}
}

void DisplayGlobalStatistics()
{

	//store totals
	for (int i=0; i<NUM_PRODUCERS; i++)
	{
		for (int j=0; j<NUM_CONSUMERS; j++)
		{		
			StoreTotals[i]+=CData[j].ConStoreTotals[i];	
		}
	}
	cout<<"========================================================"<<endl;
	cout<<"Global Store Statistics: "<<endl;
	cout<<"========================================================"<<endl;
	for (int i=0; i<NUM_PRODUCERS; i++)
	{
		cout<<"Store "<<i<<" = $"<<StoreTotals[i]<<endl;
	}
	//month Totals
	for (int i=0; i<13; i++)
	{
		for (int j=0; j<NUM_CONSUMERS; j++)
		{
			MonthTotals[i]+=CData[j].ConMonthTotals[i];
		}
	}
	cout<<"========================================================"<<endl;
	cout<<"Global Monthly Statistics: "<<endl;
	cout<<"========================================================"<<endl;
	cout<<"Jan = $"<<MonthTotals[1]<<endl;
	cout<<"Feb = $"<<MonthTotals[2]<<endl;
	cout<<"Mar = $"<<MonthTotals[3]<<endl;
	cout<<"Apr = $"<<MonthTotals[4]<<endl;
	cout<<"May = $"<<MonthTotals[5]<<endl;
	cout<<"Jun = $"<<MonthTotals[6]<<endl;
	cout<<"Jul = $"<<MonthTotals[7]<<endl;
	cout<<"Aug = $"<<MonthTotals[8]<<endl;
	cout<<"Sep = $"<<MonthTotals[9]<<endl;
	cout<<"Oct = $"<<MonthTotals[10]<<endl;
	cout<<"Nov = $"<<MonthTotals[11]<<endl;
	cout<<"Dec = $"<<MonthTotals[12]<<endl<<endl;
}