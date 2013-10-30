#include <iostream>
#include <iomanip>
using namespace std;

class SalesRecord
{
public:

//default constructor
SalesRecord();
//constructor with parameters requires the number of Stores/Producers
SalesRecord(int);

//accessors
int getDay() const;
int getMonth() const;
int getYear() const;
int getRegister() const;
double getSaleAmount() const;
int getStoreID() const;

//display
void display() const;

//helper functions
int RandomInt(int); //Generates a random int for day, month, and register
double RandomAmount(); //returns a random price within the given limits

private:
int Day, Month, Year, Register, StoreID;
double SaleAmount;
};