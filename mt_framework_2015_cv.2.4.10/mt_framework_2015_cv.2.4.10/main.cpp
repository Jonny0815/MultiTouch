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

int main(void)
{
	//VideoCapture cap(0); // use the first camera found on the system
	cv::VideoCapture cap("C:/mt_camera_raw.AVI");


	if (!cap.isOpened())
	{
		std::cout << "ERROR: Could not open camera / video stream.\n";
		return -1;
	}

	// get the frame size of the videocapture object
	double videoWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double videoHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	cv::Mat frame, original, grey, background, background_blured, binary, source;

	Size blursizebig, blursizesmall;

	blursizebig.height = 17;
	blursizebig.width = 17;

	blursizesmall.height = 5;
	blursizesmall.width = 5;


	vector<vector<Point>>  contours; 
	vector<Vec4i>  hierarchy;
	vector<Finger*> fingers;

	RotatedRect rr;
	Finger* best_match;

	int currentFrame = 0; // frame counter
	clock_t ms_start, ms_end, ms_time; // time

	char buffer[10]; // buffer for int to ascii conversion -> itoa(...)

	cap >> background;

	for (;;)
	{
		ms_start = clock(); // time start

		cap >> frame; // get a new frame from the videostream

		if (frame.data == NULL) // terminate the program if there are no more frames
		{
			std::cout << "TERMINATION: Camerastream stopped or last frame of video reached.\n";
			break;
		}

		original = frame.clone(); // copy frame to original

		source = frame.clone();

		cvtColor(original, grey, CV_BGR2GRAY); // convert frame to greyscale image (copies the image in the process!)

		//--------------------------
		// your code goes here
		//--------------------------


		absdiff(original, background, original);

		blur(original, background_blured, blursizebig);
		absdiff(original, background_blured, original);
		blur(original, original, blursizesmall);

		threshold(original, original, 8, 172,THRESH_BINARY);

		cvtColor(original, binary, CV_BGR2GRAY);
		

		
		findContours(binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// iterate through all the top-level contours -> "hierarchy" may not be empty!)
		if (hierarchy.size() > 0)
		{
			for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
			{
				// check contour size (number of points) and area ("blob" size)
				if (contourArea(Mat(contours.at(idx))) > 30 && contours.at(idx).size() > 4)
				{
					rr = fitEllipse(Mat(contours.at(idx)));

					ellipse(source, rr, Scalar(0, 0, 255), 1, 8); // fit & draw ellipse to contour at index
					drawContours(source, contours, idx, Scalar(255, 0, 0), 1, 8, hierarchy); // draw contour at index

					best_match = nullptr; // reset best matching finger

					if (fingers.size() != 0)
					{
						double clostest = 150;
						

						for (size_t i = 0; i < fingers.size(); i++)
						{
							if (fingers.at(i)->get_distance(rr.center) > 0 && fingers.at(i)->get_distance(rr.center) < clostest && fingers.at(i)->get_active())
							{
								clostest = fingers.at(i)->get_distance(rr.center);
								best_match = fingers.at(i);
							}
						}

						if (best_match != nullptr)
						{
							best_match->update(rr.center, currentFrame);
						}
						else
						{
							fingers.push_back(new Finger(rr.center, currentFrame));
						}
						

					}
					else {
						fingers.push_back(new Finger(rr.center, currentFrame));
					}

				}
			}
		}
		
		

		if (fingers.size() != 0) // delete outdated fingers
		{
			
			for (unsigned int k = 0; k < fingers.size(); k++)
			{

				if (3 < currentFrame - fingers.at(k)->get_frame()) // finger hasnt been updated for 5+ frames
				{
					
					//delete fingers.at(k);
					fingers.at(k)->deactivate();
					

				}
			}
			
		}

		

		for (size_t i = 0; i < fingers.size(); i++) // draw coordinates and id
		{

			if (fingers.at(i)->get_active())
			{
				putText(source, fingers.at(i)->get_info(), CvPoint(fingers.at(i)->get_pos()), FONT_HERSHEY_SIMPLEX, 0.5, cvScalar(255, 255, 255), 1);
			}
		}

		if (cv::waitKey(50) == 27) // wait for user input
		{
			std::cout << "TERMINATION: User pressed ESC\n";
			break;
		}


		currentFrame++;

		// time end
		ms_end = clock();
		ms_time = ms_end - ms_start;

		putText(source, "frame #" + (std::string)_itoa(currentFrame, buffer, 10), cvPoint(0, 15), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write framecounter to the image (useful for debugging)
		putText(source, "time per frame: " + (std::string)_itoa(ms_time, buffer, 10) + "ms", cvPoint(0, 30), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write calculation time per frame to the image
	
		imshow("window", source); // render the frame to a window	
	}

	std::cout << "SUCCESS: Program terminated like expected.\n";
	return 1;
}