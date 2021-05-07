//
// Created by radoslaw on 05.05.2021.
//

#ifndef SCZR_SNAKE_SYNCHRONIZER_HPP
#define SCZR_SNAKE_SYNCHRONIZER_HPP

#include <vector>
#include <semaphore.h>
#include <mqueue.h>
#include <iostream>
#include <cstring>
#include "shared_memory.hpp"
#include "constants.hpp"

using char_t = const char*;

class SharedMemorySemaphoresSynchronizer {

private:

    sem_t* sem_our;
    sem_t* sem_opp;

    char* filename;
    void* block;

    void enter_critical_section();
    void leave_critical_section();

public:
    SharedMemorySemaphoresSynchronizer();
    SharedMemorySemaphoresSynchronizer(char_t sem_our_n, char_t sem_opp_n, char_t block_n);
    ~SharedMemorySemaphoresSynchronizer();

    void close_opened_resources();

    void send_data(void* data, unsigned int size);
    void receive_data(void* data, unsigned int size);

};


class RawQueuesSynchronizer {

private:

    mqd_t queue_desc;

public:
    RawQueuesSynchronizer();
    explicit RawQueuesSynchronizer(char_t queue_name);

    void read_data(void* data, unsigned int size) const;
    void send_data(void* data, unsigned int size) const;

    void close_opened_resources() const;
};

class QueueSharedMemorySynchronizer {

private:

    mqd_t queue_sent;
    mqd_t queue_done;

    void* block;
    char* filename;


public:
    QueueSharedMemorySynchronizer();
    QueueSharedMemorySynchronizer(char_t queue_sent_name, char_t queue_done_name, char_t block_name);

    void receive_data(void* data, unsigned int size);
    void send_data(void* data, unsigned int size);

    void close_opened_resources();
};




#endif //SCZR_SNAKE_SYNCHRONIZER_HPP
