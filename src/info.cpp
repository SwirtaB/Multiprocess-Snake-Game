//
// Created by radoslaw on 07.05.2021.
//
#include <chrono>
#include <iostream>
#include "synchronizer.hpp"
#include "constants.hpp"
#include "Log.hpp"

namespace {

    [[noreturn]] void synchronize(Synchronizer& synchronizer_capture, Synchronizer& synchronizer_process, Synchronizer& synchronizer_game) {
        std::string gameOff = "0";
        Log log;
        log.open_file();
        while (true) {

//            char buffer[INFO_MESS_SIZE];
//            synchronizer_capture.receive_data(buffer, INFO_MESS_SIZE);
//            std::cout << "From capture received: " << buffer << std::endl;
//            synchronizer_process.receive_data(buffer, INFO_MESS_SIZE);
//            std::cout << "From process received: " << buffer << std::endl;

//            if (strcmp(buffer, "Image process adjust") != 0) {
//                synchronizer_game.receive_data(buffer, INFO_MESS_SIZE);
//                std::cout << "From game received: " << buffer << std::endl;
//            }

            char captureBuffer[INFO_MESS_SIZE];
            char processBuffer[INFO_MESS_SIZE];
            char gameBuffer[INFO_MESS_SIZE];
            synchronizer_capture.receive_data(captureBuffer, INFO_MESS_SIZE);
            synchronizer_process.receive_data(processBuffer, INFO_MESS_SIZE);
            if(processBuffer[0] != 'P'){
                synchronizer_game.receive_data(gameBuffer, INFO_MESS_SIZE);
                log.convert_and_write(captureBuffer, processBuffer, gameBuffer);
            }
            else {
                processBuffer[0] = ' ';
                log.convert_and_write(captureBuffer, processBuffer, gameOff.c_str());
            }
        }
    }

    void sync_with_semaphores(const char* argv[]) {

        try {

            SharedMemorySemaphoresSynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            SharedMemorySemaphoresSynchronizer synchronizer_process(argv[5], argv[6], argv[7]);
            SharedMemorySemaphoresSynchronizer synchronizer_game(argv[8], argv[9], argv[10]);

            synchronize(synchronizer_capture, synchronizer_process, synchronizer_game);

        } catch (std::runtime_error& e) {
            std::cerr << "Info process: " << e.what() << std::endl;
            exit(-1);
        }

    }

    void sync_with_queues_and_mem(const char* argv[]) {

        try {

            QueueSharedMemorySynchronizer synchronizer_capture(argv[2], argv[3], argv[4]);
            QueueSharedMemorySynchronizer synchronizer_process(argv[5], argv[6], argv[7]);
            QueueSharedMemorySynchronizer synchronizer_game(argv[8], argv[9], argv[10]);

            synchronize(synchronizer_capture, synchronizer_process, synchronizer_game);

        } catch (std::runtime_error& e) {
            std::cerr << "Info process: " << e.what() << std::endl;
            exit(-1);
        }

    }

}



int main(int argc, const char* argv[]) {

    if (argc != 11) {
        std::cerr << "CRITICAL ERROR: Not enough parameters for semaphores info process synchronization." << std::endl;
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