#include <cstdlib>
#include <Windows.h>
#include <pthread.h>
#include <semaphore.h>
#include "SaleItem.h"

//Function Prototypes
void *ProducerFunction (void *);
void *ConsumerFunction (void *);
void DisplayLocalConsumerData();
void DisplayGlobalStatistics();

//Shared Values
const int NUM_ITEMS=100;					//This can actually stay constant after program is running
const int NUM_PRODUCERS=5;					//Represents user input, will change to dynamic
const int NUM_CONSUMERS=5;					//Represents user input, will change to dynamic
const int BSIZE=10;							//Represents user input, will change to dynamic
int TotalProduced=0, TotalConsumed=0;		//global values to track the number produced/consumed
int ReadPosition=0, WritePosition=0;		//the read/right position in the buffer
double StoreTotals[NUM_PRODUCERS];			//Store-wide total sales (Store held in respective index)
double MonthTotals[13];						//Month-wide total sales (Month held in respective index)
SalesRecord Buffer[BSIZE];					//Used to hold StoreItems Produced Until Consumed

/*Problems to overcome with making the above values dynamic:
Buffer size is needed to track location of read/write - can make a SaleItem pointer and update size after user input
Producer size is needed - Used to create threads(already done dynamically) and consumer must track producer data
Consumer size is needed - USed to create threads(already done dynamically) and ConsumerData structs needs number */
					
sem_t MakeItem, TakeItem;					//Semaphore to indicate if buffer is full or has space
sem_t ProducedMutex, ConsumedMutex;			//Semaphore to ensure mutex on TotalProduced, TotalConsumed
sem_t ReadMutex, WriteMutex;				//Semaphore to ensure mutex on ReadPosition, WritePosition 

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

int main()
{
clock_t t1, t2;								//time variables for start time and end time
t1=clock();									//starting time
int rc1, rc2;								//return codes if threads fail to create
											
//Create the arrays to hold the producers and consumer threads
ProducerThreads=new pthread_t[NUM_PRODUCERS];
ConsumerThreads=new pthread_t[NUM_CONSUMERS];

//initialize the semaphores 
//parameters for sem_init(pointer to the semaphore, level of sharing, initial value)
sem_init(&MakeItem, 0, BSIZE);
sem_init(&TakeItem, 0, 0);
sem_init(&ReadMutex, 0, 1);
sem_init(&WriteMutex, 0, 1);
sem_init(&ProducedMutex, 0, 1);
sem_init(&ConsumedMutex, 0, 1);

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

//wait for threads to finish
for (int i = 0; i < NUM_CONSUMERS; i++)
	pthread_join(ConsumerThreads[i], NULL);
for (int i = 0; i < NUM_PRODUCERS; i++)
	pthread_join(ProducerThreads[i], NULL);

//print the buffer for debugging
cout<<"The current buffer (last "<<BSIZE<<" produced):"<<endl;
for(int i = 0; i < BSIZE; i++)
{
	Buffer[i].display();
    cout <<endl;
}

cout<<"Total Produced: "<<TotalProduced<<endl;

//Display the Local Consumer Data
DisplayLocalConsumerData();

//Display the Global Statistics
DisplayGlobalStatistics();


//perform cleanup of any dynamically allocated items
sem_destroy(&MakeItem);
sem_destroy(&TakeItem);
sem_destroy(&ReadMutex);
sem_destroy(&WriteMutex);
sem_destroy(&ProducedMutex);
sem_destroy(&ConsumedMutex);

t2=clock();								//ending time
float diff ((float)t2-(float)t1);
cout<<"The Total Run Time Was: "<<diff/CLOCKS_PER_SEC<<" Seconds."<<endl;
system("pause");

return 0;
}

void *ProducerFunction (void *t)
{
	int ProducerID=(long)t;
	cout<<"Producer: "<<ProducerID<<" started"<<endl;
	//pick a new random seed for each thread
	srand((int)time(NULL)^(int)ProducerID);

	//infinate loop until treads complete
	while(1)
	{
		sem_wait(&ProducedMutex);			//All producers wait while checking the number produced
		if (TotalProduced==NUM_ITEMS)		//Check NUM_ITEMS has been produced yet
		{
			sem_post(&ProducedMutex);		//Remove wait after answer is known
			cout<<"Producer: "<<ProducerID<<" done"<<endl;
			pthread_exit(NULL);				//Exit thread if true
		}
		TotalProduced++;					//Increment since an item will be produced when thread has opportunity
		sem_post(&ProducedMutex);			//Remove wait after TotalProduced is updated
		
		sem_wait(&MakeItem);				//Do Nothing - wait for signal from consumers to make an item

		//No longer waiting - there is now free buffer space
		sem_wait(&WriteMutex);				//Other producers wait while the buffer is written to and WritePostion is changed
		Buffer[WritePosition]=SalesRecord(ProducerID);
		WritePosition=(WritePosition+1)%BSIZE;
		sem_post(&WriteMutex);				//Remove wait on buffer and WritePosition
		sem_post(&TakeItem);				//Signal waiting consumers it's ok to take an item
		Sleep(int(rand()%36)+5);			//randomly sleep for 5-40 milliseconds
	}
	return 0;
}

void *ConsumerFunction (void *t)
{
	int ConsumerID=(long)t;
	cout<<"Consumer: "<<ConsumerID<<" started"<<endl;

	//infinate loop until threads complete
	while(1)
	{
		sem_wait(&ConsumedMutex);			//All consumers wait while checking the number consumed
		if(TotalConsumed==NUM_ITEMS)		
		{
			sem_post(&ConsumedMutex);		//Remove wait after answer is known
			cout<<"Consumer: "<<ConsumerID<<" done"<<endl;
			pthread_exit(NULL);				//Exit thread if true
		}
		TotalConsumed++;					//Increment since an item will be consumed when thread has opportunity
		sem_post(&ConsumedMutex);			//Remove wait after TotalConsumed is updated
		this_thread:sleep:(10);
		sem_wait(&TakeItem);				//Do Nothing - wait for signal from producer that an item is available

		//No longer waiting on item to be made
		sem_wait(&ReadMutex);				//Other consumers wait while the item is being read and ReadPosition is changed
		CData[ConsumerID].ID=ConsumerID;
		CData[ConsumerID].ConMonthTotals[Buffer[ReadPosition].getMonth()]+=Buffer[ReadPosition].getSaleAmount();
		CData[ConsumerID].ConStoreTotals[Buffer[ReadPosition].getStoreID()]+=Buffer[ReadPosition].getSaleAmount();
		ReadPosition=(ReadPosition+1)%BSIZE;
		sem_post(&ReadMutex);				//Remove wait on buffer and ReadPosition
		sem_post(&MakeItem);				//Signal Producers that it's ok to make an item
	}	
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