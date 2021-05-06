//
// Created by radoslaw on 04.05.2021.
//

#ifndef SCZR_SNAKE_CONSTANTS_HPP
#define SCZR_SNAKE_CONSTANTS_HPP

const unsigned int FRAME_WIDTH = 1280;
const unsigned int FRAME_HEIGHT = 720;
const unsigned int FRAME_SIZE = 3 * FRAME_HEIGHT * FRAME_WIDTH;

const unsigned int MESS_SIZE = 4;


const unsigned int SEMAPHORES_SYNC = 0;
const unsigned int QUEUES_MEM_SYNC = 1;
const unsigned int QUEUES_RAW_SYNC = 2;

/** Semaphores synchronization constants */
#define SEM_SYNC (char*) "0"

#define PROCESS (char*) "process"
#define CAPTURE (char*) "capture"
#define GAME (char*) "game"

#define CAPTURE_PROCESS_SEM (char*) "/capture_process_sem"
#define PROCESS_CAPTURE_SEM (char*) "/process_capture_sem"

#define PROCESS_GAME_SEM (char*) "/process_game_sem"
#define GAME_PROCESS_SEM (char*) "/game_process_sem"

#define CAPTURE_PROCESS_BLOCK (char*) "/capture_process_shm"
#define PROCESS_GAME_BLOCK (char*) "/process_game_shm"

/** Queues and memory synchronization constants */
#define QUEUE_MEM_SYNC (char*) "1"

#define CAPTURE_SEND_QUEUE (char*) "/capture_send_queue"
#define CAPTURE_RECV_QUEUE (char*) "/capture_recv_queue"

#define PROCESS_SEND_QUEUE  (char*) "/process_send_queue"
#define PROCESS_RECV_QUEUE  (char*) "/process_recv_queue"

#define SENT_MESS (const char*) "Sent"
#define DONE_MESS (const char*) "Done"


/** Queues synchronization constants */
#define QUEUE_SYNC (char*) "2"

#define CAPTURE_PROCESS_QUEUE (char*) "/capture_process_queue"
#define PROCESS_GAME_QUEUE (char*) "/process_game_queue"




#endif //SCZR_SNAKE_CONSTANTS_HPP
