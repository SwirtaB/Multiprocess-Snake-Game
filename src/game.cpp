//
// Created by radoslaw on 06.05.2021.
//
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "constants.hpp"
#include "synchronizer.hpp"

namespace {

    [[noreturn]] void synchronize(Synchronizer& synchronizer_process, Synchronizer& synchronizer_info) {

        while (true) {
            float x_, y_;
            cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);

            synchronizer_process.receive_data(frame.data, FRAME_SIZE);
            memcpy(&x_, frame.data, sizeof(float));
            memcpy(&y_, frame.data+sizeof(float), sizeof(float));

            char* game_info = (char*)"Game info";
            synchronizer_info.send_data((void*) game_info, INFO_MESS_SIZE);
        }

    }

    void sync_with_semaphores(char const* argv[]) {

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_process(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_info(argv[5], argv[6], argv[7]);

            synchronize(synchronizer_process, synchronizer_info);

        } catch (std::runtime_error& e) {
            std::cout << "Game: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(const char* argv[]) {

        try {

            QueueSharedMemorySynchronizer synchronizer_process(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer synchronizer_info(argv[5], argv[6], argv[7]);

            synchronize(synchronizer_process, synchronizer_info);
        } catch (std::runtime_error& e) {
            std::cerr << "Game process: " << e.what() << std::endl;
            exit(-1);
        }

    }
}


int main(int argc, const char** argv) {

    if (argc != 8) {
        std::cerr << "CRITICAL ERROR: Not enough parameters for capture process synchronization." << std::endl;
        exit(-1);
    }

    unsigned int sync_mode = std::atoi(argv[1]);

    if (sync_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argv);
    else if (sync_mode == QUEUES_MEM_SYNC)
        sync_with_queues_and_mem(argv);


    return 0;
}
