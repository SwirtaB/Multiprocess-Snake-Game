//
// Created by radoslaw on 06.05.2021.
//
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "constants.hpp"
#include "synchronizer.hpp"
#include "Snake.hpp"

namespace {

    [[noreturn]] void synchronize(Synchronizer& synchronizer_process, Synchronizer& synchronizer_info) {
        Game game(cv::Point(FRAME_WIDTH, FRAME_HEIGHT));
        while (game.get_state() != QUIT) {
            float x_, y_;
            cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);

            std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
            synchronizer_process.receive_data(frame.data, FRAME_SIZE);
            memcpy(&x_, frame.data, sizeof(float));
            memcpy(&y_, frame.data+sizeof(float), sizeof(float));

            cv::Point marker = cv::Point(x_, y_);

            game.run(marker, frame);
            cv::imshow("Game window", frame);
            std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();

            //Konwertowanie time_point na char * by można było przesłać
            std::chrono::duration<double> captureTime = end - begin;
            auto x = std::chrono::duration_cast<std::chrono::microseconds>(captureTime);
            std::string game_info_str = std::to_string(x.count());
            char* game_info = (char *) game_info_str.c_str();
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
