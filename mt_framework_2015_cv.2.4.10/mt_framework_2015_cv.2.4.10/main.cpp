/* Multitouch Finger Detection Framework v4.0
* for VS2013 & OpenCV 2.4.10
*
* Bjoern Froemmer, January 2010 - March 2015
*/

#include "opencv2/opencv.hpp"
#include <time.h>
#include <math.h>
#include <vector>
#include "Finger.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << " Usage: image_1 image_2" << endl;
		return -1;
	}

	Mat image1;
	Mat image2;

	Mat substracted;
	Mat binary;

	Size blursizebig, blursizesmall;

	blursizebig.height = 25;
	blursizebig.width = 25;

	blursizesmall.height = 5;
	blursizesmall.width = 5;

	vector<vector<Point>>  contours;
	vector<Vec4i>  hierarchy;


	image1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
	image2 = imread(argv[2], CV_LOAD_IMAGE_COLOR);


	if (!image1.data && !image2.data)                              // Check for invalid input
	{
		cout << "Could not open or find the images" << std::endl;
		return -1;
	}

	absdiff(image1, image2, substracted);

	

	blur(substracted, substracted, blursizebig);
	


	threshold(substracted, substracted, 8, 172, THRESH_BINARY);

	cvtColor(substracted, binary, CV_BGR2GRAY);

	findContours(binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	if (hierarchy.size() > 0)
	{
		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			// check contour size (number of points) and area ("blob" size)
			if (contourArea(Mat(contours.at(idx))) > 30 && contours.at(idx).size() > 4)
			{
				//ellipse(image1, fitEllipse(Mat(contours.at(idx))), Scalar(0, 0, 255), 1, 8); // fit & draw ellipse to contour at index
				drawContours(image1, contours, idx, Scalar(255, 0, 0), 1, 8,
					hierarchy); // draw contour at index
			}
		}
	}

	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", image1);                   // Show our image inside it.

	imwrite("C:\image.png", image1);

	waitKey(0);   // Wait for a keystroke in the window

	return 0;
}