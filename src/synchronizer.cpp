//
// Created by radoslaw on 05.05.2021.
//
#include "synchronizer.hpp"

Synchronizer::SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer() : sem_our(nullptr), sem_opp(nullptr),
filename(nullptr), block(nullptr) {}

Synchronizer::SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer(unsigned int argc,
                                                                                     const char **argv) {

    if (argc != 4)
        throw std::runtime_error("CRITICAL ERROR: Wrong number of parameters while semaphores synchronization.");

    filename = (char*) argv[3];
    sem_our = nullptr;
    sem_opp = nullptr;
    block = nullptr;

    sem_our = sem_open(argv[1], 0);
    if (sem_our == SEM_FAILED)
        throw std::runtime_error("CRITICAL ERROR: Unable to open first semaphore.");

    sem_opp = sem_open(argv[2], 0);
    if (sem_opp == SEM_FAILED) {
        sem_close(sem_our);
        throw std::runtime_error("CRITICAL ERROR: Unable to open second semaphore.");
    }

    block = attach_shared_memory_block((char *) argv[3], SHARED_MEMORY_BLOCK_SIZE);
    if (block == (char* ) nullptr) {
        sem_close(sem_our);
        sem_close(sem_opp);
        throw std::runtime_error("CRITICAL ERROR: Unable to link shared memory block.");
    }

}

void Synchronizer::SharedMemorySemaphoresSynchronizer::close_opened_resources() {
    if (sem_our != nullptr)
        if (sem_close(sem_our) != 0)
            std::cerr << "WARNING: Unable to close first semaphore." << std::endl;

    if (sem_opp != nullptr)
        if (sem_close(sem_opp) != 0)
            std::cerr << "WARNING: Unable to close first semaphore." << std::endl;

    if (block != nullptr) {
        if (!detach_shared_memory_block(block, SHARED_MEMORY_BLOCK_SIZE))
            std::cerr << "WARNING: Unable to detach shared memory block," << std::endl;
        if (!destroy_shared_memory_block(filename))
            std::cerr << "WARNING: Unable to unlink memory block." << std::endl;
    }

    sem_our = nullptr;
    sem_opp = nullptr;
    block = nullptr;
}

void Synchronizer::SharedMemorySemaphoresSynchronizer::enter_critical_section() {
    sem_wait(sem_opp);
}

void Synchronizer::SharedMemorySemaphoresSynchronizer::leave_critical_section() {
    sem_post(sem_our);
}

void Synchronizer::SharedMemorySemaphoresSynchronizer::send_data(void *data, unsigned int size) {
    enter_critical_section();
    memcpy(block, data, size);
    leave_critical_section();
}

void Synchronizer::SharedMemorySemaphoresSynchronizer::receive_data(void *data, unsigned int size) {
    enter_critical_section();
    memcpy(data, block, size);
    leave_critical_section();
}

Synchronizer::Synchronizer(unsigned int mode, unsigned int argc, const char **argv) : mode(mode) {

    if (mode == SEMAPHORES_SYNC) {
        sem_mem_sync = SharedMemorySemaphoresSynchronizer(argc, argv);
    } else {
        throw std::runtime_error("ERROR: Wrong synchronization mode");
    }
}

void Synchronizer::free_resources() {
    sem_mem_sync.close_opened_resources();
}

void Synchronizer::send_data(void *data, unsigned int size) {
    if (mode == SEMAPHORES_SYNC)
        sem_mem_sync.send_data(data, size);
}

void Synchronizer::receive_data(void *data, unsigned int size) {
    if (mode == SEMAPHORES_SYNC)
        sem_mem_sync.receive_data(data, size);
}

Synchronizer::~Synchronizer() {
    if (mode == SEMAPHORES_SYNC)
        sem_mem_sync.close_opened_resources();
}
