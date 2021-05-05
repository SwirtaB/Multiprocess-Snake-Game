//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include "constants.hpp"
#include "synchronizer.hpp"
#include "ImageProcessing.hpp"


int main(int argc, char const* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for capturing camera image"<< std::endl;
        return -1;
    }

    Synchronizer synchronizer(std::atoi(argv[0]), argc, argv);
    ImageProcessing processor;

    while (true) {
        cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
        synchronizer.receive_data(frame.data, SHARED_MEMORY_BLOCK_SIZE);
        processor.run(frame);
    }

    synchronizer.free_resources();
    return 0;
}
