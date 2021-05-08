//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include "synchronizer.hpp"
#include "CaptureVideo.hpp"

namespace {

    [[noreturn]] [[noreturn]] void synchronize(Synchronizer& synchronizer_process, Synchronizer& synchronizer_info) {

        Capture capture;
        capture.open();

        while (true) {
            cv::Mat frame = capture.capture();
            char *capture_info = (char *) "Capture info";
            synchronizer_info.send_data(capture_info, INFO_MESS_SIZE);
            synchronizer_process.send_data(frame.data, FRAME_SIZE);
        }
    }

    void sync_with_semaphores(char const* argv[]) {

        try {
            SharedMemorySemaphoresSynchronizer synchronizer_process(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_info(argv[5], argv[6], argv[7]);

            synchronize(synchronizer_process, synchronizer_info);

        } catch (std::runtime_error& e) {
            std::cerr << "Capture: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(const char* argv[]) {

        try {

            QueueSharedMemorySynchronizer synchronizer_process(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer synchronizer_info(argv[5], argv[6], argv[7]);

            synchronize(synchronizer_process, synchronizer_info);

        } catch (std::runtime_error& e) {
            std::cerr << "Capture process: " << e.what() << std::endl;
            exit(-1);
        }

    }
}


int main(int argc, char const* argv[]) {

    if (argc != 8) {
        std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores capture process synchronization." << std::endl;
        exit(-1);
    }

    unsigned int syn_mode = std::atoi(argv[1]);

    if (syn_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argv);
    else if (syn_mode == QUEUES_MEM_SYNC)
        sync_with_queues_and_mem(argv);
    else
        std::cerr << "WRONG SYNC METHOD" << std::endl;

    return 0;
}
