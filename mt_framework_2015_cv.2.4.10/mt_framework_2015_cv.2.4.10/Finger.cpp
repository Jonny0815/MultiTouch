#include "Finger.h"



Finger::Finger(Point2f coor_h, int frame_h) :id(counter++)
{
	x = coor_h.x;
	y = coor_h.y;

	

	frame = frame_h;

}


Finger::~Finger()
{
}

float Finger::get_distance(Point2f coor_h)
{

	float a = x - coor_h.x;
	float b = y - coor_h.y;

	float result = sqrt(((a * a) + (b * b)));

	if (result > radius)
	{
		return -1;
	}
	else
	{
		return result;
	}

	active = true;

}

int Finger::get_frame()
{
	return frame;
}

string Finger::get_info()
{
	string tbr = "ID:" + to_string(id);
	return tbr;
}

Point2f Finger::get_pos()
{
	return Point2f(x,y);
}

void Finger::deactivate()
{
	active = false;
}

bool Finger::get_active()
{
	return active;
}

int Finger::get_id()
{
	return id;
}

float Finger::get_x_norm(int framex)
{
	return x/framex;
}

float Finger::get_y_norm(int framey) {
	return y / framey;
}

void Finger::update(Point2f coor_h, int frame_h)
{
	x = coor_h.x;
	y = coor_h.y;
	frame = frame_h;

}


int Finger::counter;
