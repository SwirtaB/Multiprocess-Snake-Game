//
// Created by swirta on 27.04.2021.
//

#include "../include/Snake.hpp"

#include "../include/ImageProcessing.hpp"
#include <iostream>
using namespace std;
using namespace cv;



void ImageProcessing::modify_color_search(cv::Mat& rec_frame) {

    create_filtering_window();

    Mat filtered_frame = perform_filtering(rec_frame);
    show_filtering_window(filtered_frame);

    int key = waitKey(1);

    if(key == M_BUTTON || key == ESC_BUTTON) {
        destroy_filtering_window();
        set_status(key == ESC_BUTTON ? END_STATUS : DISPLAY_STATUS);
    }
}

void ImageProcessing::create_filtering_window() {
    namedWindow(window_name);
    createTrackbar("Hue Min", "Edit marker color", &hueMin, 179);
    createTrackbar("Hue Max", "Edit marker color", &hueMax, 179);
    createTrackbar("Saturation Min", "Edit marker color", &saturationMin, 255);
    createTrackbar("Saturation Max", "Edit marker color", &saturationMax, 255);
    createTrackbar("Value Min", "Edit marker color", &valueMin, 255);
    createTrackbar("Value Max", "Edit marker color", &valueMax, 255);
}

void ImageProcessing::destroy_filtering_window() {
    destroyWindow(window_name);
}

void ImageProcessing::set_status(unsigned int new_status) {
    status = new_status;
}

void ImageProcessing::show_filtering_window(cv::Mat& frame) {
    cv::imshow(window_name, frame);
}

cv::Mat ImageProcessing::perform_filtering(cv::Mat &rec_frame) const {

    Mat frame, frameHSV, mask;
    flip(rec_frame, frame, 1);

    Scalar lowerLimit(hueMin, saturationMin, valueMin);
    Scalar upperLimit(hueMax, saturationMax, valueMax);

    cvtColor(frame, frame, COLOR_BGR2HSV);
    inRange(frame, lowerLimit, upperLimit, frame);

    return frame;
}

pair<Point2f, float> ImageProcessing::find_marker(Mat &frame) const {
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
        auto marker = min_element(circles.begin(), circles.end(), [](const auto &x, const auto &y) {return y.second < x.second; });
        return *marker;
    }
    return pair<Point2f, float>(Point2f(0.0, 0.0), 1.0);
}

bool ImageProcessing::end() const {
    return status == END_STATUS;
}

std::pair<Point2f, cv::Mat> ImageProcessing::run(cv::Mat& rec_frame) {
    if (status == ADJUST_STATUS)
        modify_color_search(rec_frame);
    else if (status == DISPLAY_STATUS) {
        cv::Mat frame, gameFrame;

        if (rec_frame.empty()) {
            return std::make_pair(Point2f(0, 0), rec_frame);
        }

        flip(rec_frame, frame, 1);
        frame.copyTo(gameFrame);
        pair<Point, float> marker = find_marker(frame);
        circle(gameFrame, marker.first, marker.second, Scalar(255, 0, 0), 1);

        int key = cv::waitKey(1);
        if (key == ESC_BUTTON || key == M_BUTTON)
            set_status(key == ESC_BUTTON ? END_STATUS : ADJUST_STATUS);
        return std::make_pair(marker.first, gameFrame);
    }

    return std::make_pair(Point2f(0, 0), rec_frame);
}

bool ImageProcessing::send_result() const {
    return status == DISPLAY_STATUS;
}
