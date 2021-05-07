//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include "synchronizer.hpp"
#include "CaptureVideo.hpp"

namespace {

    void sync_with_semaphores(int argc, char const* argv[]) {

        if (argc != 5) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores capture process synchronization." << std::endl;
            exit(-1);
        }

        try {
            SharedMemorySemaphoresSynchronizer synchronizer(argv[2], argv[3], argv[4]);

            Capture capture;
            capture.open();

            while(true) {
                cv::Mat frame = capture.capture();
                synchronizer.send_data(frame.data, FRAME_SIZE);
            }

            synchronizer.close_opened_resources();

        } catch (std::runtime_error& e) {
            std::cerr << "Capture: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues(int argc, char const* argv[]) {

        if (argc != 3) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for queue capture process synchronization.";
            exit(-1);
        }

        try {

            RawQueuesSynchronizer synchronizer(argv[2]);
            Capture capture;
            capture.open();

            for (int i = 0; i < 100; i++) {
                cv::Mat frame = capture.capture();
                synchronizer.send_data(frame.data, FRAME_SIZE);
            }

            synchronizer.close_opened_resources();

        } catch (std::runtime_error& e) {
            std::cerr << "Capture process: " << e.what();
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(int argc, const char* argv[]) {

        if (argc != 5) {
            std::cerr << "Capture process: wrong number of parameters for queues and memory synchronization" << std::endl;
            exit(-1);
        }

        try {

            QueueSharedMemorySynchronizer synchronizer(argv[2], argv[3], argv[4]);
            Capture capture;
            capture.open();

            for (int i = 0; i < 200; i++) {
                cv::Mat frame = capture.capture();
                synchronizer.send_data(frame.data, FRAME_SIZE);
            }

            synchronizer.close_opened_resources();

        } catch (std::runtime_error& e) {
            std::cerr << "Capture process: " << e.what() << std::endl;
            exit(-1);
        }

    }


}





int main(int argc, char const* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for capturing camera image"<< std::endl;
        return -1;
    }

    unsigned int syn_mode = std::atoi(argv[1]);

    if (syn_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argc, argv);
    else if (syn_mode == QUEUES_RAW_SYNC)
        sync_with_queues(argc, argv);
    else if (syn_mode == QUEUES_MEM_SYNC)
        sync_with_queues_and_mem(argc, argv);
    else
        std::cerr << "WRONG SYNC METHOD" << std::endl;

    return 0;
}
