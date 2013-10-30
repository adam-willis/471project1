#include <iostream>
#include <pthread.h>
#define NUM_THREADS 5

using namespace std;

int sum=0;
int Array[30]={10,15,20,25,30,35,0,10,20,30,40,50,2,12,22,32,42,52,4,14,24,34,44,54,5,15,25,35,45,55};

//function protos
void *CallFunction(void *t);
void Task(int id);

int main ()
{
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
	cout<<"All threads have finished"<<endl;
	cout<<"Total of all thread sums is: "<<sum<<endl;

	return 0;
}


void *task(int id)
{
	cout<<"Task: "<<id<<" started"<<endl;
	int start=(id*6);
	int end=(id*6+6)-1;
	int i=start;
	int result=0;
	for (i;i<=end;i++)
	{
		result=result+Array[i];
		cout<<"The data at index: "<<i<<" is: "<<Array[i]<<endl;
	}
	cout<<"Task: "<<id<<" completed with result: "<<result<<endl;
	sum=sum+result;
	return 0;
}

void *CallFunction(void *t)
{
	long id=(long)t;
	cout<<"Thread: "<<t<<" started"<<endl;
	task(id);
	cout<<"Thread: "<<t<<" done"<<endl;
	pthread_exit(0);
	return 0;
}