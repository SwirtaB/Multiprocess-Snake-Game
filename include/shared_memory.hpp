//
// Created by Radoslaw Radziukiewicz on 04.05.2021.
//

#ifndef SCZR_SNAKE_SHARED_MEMORY_HPP
#define SCZR_SNAKE_SHARED_MEMORY_HPP


int create_shared_memory_block(char* filename, unsigned int size);
void* attach_shared_memory_block(char* filename, unsigned int size);
bool detach_shared_memory_block(void* block, unsigned int size);
bool destroy_shared_memory_block(char* filename);



#endif //SCZR_SNAKE_SHARED_MEMORY_HPP
