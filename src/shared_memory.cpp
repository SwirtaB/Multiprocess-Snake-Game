//
// Created by Radosław Radziukiewicz on 04.05.2021.
//
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


#include "shared_memory.hpp"


namespace {

    void* get_shared_block(char* filename, unsigned int size) {

        int shm_id = shm_open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (shm_id == -1)
            return nullptr;

        void* shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
        if (shm == MAP_FAILED)
            return nullptr;

        return shm;
    }

    int create_shared_block(char* filename, unsigned int size) {

        int shm_id = shm_open(filename, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
        if (shm_id == -1)
            return -1;
        if ((ftruncate(shm_id, size)) == -1)
            return -1;

        return shm_id;
    }

}

int create_shared_memory_block(char* filename, unsigned int size) {
    return create_shared_block(filename, size);
}


void* attach_shared_memory_block(char* filename, unsigned int size) {
    return get_shared_block(filename, size);
}

bool detach_shared_memory_block(void* block, unsigned int size) {
    return munmap(block, size) != -1;
}

bool destroy_shared_memory_block(char* filename) {
    return shm_unlink(filename) != -1;
}