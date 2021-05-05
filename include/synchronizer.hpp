//
// Created by radoslaw on 05.05.2021.
//

#ifndef SCZR_SNAKE_SYNCHRONIZER_HPP
#define SCZR_SNAKE_SYNCHRONIZER_HPP

#include <vector>
#include <semaphore.h>
#include <constants.hpp>
#include <iostream>
#include <cstring>
#include "shared_memory.hpp"

class Synchronizer {

private:
    class SharedMemorySemaphoresSynchronizer {

    private:
        sem_t* sem_our;
        sem_t* sem_opp;

        char* filename;
        void* block;

    public:
        SharedMemorySemaphoresSynchronizer();
        SharedMemorySemaphoresSynchronizer(unsigned int argc, char const* argv[]);

        void close_opened_resources();

        void enter_critical_section();
        void leave_critical_section();
        void send_data(void* data, unsigned int size);
        void receive_data(void* data, unsigned int size);
    };

    unsigned int mode;

    SharedMemorySemaphoresSynchronizer sem_mem_sync;



public:
    explicit Synchronizer(unsigned int mode, unsigned int argc, char const* argv[]);
    ~Synchronizer();

    void free_resources();
    void send_data(void* data, unsigned int size);
    void receive_data(void* data, unsigned int size);
};


#endif //SCZR_SNAKE_SYNCHRONIZER_HPP
