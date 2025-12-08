#ifndef SHM_COMMON_H
#define SHM_COMMON_H

#include <sys/shm.h>    // Shared memory
#include <semaphore.h>  // Semaphores
#include <iostream>
#include <string>
#include <cstring>
#include "../core/message.h"  // Reuse Message from Member 4

// Shared memory key and size
#define SHM_KEY 0x1234
#define SEM_NAME "/chat_sem"
#define MAX_MESSAGES 10


// Plain struct for shared memory (POD only!)
struct ShmMessage {
    char sender[32];
    char content[256];
    double timestamp;
};

// Shared memory structure
struct SharedMemory {
    ShmMessage messages[MAX_MESSAGES];
    int count;              // Number of messages stored
    bool sender_done;       // Signal that sender finished
};

#endif