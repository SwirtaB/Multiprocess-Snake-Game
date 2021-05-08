//
// Created by radoslaw on 05.05.2021.
//
#include "synchronizer.hpp"

SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer() : sem_our(nullptr), sem_opp(nullptr),
filename(nullptr), block(nullptr) {}

SharedMemorySemaphoresSynchronizer::SharedMemorySemaphoresSynchronizer(char_t sem_our_n, char_t sem_opp_n, char_t block_n) {

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

    block = attach_shared_memory_block(filename, FRAME_SIZE);
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

    if (block != nullptr)
        if (!detach_shared_memory_block(block, FRAME_SIZE))
            std::cerr << "WARNING: Unable to detach shared memory block," << std::endl;

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

QueueSharedMemorySynchronizer::QueueSharedMemorySynchronizer(): queue_sent(-1), queue_done(-1), block(nullptr), filename(nullptr) {}

QueueSharedMemorySynchronizer::QueueSharedMemorySynchronizer(char_t queue_sent_name, char_t queue_done_name, char_t block_name) {

    filename = (char*) block_name;
    queue_sent = mq_open(queue_sent_name, O_RDWR);
    queue_done = mq_open(queue_done_name, O_RDWR);

    if (queue_sent == -1 || queue_done == -1)
        throw std::runtime_error("ERROR: Unable to open queue.");

    block = attach_shared_memory_block(filename, FRAME_SIZE);
    if (block == (char* ) nullptr) {
        mq_close(queue_sent);
        throw std::runtime_error("CRITICAL ERROR: Unable to link shared memory block.");
    }

}

void QueueSharedMemorySynchronizer::receive_data(void *data, unsigned int size) {

    char buffer[MESS_SIZE];
    if (mq_receive(queue_sent, buffer, MESS_SIZE, nullptr) == -1)
        std::cerr << "WARNING: Unable to receive message." << std::endl;

   if (strcmp(buffer, SENT_MESS) == 0) {

        memcpy(data, block, size);

        if (mq_send(queue_done, DONE_MESS, MESS_SIZE, 1) == -1)
            std::cerr << "WARNING: Unable to send done message.";

    } else
        std::cerr << "WARNING: Received unexpected message" << std::endl;

}

void QueueSharedMemorySynchronizer::send_data(void *data, unsigned int size) {

    memcpy(block, data, size);

    if (mq_send(queue_sent, SENT_MESS, MESS_SIZE, 1) == -1)
        std::cerr << "WARNING: Unable to send sent message." << std::endl;

    char buffer[MESS_SIZE];

    if (mq_receive(queue_done, buffer, MESS_SIZE, nullptr) == -1)
        std::cerr << "WARNING: Unable to receive done message" << std::endl;

    if (strcmp(buffer, DONE_MESS) != 0)
        std::cerr << "WARNING: Received unexpected message; expecting done." << std::endl;

}

void QueueSharedMemorySynchronizer::close_opened_resources() {
    mq_close(queue_sent);
    mq_close(queue_done);
    detach_shared_memory_block(block, FRAME_SIZE);
}
