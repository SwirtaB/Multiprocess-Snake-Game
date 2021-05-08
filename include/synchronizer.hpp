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

class Synchronizer {

public:

    virtual void send_data(void* data, unsigned int size) = 0;
    virtual void receive_data(void* data, unsigned int size) = 0;
    virtual void close_opened_resources() = 0;
};


class SharedMemorySemaphoresSynchronizer: public Synchronizer {

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

    void close_opened_resources() override;

    void send_data(void* data, unsigned int size) override;
    void receive_data(void* data, unsigned int size) override;

};

class QueueSharedMemorySynchronizer: public Synchronizer {

private:

    mqd_t queue_sent;
    mqd_t queue_done;

    void* block;
    char* filename;


public:
    QueueSharedMemorySynchronizer();
    QueueSharedMemorySynchronizer(char_t queue_sent_name, char_t queue_done_name, char_t block_name);

    void receive_data(void* data, unsigned int size) override;
    void send_data(void* data, unsigned int size) override;

    void close_opened_resources() override;
};

#endif //SCZR_SNAKE_SYNCHRONIZER_HPP
