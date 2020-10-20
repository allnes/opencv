/**
  @file videocapture_basic.cpp
  @brief A very basic sample for using VideoCapture and VideoWriter
  @author PkLab.net
  @date Aug 24, 2016
*/

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
    Mat frame;
    Mat buffer;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_MSMF;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open("organfinale.mp3", apiID);
    cap.set(CAP_SWITCH_AUDIO_STREAM,15);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open file\n";
        return -1;
    }
    int count =0;
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    cout << "asd" << endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        buffer.push_back(frame);
        //cout << frame << endl;
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        count++;
        //std::cout << frame << std::endl;
        // show live and wait for a key with timeout long enough to show images
        //imshow("Live", frame);
        //if (waitKey(5) >= 0)
        //    break;
    }
    cout << count << endl;
    //cout << buffer.rows << endl;
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
