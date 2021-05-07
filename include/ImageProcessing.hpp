//
// Created by swirta on 27.04.2021.
//

#ifndef SCZR_SNAKE_IMAGEPROCESSING_HPP
#define SCZR_SNAKE_IMAGEPROCESSING_HPP

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

class ImageProcessing{
public:
    void modify_color_search();
    std::pair<cv::Point2f, float> find_marker(cv::Mat &frame);
    void run();

private:
    cv::VideoCapture camera;
    int hueMin = 90, saturationMin = 100, valueMin = 0;
    int hueMax = 120, saturationMax = 255, valueMax = 255;
};

#endif //SCZR_SNAKE_IMAGEPROCESSING_HPP
