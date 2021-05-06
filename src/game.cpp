//
// Created by radoslaw on 06.05.2021.
//
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include "constants.hpp"
#include "synchronizer.hpp"


namespace {

    void sync_with_semaphores(int argc, char const* argv[]) {

        if (argc != 5) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for capture process synchronization." << std::endl;
            exit(-1);
        }

        try {

            SharedMemorySemaphoresSynchronizer synchronizer(argv[2], argv[3], argv[4]);

            while (true) {
                cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
                synchronizer.receive_data(frame.data, SHARED_MEMORY_FRAME_BLOCK_SIZE);
                cv::imshow("Game received", frame);
                cv::waitKey(1);
            }


        } catch (std::runtime_error& e) {
            std::cout << "Game: " << e.what() << std::endl;
            exit(-1);
        }

    }



}





int main(int argc, const char** argv) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for game management."<< std::endl;
        return -1;
    }

    unsigned int sync_mode = std::atoi(argv[0]);

    if (sync_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argc, argv);


    return 0;
}
