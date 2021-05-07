//
// Created by swirta on 27.04.2021.
//

#ifndef SCZR_SNAKE_IMAGEPROCESSING_HPP
#define SCZR_SNAKE_IMAGEPROCESSING_HPP

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include "Snake.hpp"

const unsigned int M_BUTTON = 109;
const unsigned int ESC_BUTTON = 27;

const unsigned int DISPLAY_STATUS = 0;
const unsigned int ADJUST_STATUS = 1;
const unsigned int END_STATUS = 2;

class ImageProcessing{

public:
    void modify_color_search(cv::Mat& rec_frame);
    std::pair<cv::Point2f, float> find_marker(cv::Mat &frame) const;
    std::pair<bool, cv::Mat> run(cv::Mat& rec_frame, Game &game);

    bool end() const;

private:

    const std::string window_name = "Edit marker color";

    int hueMin = 90, saturationMin = 100, valueMin = 0;
    int hueMax = 120, saturationMax = 255, valueMax = 255;

    unsigned int status = ADJUST_STATUS;

    void create_filtering_window();
    void show_filtering_window(cv::Mat& frame);
    void destroy_filtering_window();

    void set_status(unsigned int new_status);

    cv::Mat perform_filtering(cv::Mat& rec_frame) const;
};

#endif //SCZR_SNAKE_IMAGEPROCESSING_HPP
