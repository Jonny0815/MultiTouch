/* Multitouch Finger Detection Framework v4.0
* for VS2013 & OpenCV 2.4.10
*
* Bjoern Froemmer, January 2010 - March 2015
*/

#include "opencv2/opencv.hpp"
#include <time.h>
#include <math.h>

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

	cv::Mat frame, original, grey, background;

	

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

		cvtColor(original, grey, CV_BGR2GRAY); // convert frame to greyscale image (copies the image in the process!)

		//--------------------------
		// your code goes here
		//--------------------------


		absdiff(original, background, original);


		if (cv::waitKey(1) == 27) // wait for user input
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
	
		imshow("window", original); // render the frame to a window	
	}

	std::cout << "SUCCESS: Program terminated like expected.\n";
	return 1;
}