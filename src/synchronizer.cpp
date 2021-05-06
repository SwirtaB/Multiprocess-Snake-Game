//
// Created by radoslaw on 05.05.2021.
//
#include "synchronizer.hpp"

SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer() : sem_our(nullptr), sem_opp(nullptr),
filename(nullptr), block(nullptr) {}

SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer(
        SharedMemorySemaphoresSynchronizer::char_t sem_our_n, SharedMemorySemaphoresSynchronizer::char_t sem_opp_n,
        SharedMemorySemaphoresSynchronizer::char_t block_n) {

    filename = (char*) block_n;
    sem_our = nullptr;
    sem_opp = nullptr;
    block = nullptr;

    sem_our = sem_open(sem_our_n, 0);
    if (sem_our == SEM_FAILED)
        throw std::runtime_error("CRITICAL ERROR: Unable to open first semaphore.");

    sem_opp = sem_open(sem_opp_n, 0);
    if (sem_opp == SEM_FAILED) {
        sem_close(sem_our);
        throw std::runtime_error("CRITICAL ERROR: Unable to open second semaphore.");
    }

    block = attach_shared_memory_block(filename, SHARED_MEMORY_FRAME_BLOCK_SIZE);
    if (block == (char* ) nullptr) {
        sem_close(sem_our);
        sem_close(sem_opp);
        throw std::runtime_error("CRITICAL ERROR: Unable to link shared memory block.");
    }
}

void SharedMemorySemaphoresSynchronizer::close_opened_resources() {
    if (sem_our != nullptr)
        if (sem_close(sem_our) != 0)
            std::cerr << "WARNING: Unable to close first semaphore." << std::endl;

    if (sem_opp != nullptr)
        if (sem_close(sem_opp) != 0)
            std::cerr << "WARNING: Unable to close first semaphore." << std::endl;

    if (block != nullptr) {
        if (!detach_shared_memory_block(block, SHARED_MEMORY_FRAME_BLOCK_SIZE))
            std::cerr << "WARNING: Unable to detach shared memory block," << std::endl;
        if (!destroy_shared_memory_block(filename))
            std::cerr << "WARNING: Unable to unlink memory block." << std::endl;
    }

    sem_our = nullptr;
    sem_opp = nullptr;
    block = nullptr;
}

void SharedMemorySemaphoresSynchronizer::enter_critical_section() {
    sem_wait(sem_opp);
}

void SharedMemorySemaphoresSynchronizer::leave_critical_section() {
    sem_post(sem_our);
}

void SharedMemorySemaphoresSynchronizer::send_data(void *data, unsigned int size) {
    enter_critical_section();
    memcpy(block, data, size);
    leave_critical_section();
}

void SharedMemorySemaphoresSynchronizer::receive_data(void *data, unsigned int size) {
    enter_critical_section();
    memcpy(data, block, size);
    leave_critical_section();
}

SharedMemorySemaphoresSynchronizer::~SharedMemorySemaphoresSynchronizer() {
    close_opened_resources();
}

