//
// Created by radoslaw on 05.05.2021.
//

#ifndef SCZR_SNAKE_SYNCHRONIZER_HPP
#define SCZR_SNAKE_SYNCHRONIZER_HPP

#include <vector>
#include <semaphore.h>
#include <iostream>
#include <cstring>
#include "shared_memory.hpp"
#include "constants.hpp"

class SharedMemorySemaphoresSynchronizer {

private:

    using char_t = const char*;

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




#endif //SCZR_SNAKE_SYNCHRONIZER_HPP
