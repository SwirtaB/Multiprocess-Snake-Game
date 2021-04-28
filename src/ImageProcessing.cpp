//
// Created by swirta on 27.04.2021.
//

#include "../include/ImageProcessing.hpp"
#include <iostream>
using namespace std;
using namespace cv;

void ImageProcessing::modify_color_search(Mat frame) {
    namedWindow("Edit marker color");
    createTrackbar("Hue Min", "Edit marker color", &hueMin, 179);
    createTrackbar("Hue Max", "Edit marker color", &hueMax, 179);
    createTrackbar("Saturation Min", "Edit marker color", &saturationMin, 255);
    createTrackbar("Saturation Max", "Edit marker color", &saturationMax, 255);
    createTrackbar("Value Min", "Edit marker color", &valueMin, 255);
    createTrackbar("Value Max", "Edit marker color", &valueMax, 255);

    while(true){
        Scalar lowerLimit(hueMin, saturationMin, valueMin);
        Scalar upperLimit(hueMax, saturationMax, valueMax);
        Mat frameHSV = generate_mask(frame);
        Mat mask;

        inRange(frameHSV, lowerLimit, upperLimit, mask);

        imshow("Edit marker color", mask);
        if(waitKey(1) == 109){
            destroyWindow("Edit marker color");
            break;
        }
    }
}

Mat ImageProcessing::generate_mask(cv::Mat frame) {
    Mat frameHSV;
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);
    return frameHSV;
}

void ImageProcessing::find_marker(cv::Mat frame) {

}

void ImageProcessing::run() {
    cv::Mat frame;
    //VIDEOCAPTURE INITIALIZE
    cv::VideoCapture capture;
    capture.open(0);    //DEFAULT API INIT

    if(!capture.isOpened()){
        return;
    }

    while(true){
        //waits for a new farme from camera and store it into 'frame'
        capture.read(frame);
        if(frame.empty()){
            return;
        }
        //shows camera feed and waits with timeout
        cv::imshow("Live", frame);
        int key = cv::waitKey(25);
        if(key == 109)
            modify_color_search(frame);
        if(key == 27)
            break;
    }
}