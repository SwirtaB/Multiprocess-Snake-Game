//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include "constants.hpp"
#include "synchronizer.hpp"
#include "ImageProcessing.hpp"


namespace {

    void sync_with_semaphores(int argc, char const* argv[]) {

        if (argc != 8) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for capture process synchronization." << std::endl;
            exit(-1);
        }

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_game(argv[5], argv[6], argv[7]);
            ImageProcessing processor;

            while (!processor.end()) {

                cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
                synchronizer_capture.receive_data(frame.data, SHARED_MEMORY_FRAME_BLOCK_SIZE);
                auto result = processor.run(frame);

                if (result.first)
                    synchronizer_game.send_data(result.second.data, SHARED_MEMORY_FRAME_BLOCK_SIZE);

            }

            synchronizer_capture.close_opened_resources();

        } catch (std::runtime_error& e) {
            std::cout << "Processing image: " << e.what() << std::endl;
            exit(-1);
        }

    }



}




int main(int argc, char const* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for capturing camera image"<< std::endl;
        return -1;
    }

    unsigned int sync_mode = std::atoi(argv[0]);

    if (sync_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argc, argv);
    else
        std::cerr << "WRONG SYN METHOD" << std::endl;

    return 0;
}
