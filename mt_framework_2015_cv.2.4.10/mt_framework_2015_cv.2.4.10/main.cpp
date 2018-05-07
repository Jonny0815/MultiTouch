/* Multitouch Finger Detection Framework v4.0
* for VS2013 & OpenCV 2.4.10
*
* Bjoern Froemmer, January 2010 - March 2015
*/

#include "opencv2/opencv.hpp"
#include <time.h>
#include <math.h>
#include <vector>
#include <string>
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

	int counter = 0;

	cv::Mat frame, original, grey, background, background_blured, binary, source;

	Size blursizebig, blursizesmall;

	blursizebig.height = 17;
	blursizebig.width = 17;

	blursizesmall.height = 5;
	blursizesmall.width = 5;


	vector<vector<Point>>  contours; 
	vector<Vec4i>  hierarchy;
	vector<Finger> fingerVec;

	
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
		
		RotatedRect tempRect;
		
		findContours(binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// iterate through all the top-level contours -> "hierarchy" may not be empty!)
		if (hierarchy.size() > 0)
		{
			for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
			{
				Finger * besterFinger = nullptr;
				// check contour size (number of points) and area ("blob" size)
				if (contourArea(Mat(contours.at(idx))) > 30 && contours.at(idx).size() > 4)
				{
					tempRect = fitEllipse(Mat(contours.at(idx)));
					ellipse(source, tempRect,
						Scalar(0, 0, 255), 1, 8); // fit & draw ellipse to contour at index
					drawContours(source, contours, idx, Scalar(255, 0, 0), 1, 8,
						hierarchy); // draw contour at index


					if (fingerVec.size() != 0) {
						double nearest = 300;

						for (int i = 0; i < fingerVec.size(); i++)
						{
							double maybeMe = fingerVec.at(i).getDistance((int)tempRect.center.x, (int)tempRect.center.y);
							if (maybeMe > 0 && maybeMe < nearest) {
								nearest = maybeMe;
								besterFinger = &fingerVec.at(i);

							}

							if (besterFinger != nullptr) {
								besterFinger->setCoordiates((int)tempRect.center.x, (int)tempRect.center.y,currentFrame);
							}

							else {

								Finger fTemp;
								fTemp.setCoordiates((int)tempRect.center.x, (int)tempRect.center.y, currentFrame);
								fingerVec.push_back(fTemp);	
								besterFinger = &fTemp;
							}

						}

					}

					else {
						Finger fTemp;
						besterFinger = &fTemp;
						fTemp.setCoordiates((int)tempRect.center.x, (int)tempRect.center.y,currentFrame);
						fingerVec.push_back(fTemp);
					}
					


					
				}

				// show xy Coordanates and draw them
				if (besterFinger != nullptr) {
					String text = to_string((int)tempRect.center.x) + " " + to_string((int)tempRect.center.y) + " " + to_string(besterFinger->getID());
					putText(source, text, CvPoint(tempRect.center), FONT_HERSHEY_SIMPLEX, 0.5, cvScalar(255, 255, 255), 1);
					//cout << tempRect.center << endl;

				}
			}

			for (int i = 0; i < fingerVec.size(); i++) {

				if (5 >(currentFrame - fingerVec.at(i).getcounterFrame())) {

					fingerVec.erase(fingerVec.begin() + i);
				}

			}
			
		}
		


		if (cv::waitKey(100) == 27) // wait for user input
		{
			std::cout << "TERMINATION: User pressed ESC\n";
			break;
		}

		currentFrame++;

		// time end
		ms_end = clock();
		ms_time = ms_end - ms_start;

		putText(original, "frame #" + (std::string)_itoa(currentFrame, buffer, 10), cvPoint(0, 15), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write framecounter to the image (useful for debugging)
		putText(original, "time per frame: " + (std::string)_itoa(ms_time, buffer, 10) + "ms", cvPoint(0, 30), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write calculation time per frame to the image
	
		imshow("window", source); // render the frame to a window	
	}

	std::cout << "SUCCESS: Program terminated like expected.\n";
	return 1;
}