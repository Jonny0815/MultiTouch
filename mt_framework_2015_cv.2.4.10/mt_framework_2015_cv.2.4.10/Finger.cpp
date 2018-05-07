#include "Finger.h"

//volatile int Finger::counter = 0;





Finger::Finger():id(counter++)
{
	this->x = 0;
	this->y = 0;
	
	id = this->counter++;
	this->radius = 10;
}



Finger::~Finger()
{
}


int Finger::getID()
{
	return this->id;
}

void Finger::setID(int id)
{
	this->id = id;
}

void Finger::setCoordiates(int x, int y, int counterFrame)
{

	this->x = x;
	this->y = y;
	this->counterFrame = counterFrame;
}

double Finger::itsMe(int x, int y)
{
	if (getDistance(x, y) > radius) {
		return -1;
	}

	else {
		return getDistance(x,y);
	}

}

double Finger::getDistance(int x, int y)
{
	int a = this->x - x;
	int b = this->y - y;


	return sqrt(((a * a) + (b * b)));
}

int Finger::getcounterFrame()
{
	return this->counterFrame;
}


int Finger::counter;


