//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include "synchronizer.hpp"
#include "CaptureVideo.hpp"

#include <opencv4/opencv2/imgcodecs.hpp>




int main(int argc, char const* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for capturing camera image"<< std::endl;
        return -1;
    }

    Synchronizer synchronizer (std::atoi(argv[0]), argc, argv);
    Capture capture;
    capture.open();

    for(int i = 0; i < 50; i++) {
        cv::Mat frame = capture.capture();
        synchronizer.send_data(frame.data, SHARED_MEMORY_BLOCK_SIZE);
    }

    synchronizer.free_resources();

    return 0;
}
