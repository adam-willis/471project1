#include "SaleItem.h"
//default constructor should never be used, but it's there
SalesRecord::SalesRecord()
{
	Day=10;
	Month=0;
	Year=0;
	Register=0;
	SaleAmount=0.0;
	StoreID=0;
}
//constructor with the number of threads as a parameter
SalesRecord::SalesRecord(int ThreadID)
{
	//random day between 1-30
	Day=RandomInt(30);
	//random month between 1-12
	Month=RandomInt(12);
	//year is always 06
	Year=06;
	//Random number between 1-10
	Register=RandomInt(10);
	//Random amount between .50 and 999.99
	SaleAmount=RandomAmount();
	//Random Store ID - This might be the actual thread ID instead of a random number between 1-p
	//I believe you are right!
	StoreID=ThreadID;
}

//accessors
int SalesRecord::getDay() const
{
	return Day;
}

int SalesRecord::getMonth() const
{
	return Month;
}

int SalesRecord::getYear() const
{
	return Year;
}

int SalesRecord::getRegister() const
{
	return Register;
}

double SalesRecord::getSaleAmount() const
{
	return SaleAmount;
}

int SalesRecord::getStoreID() const
{
	return StoreID;
}

//helper functions
int SalesRecord::RandomInt(int HighNum)
{
	int Result=(rand()%HighNum)+1;
	return Result;
}

double SalesRecord::RandomAmount()
{
	double Result=(double)((float)rand()/RAND_MAX*1000.0);
	//Round anything less .50 up to .50 (rare event, but it can happen)
	if (Result<.50)
		Result=.50;
	return Result;
}

//display
void SalesRecord::display() const
{
	cout<<"Date: "<<setw(2)<<setfill('0')<<getDay()<<"/"
		<<setw(2)<<setfill('0')<<getMonth()<<"/"
		<<setw(2)<<setfill('0')<<getYear()<<"  "
		<<"Register #: "<<getRegister()<<"  "
		<<"Sale amount: "<<getSaleAmount()<<"  "
		<<"Store ID: "<<getStoreID()<<endl;
}