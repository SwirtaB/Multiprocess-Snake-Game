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

    void modify_color_search(cv::Mat frame);
    cv::Mat generate_mask(cv::Mat frame);
    void find_marker(cv::Mat frame);
    void run();

private:
    int hueMin, saturationMin, valueMin;
    int hueMax, saturationMax, valueMax;
};

#endif //SCZR_SNAKE_IMAGEPROCESSING_HPP
