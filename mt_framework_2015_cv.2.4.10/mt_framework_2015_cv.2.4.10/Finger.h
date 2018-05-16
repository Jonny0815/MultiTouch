#pragma once

#include "opencv2/opencv.hpp"
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

	void update(Point2f, int);

	static int counter;

private:
	int x;
	int y;
	int frame;
	int id;

	const int radius = 20;
};

