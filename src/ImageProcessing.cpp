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
        Mat rawFrame, frameHSV;
        camera.read(rawFrame);
        flip(rawFrame, frame, 1);
        Scalar lowerLimit(hueMin, saturationMin, valueMin);
        Scalar upperLimit(hueMax, saturationMax, valueMax);
        cvtColor(frame, frameHSV, COLOR_BGR2HSV);
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

pair<Point2f, float> ImageProcessing::find_marker(cv::Mat frame) {
    vector<vector<Point>> contours;
    Mat frameHSV, gameFrame;
    frame.copyTo(gameFrame);
    medianBlur(frame, frame, 15);
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);
    inRange(frameHSV, Scalar(hueMin, saturationMin, valueMin), Scalar(hueMax, saturationMax, valueMax), frame);
    morphologyEx(frame, frame, MORPH_OPEN, getStructuringElement(MorphShapes::MORPH_RECT, Size(10,10)));
    morphologyEx(frame, frame, MORPH_CLOSE, getStructuringElement(MorphShapes::MORPH_RECT, Size(10, 10)));
    findContours(frame, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> contoursPoly(contours.size());
    vector<pair<Point2f, float>> circles(contours.size());
    try{
        for(size_t i = 0; i < contours.size(); ++i){
            approxPolyDP(contours[i], contoursPoly[i], 3, true);
            minEnclosingCircle(contours[i], circles[i].first, circles[i].second);
        }
    }
    catch (const cv::Exception &e){
        cerr << e.what() << endl;
    }
    if(!circles.empty()) {
        sort(circles.begin(), circles.end(), [](const auto &x, const auto &y) { return y.second < x.second; });

        circle(gameFrame, circles[0].first, circles[0].second, {255, 0, 0});
        circle(gameFrame, circles[0].first, 1, {0, 0, 255});
    }
    imshow("test", gameFrame);
    cerr << circles.size() << endl;
    return pair<Point2f, float>(Point2f(0.0, 0.0), 1.0);
}

void ImageProcessing::run() {
    cv::Mat frame, rawFrame;
    camera.open(0);    //DEFAULT API INIT

    if(!camera.isOpened()){
        return;
    }

    while(true){
        //waits for a new farme from camera and store it into 'frame'
        camera.read(rawFrame);
        if(rawFrame.empty()){
            return;
        }
        flip(rawFrame, frame, 1);
        //shows camera feed and waits with timeout
        find_marker(frame);
        //circle(frame, marker.centre, marker.radius, Scalar(0, 0, 255), 5);
        //cv::imshow("Live", frame);
        int key = cv::waitKey(25);
        if(key == 109)
            modify_color_search(frame);
        if(key == 27)
            break;
    }
}