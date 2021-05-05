#include <iostream>
#include <vector>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>

//#include "../include/ImageProcessing.hpp"
//#include "../include/CaptureVideo.hpp"
#include "shared_memory.hpp"
#include "constants.hpp"

//using namespace cv;
using namespace std;

char* CAPTURE_SEM = (char*) "/capture_sem";
char* PROCESS_SEM = (char*) "/process_sem";

namespace {

    void end_processes(std::vector<pid_t>& children) {
        for (auto& pid : children)
            if (kill(pid, 0) == 0)
                if (kill(pid, SIGINT) != 0)
                    std::cerr << "WARNING: Unable to kill process with pid: " << pid << std::endl;
    }


    void start_processes_using_semaphores() {

        std::vector<char*> filenames = std::vector<char*>({(char*) "/shared_mem"});
        int block_id = create_shared_memory_block(filenames[0], SHARED_MEMORY_BLOCK_SIZE);
        if (block_id == -1) {
            std::cerr << "CRITICAL ERROR: Unable to create shared memory block" << std::endl;
            exit(-1);
        }

        sem_t* sem_capture = sem_open(CAPTURE_SEM, O_CREAT, 0660, 0);
        if (sem_capture == SEM_FAILED) {
            std::cerr << "CRITICAL ERROR: Unable to create capture semaphore" << std::endl;
            destroy_shared_memory_block(filenames[0]);
            exit(-1);
        }

        sem_t* sem_process = sem_open(PROCESS_SEM, O_CREAT, 0660, 1);
        if (sem_process == SEM_FAILED) {
            std::cerr << "CRITICAL ERROR: Unable to create process semaphore" << std::endl;
            destroy_shared_memory_block(filenames[0]);
            sem_destroy(sem_capture);
            exit(-1);
        }

        std::vector<char const*> processes = std::vector<char const*> ({"capture", "process"});
        std::vector<std::pair<char*, char*>> processes_sem = std::vector<std::pair<char*, char*>>({{CAPTURE_SEM, PROCESS_SEM},
                                                                                                   {PROCESS_SEM, CAPTURE_SEM}});
        std::vector<pid_t> children;

        for (size_t i = 0; i < processes.size(); i++) {

            pid_t result = fork();

            if (result < 0) {
                std::cerr << "Unable to create new process" << std::endl;
                end_processes(children);
                exit(-1);
            }

            if (result == 0) {
                execl(processes[i], (char*)"0",processes_sem[i].first , processes_sem[i].second, filenames[0], NULL);
            } else {
                children.push_back(result);
            }

        }

        wait(NULL);

        end_processes(children);

        sem_close(sem_capture);
        sem_close(sem_process);

        sem_unlink(PROCESS_SEM);
        sem_unlink(CAPTURE_SEM);

        destroy_shared_memory_block(filenames[0]);
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