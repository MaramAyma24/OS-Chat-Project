#include "shm_common.h"
#include <unistd.h>
#include <fcntl.h>  // for O_CREAT

int main() {
    // Clean up any old shared memory segment
    int old_shmid = shmget(SHM_KEY, 0, 0);
    if (old_shmid != -1) {
        shmctl(old_shmid, IPC_RMID, nullptr);
    }

    // Clean up any old semaphore
    sem_unlink(SEM_NAME);

    // 1. Create shared memory
    int shmid = shmget(SHM_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    SharedMemory* shm = (SharedMemory*)shmat(shmid, nullptr, 0);
    if (shm == (void*)-1) {
        perror("shmat (sender)");
        return 1;
    }

    // 2. Create/open semaphore
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open FAILED in sender");
        shmdt(shm);
        return 1;
    }
    std::cout << "[Sender] Semaphore opened successfully with initial value 1.\n"; // ✅ Debug: this should appear

    // Initialize shared memory
    shm->count = 0;
    shm->sender_done = false;

    // 3. Send messages
    std::cout << "[Sender] Ready to send messages...\n";

    for (int i = 1; i <= 3; ++i) {
        std::cout << "[Sender] About to send message " << i << "...\n";
        // Wait for semaphore (enter critical section)
        // sem_wait(sem);

        // Add message
        // shm->messages[shm->count] = Message{"User1", "Message " + std::to_string(i), static_cast<double>(i)};
        Message msg{"User1", "Hello from sender #" + std::to_string(i), static_cast<double>(i)};


        // Convert Message → ShmMessage (safe for shared memory)
        ShmMessage shm_msg;
        strncpy(shm_msg.sender, msg.sender.c_str(), sizeof(shm_msg.sender) - 1);
        strncpy(shm_msg.content, msg.content.c_str(), sizeof(shm_msg.content) - 1);
        shm_msg.sender[sizeof(shm_msg.sender) - 1] = '\0';
        shm_msg.content[sizeof(shm_msg.content) - 1] = '\0';
        shm_msg.timestamp = msg.timestamp;

        // Enter critical section
        std::cout << "[Sender] Waiting to acquire semaphore...\n";
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            break;
        }
        std::cout << "[Sender] Acquired semaphore. Writing message...\n";

        // Write to shared memory
        if (shm->count < MAX_MESSAGES) {
            shm->messages[shm->count] = shm_msg;
            shm->count++;
            std::cout << "[Sender] Wrote message " << i << ", count now: " << shm->count << std::endl;
            std::cout << "[Sender] Sent: " << msg.content << std::endl;
        } else {
            std::cout << "[Sender] Buffer full!" << std::endl;
        }

        // shm->count++;
        // std::cout << "[Sender] Sent: Message " << i << std::endl;

        // Release semaphore
        sem_post(sem);

        sleep(1);
    }

    // Signal receiver to stop
    sem_wait(sem);
    shm->sender_done = true;
    sem_post(sem);

    // 4. Cleanup
    sem_close(sem);
    shmdt(shm);
    std::cout << "[Sender] Finished.\n";
    return 0;
}