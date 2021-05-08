//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include "constants.hpp"
#include "synchronizer.hpp"
#include "ImageProcessing.hpp"
#include "Snake.hpp"


namespace {

    void synchronize(Synchronizer& synchronizer_capture, Synchronizer& synchronizer_game, Synchronizer& synchronizer_info) {

        ImageProcessing processor;

        while (!processor.end()) {

            cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
            synchronizer_capture.receive_data(frame.data, FRAME_SIZE);

            std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
            auto result = processor.run(frame);
            std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();

            //Konwertowanie time_point na char * by można było przesłać
            std::chrono::duration<double> captureTime = end - begin;
            auto x = std::chrono::duration_cast<std::chrono::microseconds>(captureTime);
            std::string process_info_str = result.first != cv::Point2f(0, 0) ? std::to_string(x.count()) : "P" + std::to_string(x.count());
            char* process_info = (char *) process_info_str.c_str();
            //char* process_info = result.first ? (char*) "Process image info" : (char*) "Image process adjust";
            synchronizer_info.send_data((void*)process_info, INFO_MESS_SIZE);

            if (processor.send_result()) {
                memcpy(result.second.data, &result.first.x, sizeof(float));
                memcpy(result.second.data+sizeof(float), &result.first.y, sizeof(float));
                synchronizer_game.send_data(result.second.data, FRAME_SIZE);
            }
        }
    }

    void sync_with_semaphores(char const* argv[]) {

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_game(argv[5], argv[6], argv[7]);
            SharedMemorySemaphoresSynchronizer synchronizer_info(argv[8], argv[9], argv[10]);

            synchronize(synchronizer_capture, synchronizer_game, synchronizer_info);

        } catch (std::runtime_error& e) {
            std::cout << "Processing image: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(const char* argv[]) {

        try {

            QueueSharedMemorySynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer synchronizer_game(argv[5], argv[6], argv[7]);
            QueueSharedMemorySynchronizer synchronizer_info(argv[8], argv[9], argv[10]);

            synchronize(synchronizer_capture, synchronizer_game, synchronizer_info);

        } catch (std::runtime_error& e) {
            std::cerr << "Capture process: " << e.what() << std::endl;
            exit(-1);
        }

    }



}




int main(int argc, char const* argv[]) {

    if (argc != 11) {
        std::cerr << "Capture process: wrong number of parameters for queues and memory synchronization" << std::endl;
        exit(-1);
    }

    unsigned int sync_mode = std::atoi(argv[1]);

    if (sync_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argv);
    else if (sync_mode == QUEUES_MEM_SYNC)
        sync_with_queues_and_mem(argv);
    else
        std::cerr << "WRONG SYNC METHOD" << std::endl;

    return 0;
}
