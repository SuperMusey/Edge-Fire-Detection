#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{   
    // Set to /dev/video0
    VideoCapture capture(0);
    capture.set(CAP_PROP_FOURCC,0x47504A4D);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,480);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
    // Capturing a single frame/image
    Mat frame;
    capture >> frame;
    if(frame.empty()){
	cout << "Failed to capture an image" << endl;
	return -1;
    }
    imwrite("image.png", frame);
    return 0;
}
