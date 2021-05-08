//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include "constants.hpp"
#include "synchronizer.hpp"
#include "ImageProcessing.hpp"
#include "Snake.hpp"


namespace {

    void sync_with_semaphores(int argc, char const* argv[]) {

        if (argc != 8) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores image process synchronization." << std::endl;
            exit(-1);
        }

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_game(argv[5], argv[6], argv[7]);
            ImageProcessing processor;

            //Test only
            Game game(cv::Point(FRAME_WIDTH, FRAME_HEIGHT));
            //Test only

            while (!processor.end()) {

                cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
                synchronizer_capture.receive_data(frame.data, FRAME_SIZE);
                auto result = processor.run(frame, game);

                if (result.first)
                    synchronizer_game.send_data(result.second.data, FRAME_SIZE);

            }

            synchronizer_capture.close_opened_resources();

        } catch (std::runtime_error& e) {
            std::cout << "Processing image: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues(int argc, char const* argv[]) {

        if (argc != 3) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for queue image process synchronization.";
            exit(-1);
        }

        try {
            RawQueuesSynchronizer synchronizer(argv[2]);

            while (true) {
                cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
                synchronizer.read_data(frame.data, FRAME_SIZE);
                cv::imshow("Process received", frame);
                cv::waitKey(1);
            }

        } catch (std::runtime_error& e) {
            std::cerr << "Capture process: " << e.what();
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(int argc, const char* argv[]) {

        if (argc != 8) {
            std::cerr << "Capture process: wrong number of parameters for queues and memory synchronization" << std::endl;
            exit(-1);
        }

        try {

            QueueSharedMemorySynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer synchronizer_game(argv[5], argv[6], argv[7]);
            ImageProcessing processor;

            //Test only
            Game game(cv::Point(FRAME_WIDTH, FRAME_HEIGHT));
            //Test only

            while(!processor.end()) {
                cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);
                synchronizer_capture.receive_data(frame.data, FRAME_SIZE);
                auto result = processor.run(frame, game);

                if (result.first)
                    synchronizer_game.send_data(result.second.data, FRAME_SIZE);

            }

            synchronizer_capture.close_opened_resources();

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

    unsigned int sync_mode = std::atoi(argv[1]);

    if (sync_mode == SEMAPHORES_SYNC)
        sync_with_semaphores(argc, argv);
    else if (sync_mode == QUEUES_RAW_SYNC)
        sync_with_queues(argc, argv);
    else if (sync_mode == QUEUES_MEM_SYNC)
        sync_with_queues_and_mem(argc, argv);
    else
        std::cerr << "WRONG SYNC METHOD" << std::endl;

    return 0;
}
