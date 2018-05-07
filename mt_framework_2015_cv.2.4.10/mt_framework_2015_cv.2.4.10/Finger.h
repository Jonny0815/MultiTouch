#pragma once
#include <math.h>

class Finger
{
public:
	Finger();
	~Finger();
	Finger(int &counter);
	int getID();
	void setID(int id);
	void setCoordiates(int x, int y, int counterFrame);
	double itsMe(int x, int y);
	double getDistance(int x, int y);
	int getcounterFrame();
	

	static int counter;



private:
	int x;
	int y;
	int radius;
	int id;
	int counterFrame = 0;




};

