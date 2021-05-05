//
// Created by swirta on 02.05.2021.
//
#include "../include/CaptureVideo.hpp"
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void Capture::open() {
    camera.open(0);
    if (!camera.isOpened())
        throw std::runtime_error("ERROR: Unable to open camera");

    camera.set(cv::CAP_PROP_FRAME_WIDTH,1280);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT,720);
}

cv::Mat Capture::capture() {

    camera.read(rawFrame);
    if(rawFrame.empty())
        throw std::runtime_error("ERROR: Read frame is empty");

    return rawFrame;
}