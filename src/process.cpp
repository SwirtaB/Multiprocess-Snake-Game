//
// Created by radoslaw on 04.05.2021.
//
#include <iostream>
#include <cstring>
#include <semaphore.h>
#include "constants.hpp"
#include "shared_memory.hpp"
#include "ImageProcessing.hpp"

namespace {
    void communicate(sem_t* capture_sem, sem_t* process_sem, void* block) {

        ImageProcessing processor;


        while (true) {

            sem_wait(capture_sem);
            cv::Mat frame(720, 1280, CV_8UC3, block);
            //memcpy(frame.data, block, SHARED_MEMORY_BLOCK_SIZE);
            processor.run(frame);
            sem_post(process_sem);

        }
    }
}





int sync_with_semaphores(int argc, char const* argv[]) {

    if (argc != 4) {
        std::cerr << "CRITICAL ERROR: Wrong number of parameters while semaphores synchronization for process" << std::endl;
        return -1;
    }

    sem_t* capture_sem = sem_open(argv[1], 0);
    if (capture_sem == SEM_FAILED) {
        std::cerr << "CRITICAL ERROR: Unable to open capture semaphore for capture process" << std::endl;
        return -1;
    }
    sem_t* process_sem = sem_open(argv[2], 0);
    if (process_sem == SEM_FAILED) {
        sem_close(capture_sem);
        std::cerr << "CRITICAL ERROR: Unable to open process semaphore for capture process" << std::endl;
        return -1;
    }
    void* block = attach_shared_memory_block((char *) argv[3], SHARED_MEMORY_BLOCK_SIZE);
    if (block == (char* ) nullptr) {
        sem_close(capture_sem);
        sem_close(process_sem);
        std::cerr << "CRITICAL ERROR: Unable to link shared memory block for capture process" << std::endl;
        return -1;
    }

    communicate(capture_sem, process_sem, block);

    sem_close(process_sem);
    sem_close(capture_sem);
    detach_shared_memory_block(block, SHARED_MEMORY_BLOCK_SIZE);
    destroy_shared_memory_block((char *)argv[3]);

    return 0;
}










int main(int argc, char const* argv[]) {

    if (argc < 2) {
        std::cerr << "CRITICAL ERROR: Not enough parameters to run process responsible for capturing camera image"<< std::endl;
        return -1;
    }

    unsigned int sync_method = std::atoi(argv[0]);

    if (sync_method == SEMAPHORES_SYNC) {
        return sync_with_semaphores(argc, argv);

    } else {
        std::cerr << "DEPRECATED: Use of unknown synchronization method" << std::endl;
        return -1;
    }
}
