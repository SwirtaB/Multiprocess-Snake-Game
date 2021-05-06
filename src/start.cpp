#include <iostream>
#include <vector>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>

#include "shared_memory.hpp"
#include "constants.hpp"

/** Semaphores synchronization constants */
char* SEM_SYNC = (char*) "0";

char* PROCESS = (char*) "process";
char* CAPTURE = (char*) "capture";
char* GAME = (char*) "game";

char* CAPTURE_PROCESS_SEM = (char*) "/capture_process_sem";
char* PROCESS_CAPTURE_SEM = (char*) "/process_capture_sem";

char* PROCESS_GAME_SEM = (char*) "/process_game_sem";
char* GAME_PROCESS_SEM = (char*) "/game_process_sem";

char* CAPTURE_PROCESS_BLOCK = (char*) "/capture_process_shm";
char* PROCESS_GAME_BLOCK = (char*) "/process_game_shm";




namespace {

    void end_processes(std::vector<pid_t>& children) {
        for (auto& pid : children)
            if (kill(pid, 0) == 0)
                if (kill(pid, SIGINT) != 0)
                    std::cerr << "WARNING: Unable to kill process with pid: " << pid << std::endl;
    }

    void create_blocks(std::vector<std::pair<char*, unsigned int>>& blocks_with_sizes) {

        for (auto& pair : blocks_with_sizes) {
            int block_id = create_shared_memory_block(pair.first, pair.second);
            if (block_id == -1)
                throw std::runtime_error("CRITICAL ERROR: Unable to create shared memory block");
        }

    }

    void destroy_blocks(std::vector<std::pair<char*, unsigned int>>& blocks_with_sizes) {
        for (auto& pair : blocks_with_sizes)
            destroy_shared_memory_block(pair.first);
    }

    void create_semaphores(std::vector<std::pair<char*, unsigned int>>& semaphores_with_val) {
        for (auto& pair : semaphores_with_val) {
            sem_t* sem = sem_open(pair.first, O_CREAT, 0660, pair.second);
            if (sem == SEM_FAILED)
                throw std::runtime_error("CRITICAL ERROR: Unable to create process semaphore");
        }
    }

    void destroy_semaphores(std::vector<std::pair<char*, unsigned int>>& semaphores_with_val) {
        for (auto& pair : semaphores_with_val)
            sem_unlink(pair.first);
    }

    pid_t start_process(char* argv[]) {

        pid_t result = fork();

        if (result < 0)
            throw std::runtime_error("Unable to create new process");

        if (result == 0)
            execv(argv[0], argv);

        return result;
    }


    void start_processes_using_semaphores() {

        std::vector<std::pair<char*, unsigned int>> block_with_sizes =
                std::vector<std::pair<char*, unsigned int>>({{CAPTURE_PROCESS_BLOCK, SHARED_MEMORY_FRAME_BLOCK_SIZE},
                                                             {PROCESS_GAME_BLOCK, SHARED_MEMORY_FRAME_BLOCK_SIZE}});

        std::vector<std::pair<char*, unsigned int>> semaphores_with_values =
                std::vector<std::pair<char*, unsigned int>> ({{CAPTURE_PROCESS_SEM, 0},
                                                              {PROCESS_CAPTURE_SEM, 1},
                                                              {PROCESS_GAME_SEM, 0},
                                                              {GAME_PROCESS_SEM, 1}});

        try {
            create_blocks(block_with_sizes);
            create_semaphores(semaphores_with_values);
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            destroy_blocks(block_with_sizes);
            destroy_semaphores(semaphores_with_values);
            exit(-1);
        }

        char* capture_param[] = {CAPTURE, SEM_SYNC, CAPTURE_PROCESS_SEM, PROCESS_CAPTURE_SEM, CAPTURE_PROCESS_BLOCK, nullptr};
        char* process_param[] = {PROCESS, SEM_SYNC, PROCESS_CAPTURE_SEM, CAPTURE_PROCESS_SEM, CAPTURE_PROCESS_BLOCK,
                                 PROCESS_GAME_SEM, GAME_PROCESS_SEM, PROCESS_GAME_BLOCK, nullptr};
        char* game_process[] = {GAME, SEM_SYNC, GAME_PROCESS_SEM, PROCESS_GAME_SEM, PROCESS_GAME_BLOCK, nullptr};

        std::vector<pid_t> children;

        try {

            children.emplace_back(start_process(capture_param));
            children.emplace_back(start_process(process_param));
            children.emplace_back(start_process(game_process));

        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            end_processes(children);
            destroy_semaphores(semaphores_with_values);
            destroy_blocks(block_with_sizes);
            exit(-1);
        }

        wait(nullptr);

        end_processes(children);
        destroy_semaphores(semaphores_with_values);
        destroy_blocks(block_with_sizes);
    }













}



int main(int argc, char const* argv[]){

    if (argc != 2) {
        std::cerr << "Supply synchronization mode" << std::endl;
        return -1;
    }

    unsigned int sync_mode = std::atoi(argv[1]);

    if (sync_mode == SEMAPHORES_SYNC)
        start_processes_using_semaphores();
    else {
        std::cerr << "Deprecated synchronization mode" << std::endl;
        return -1;
    }


    return 0;
}