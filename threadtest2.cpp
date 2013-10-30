#include <pthread.h>
#include <conio.h>
 #include <windows.h>
#include "SaleItem.h"
#define NUM_THREADS 5 //Producers
#define BSIZE 30 //Buffer

SalesRecord Buffer[BSIZE];

//function protos
void *CallFunction(void *t);
void *Task(int id);

using namespace std;


int main ()
{
	//seed random
	srand(time(NULL));

	pthread_t thread[NUM_THREADS];
	int rc;
	long t;
	for (t=0; t<NUM_THREADS; t++)
	{
		cout<<"Creating thread: "<<t<<endl;
		rc=pthread_create(&thread[t], NULL, CallFunction, (void *)t);
		if (rc)
		{
			cout<<"Error: return code from pthread_create is: "<<rc<<endl;
			return 1;
		}
	}

	// wait for all threads to exit
	for (t=0; t<NUM_THREADS; t++)
	{
		pthread_join(thread[t], NULL);
	}

	cout<<"All threads have finished - Buffer holds: "<<endl;
		for (int i=0; i<BSIZE; i++)
	{
		cout<<"Index: "<<i<<"  ";
		Buffer[i].display();
	}

	return 0;
}

//The CallFunction is just a simple way for each thread
//to call a sub-function(s) to perform its different tasks
void *CallFunction(void *t)
{
	long id=(long)t;
	cout<<"Thread: "<<t<<" started"<<endl;
	Task(id); //call the task function
	cout<<"Thread: "<<t<<" done"<<endl;
	pthread_exit(0);
	return 0;
}

void *Task(int id)
{
	SalesRecord *CurrentRecord; //pointer for SalesRecord
	CurrentRecord=new SalesRecord(NUM_THREADS);	
	cout<<"Task: "<<id<<" started"<<endl;
	int start=(id*6);//starting index for thread
	int end=(id*6+6)-1;//ending index for thread
	int i=start;
	for (i; i<=end; i++)
	{
	cout<<"Index: "<<i<<" being updated by thread: "<<id<<endl;
	Buffer[i]=SalesRecord(NUM_THREADS);	
	}
	return 0;
}