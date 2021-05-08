#include <iostream>
#include <vector>
#include <unistd.h>
#include <semaphore.h>
#include <mqueue.h>
#include <fcntl.h>
#include <wait.h>

#include "shared_memory.hpp"
#include "constants.hpp"


namespace {

    void end_processes(std::vector<std::pair<pid_t, char*>>& children) {
        for (auto& pair : children)
            if (kill(pair.first, 0) == 0) {
                if (kill(pair.first, SIGINT) != 0)
                    std::cerr << "WARNING: Unable to kill process " << pair.second << " with pid: " << pair.first << std::endl;
            } else {
                std::cout << "Process " << pair.second << " (pid: " << pair.first << ") died." << std::endl;
            }
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

    std::pair<pid_t, char*> start_process(char* argv[]) {

        pid_t result = fork();

        if (result < 0)
            throw std::runtime_error("Unable to create new process");

        if (result == 0)
            execv(argv[0], argv);

        return std::make_pair(result, argv[0]);
    }


    void start_processes_using_semaphores() {

        std::vector<std::pair<char*, unsigned int>> block_with_sizes =
                std::vector<std::pair<char*, unsigned int>>({{CAPTURE_PROCESS_BLOCK, FRAME_SIZE},
                                                             {PROCESS_GAME_BLOCK,    FRAME_SIZE},
                                                             {CAPTURE_INFO_BLOCK, INFO_MESS_SIZE},
                                                             {PROCESS_INFO_BLOCK, INFO_MESS_SIZE},
                                                             {GAME_INFO_BLOCK, INFO_MESS_SIZE}});

        std::vector<std::pair<char*, unsigned int>> semaphores_with_values =
                std::vector<std::pair<char*, unsigned int>> ({{CAPTURE_PROCESS_SEM, 0},
                                                              {PROCESS_CAPTURE_SEM, 1},
                                                              {PROCESS_GAME_SEM, 0},
                                                              {GAME_PROCESS_SEM, 1},
                                                              {CAPTURE_INFO_SEM, 0},
                                                              {INFO_CAPTURE_SEM, 1},
                                                              {PROCESS_INFO_SEM, 0},
                                                              {INFO_PROCESS_SEM, 1},
                                                              {GAME_INFO_SEM, 0},
                                                              {INFO_GAME_SEM, 1}});

        try {
            create_blocks(block_with_sizes);
            create_semaphores(semaphores_with_values);
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            destroy_blocks(block_with_sizes);
            destroy_semaphores(semaphores_with_values);
            exit(-1);
        }
        //TODO Fix starting parameters and each of processes sync_with_semaphores function.
        char* capture_param[] = {CAPTURE, SEM_SYNC, CAPTURE_PROCESS_SEM, PROCESS_CAPTURE_SEM, CAPTURE_PROCESS_BLOCK,
                                                    CAPTURE_INFO_SEM, INFO_CAPTURE_SEM, CAPTURE_INFO_BLOCK, nullptr};
        char* process_param[] = {PROCESS, SEM_SYNC, PROCESS_CAPTURE_SEM, CAPTURE_PROCESS_SEM, CAPTURE_PROCESS_BLOCK,
                                                    PROCESS_GAME_SEM, GAME_PROCESS_SEM, PROCESS_GAME_BLOCK,
                                                    PROCESS_INFO_SEM, INFO_PROCESS_SEM, PROCESS_INFO_BLOCK, nullptr};
        char* game_process[] = {GAME, SEM_SYNC, GAME_PROCESS_SEM, PROCESS_GAME_SEM, PROCESS_GAME_BLOCK,
                                                GAME_INFO_SEM, INFO_GAME_SEM, GAME_INFO_BLOCK, nullptr};
        char* info_process[] = {INFO, SEM_SYNC, INFO_CAPTURE_SEM, CAPTURE_INFO_SEM, CAPTURE_INFO_BLOCK,
                                                INFO_PROCESS_SEM, PROCESS_INFO_SEM, PROCESS_INFO_BLOCK,
                                                INFO_GAME_SEM, GAME_INFO_SEM, GAME_INFO_BLOCK, nullptr};
        std::vector<std::pair<pid_t, char*>> children;

        try {

            children.emplace_back(start_process(capture_param));
            children.emplace_back(start_process(process_param));
            children.emplace_back(start_process(game_process));
            children.emplace_back(start_process(info_process));

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

    void create_queues(std::vector<char*>& queues, unsigned int mess_size) {

        mq_attr attr {};
        attr.mq_flags = 0;
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = mess_size;
        attr.mq_curmsgs = 0;

        for (auto& q : queues) {
            mqd_t queue = mq_open(q, O_RDWR | O_CREAT, 0777, &attr);
            if (queue == -1)
                throw std::runtime_error("Unable to create message queue.");
        }
    }

    void destroy_queues(std::vector<char*>& queues) {
        for (auto& q : queues)
            mq_unlink(q);
    }


    void start_processes_using_queues() {

        std::vector<char* > queues = std::vector<char*> ({CAPTURE_PROCESS_QUEUE, PROCESS_GAME_QUEUE});

        try {
            create_queues(queues, FRAME_SIZE);
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            destroy_queues(queues);
            exit(-1);
        }

        char* capture_param[] = {CAPTURE, QUEUE_SYNC, CAPTURE_PROCESS_QUEUE, nullptr};
        char* process_param[] = {PROCESS, QUEUE_SYNC, CAPTURE_PROCESS_QUEUE, nullptr};

        std::vector<std::pair<pid_t, char*>> children;

        try {

            children.emplace_back(start_process(capture_param));
            children.emplace_back(start_process(process_param));

        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            end_processes(children);
            destroy_queues(queues);
            exit(-1);
        }

        wait(nullptr);

        end_processes(children);
        destroy_queues(queues);
    }

    void start_process_using_mem_and_queues() {

        std::vector<std::pair<char*, unsigned int>> block_with_sizes =
                std::vector<std::pair<char*, unsigned int>>({{CAPTURE_PROCESS_BLOCK, FRAME_SIZE},
                                                             {PROCESS_GAME_BLOCK,    FRAME_SIZE}});
        std::vector<char* > queues = std::vector<char*> ({CAPTURE_SEND_QUEUE, CAPTURE_RECV_QUEUE,
                                                          PROCESS_RECV_QUEUE, PROCESS_SEND_QUEUE});

        try {
            create_queues(queues, MESS_SIZE);
            create_blocks(block_with_sizes);
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            destroy_queues(queues);
            destroy_blocks(block_with_sizes);
            exit(-1);
        }

        char* capture_param[] = {CAPTURE, QUEUE_MEM_SYNC, CAPTURE_SEND_QUEUE, CAPTURE_RECV_QUEUE, CAPTURE_PROCESS_BLOCK, nullptr};
        char* process_param[] = {PROCESS, QUEUE_MEM_SYNC, CAPTURE_SEND_QUEUE, CAPTURE_RECV_QUEUE, CAPTURE_PROCESS_BLOCK,
                                 PROCESS_SEND_QUEUE, PROCESS_RECV_QUEUE, PROCESS_GAME_BLOCK, nullptr};
        char* game_param[] = {GAME, QUEUE_MEM_SYNC, PROCESS_SEND_QUEUE, PROCESS_RECV_QUEUE, PROCESS_GAME_BLOCK, nullptr};

        std::vector<std::pair<pid_t, char*>> children;

        try {

            children.emplace_back(start_process(capture_param));
            children.emplace_back(start_process(process_param));
            children.emplace_back(start_process(game_param));

        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            end_processes(children);
            destroy_queues(queues);
            destroy_blocks(block_with_sizes);
            exit(-1);
        }

        wait(nullptr);

        end_processes(children);
        destroy_queues(queues);
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
    else if (sync_mode == QUEUES_MEM_SYNC)
        start_process_using_mem_and_queues();
    else if (sync_mode == QUEUES_RAW_SYNC)
        start_processes_using_queues();
    else
        std::cerr << "Wrong synchronization mode." << std::endl;


    return 0;
}