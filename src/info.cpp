//
// Created by radoslaw on 07.05.2021.
//
#include <iostream>
#include "synchronizer.hpp"
#include "constants.hpp"

namespace {

    void sync_with_semaphores(int argc, const char* argv[]) {

        if (argc != 11) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores info process synchronization." << std::endl;
            exit(-1);
        }

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer process_synchronizer(argv[5], argv[6], argv[7]);
            SharedMemorySemaphoresSynchronizer game_synchronizer(argv[8], argv[9], argv[10]);

            while (true) {

                char buffer[INFO_MESS_SIZE];
                synchronizer_capture.receive_data(buffer, INFO_MESS_SIZE);
                std::cout << "From capture received: " << buffer << std::endl;
                process_synchronizer.receive_data(buffer, INFO_MESS_SIZE);
                std::cout << "From process received: " << buffer << std::endl;

                if (strcmp(buffer, "Image process adjust") != 0) {
                    game_synchronizer.receive_data(buffer, INFO_MESS_SIZE);
                    std::cout << "From game received: " << buffer << std::endl;
                }

            }

        } catch (std::runtime_error& e) {
            std::cerr << "Info process: " << e.what() << std::endl;
            exit(-1);
        }

    }


    void sync_with_queues(int argc, const char* argv[]) {
        //TODO implement
    }


    void sync_with_queues_and_mem(int argc, const char* argv[]) {

        if (argc != 11) {
            std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores info process synchronization." << std::endl;
            exit(-1);
        }

        try {

            QueueSharedMemorySynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer process_synchronizer(argv[5], argv[6], argv[7]);
            QueueSharedMemorySynchronizer game_synchronizer(argv[8], argv[9], argv[10]);

            while (true) {

                char buffer[INFO_MESS_SIZE];
                synchronizer_capture.receive_data(buffer, INFO_MESS_SIZE);
                std::cout << "From capture received: " << buffer << std::endl;
                process_synchronizer.receive_data(buffer, INFO_MESS_SIZE);
                std::cout << "From process received: " << buffer << std::endl;

                if (strcmp(buffer, "Image process adjust") != 0) {
                    game_synchronizer.receive_data(buffer, INFO_MESS_SIZE);
                    std::cout << "From game received: " << buffer << std::endl;
                }

            }

        } catch (std::runtime_error& e) {
            std::cerr << "Info process: " << e.what() << std::endl;
            exit(-1);
        }

    }

}







int main(int argc, const char* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for gathering info"<< std::endl;
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


    return 0;
}