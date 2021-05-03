//
// Created by swirta on 27.04.2021.
//

#include "../include/Snake.hpp"

#include "../include/ImageProcessing.hpp"
#include <iostream>
using namespace std;
using namespace cv;

void ImageProcessing::modify_color_search() {
    namedWindow("Edit marker color");
    createTrackbar("Hue Min", "Edit marker color", &hueMin, 179);
    createTrackbar("Hue Max", "Edit marker color", &hueMax, 179);
    createTrackbar("Saturation Min", "Edit marker color", &saturationMin, 255);
    createTrackbar("Saturation Max", "Edit marker color", &saturationMax, 255);
    createTrackbar("Value Min", "Edit marker color", &valueMin, 255);
    createTrackbar("Value Max", "Edit marker color", &valueMax, 255);

    while(true){
        Mat frame, rawFrame, frameHSV;
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

pair<Point2f, float> ImageProcessing::find_marker(Mat &frame) {
    vector<vector<Point>> contours;
    Mat frameHSV;
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
        //sort(circles.begin(), circles.end(), [](const auto &x, const auto &y) { return y.second < x.second; });
//
//        circle(gameFrame, circles[0].first, circles[0].second, {255, 0, 0});
//        circle(gameFrame, circles[0].first, 1, {0, 0, 255});
        auto marker = min_element(circles.begin(), circles.end(), [](const auto &x, const auto &y) {return y.second < x.second; });
        return *marker;
    }
    return pair<Point2f, float>(Point2f(0.0, 0.0), 1.0);
}

void ImageProcessing::run() {
    cv::Mat frame, rawFrame, gameFrame;

    Snake snake;
    for(int i = 1; i <= 10; ++i)
        snake.grow(Point(1000 - 5*i, 100));

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
        frame.copyTo(gameFrame);
        pair<Point, float> marker = find_marker(frame);
        snake.move(marker.first);
        if(snake.check_snake())
            cerr << "kolizja" << endl;

        snake.draw(gameFrame);

        circle(gameFrame, marker.first, marker.second, {255, 0, 0});
        circle(gameFrame, marker.first, 1, {0, 0, 255});
        cv::imshow("Live", gameFrame);

        int key = cv::waitKey(1);
        if(key == 109)
            modify_color_search();
        if(key == 27)
            break;
    }
}