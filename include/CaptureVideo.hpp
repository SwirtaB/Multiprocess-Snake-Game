//
// Created by swirta on 02.05.2021.
//
#ifndef SCZR_SNAKE_CAPTUREVIDEO_HPP
#define SCZR_SNAKE_CAPTUREVIDEO_HPP

#include <opencv4/opencv2/videoio.hpp>

class Capture{
public:
    cv::Mat capture();
    void open();
private:
    cv::VideoCapture camera;
    cv::Mat rawFrame;
};

#endif //SCZR_SNAKE_CAPTUREVIDEO_HPP
