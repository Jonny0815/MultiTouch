#pragma once

#include "opencv2/opencv.hpp"
#include "TuioCursor.h"
#include <vector>
#include <string>

using namespace std;
using namespace cv;

class Finger
{
public:
	Finger(Point2f, int);
	~Finger();

	float get_distance(Point2f);
	int get_frame();
	string get_info();
	Point2f get_pos();

	void deactivate();
	bool get_active();
	int get_id();

	float get_x_norm(int);
	float get_y_norm(int);

	void update(Point2f, int);

	static int counter;

private:
	float x;
	float y;
	int frame;
	int id;

	bool active;

	const int radius = 30;
};

