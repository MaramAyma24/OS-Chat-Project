#include "shm_common.h"
#include <unistd.h>
#include <fcntl.h>  // for O_CREAT

int main() {
    // 1. Get shared memory (already created by sender)
    int shmid = shmget(SHM_KEY, sizeof(SharedMemory), 0);
    if (shmid == -1) {
        perror("shmget (receiver)");
        return 1;
    }

    SharedMemory* shm = (SharedMemory*)shmat(shmid, nullptr, 0);
    if (shm == (void*)-1) {
        perror("shmget (receiver)");
        return 1;
    }

    // 2. Open existing semaphore
    sem_t* sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open FAILED in receiver");
        shmdt(shm);
        return 1;
    }
    std::cout << "[Receiver] Semaphore opened.\n";

    std::cout << "[Receiver] Waiting for messages...\n";
    int received = 0;

    // 3. Read until sender is done
    while (true) {
        sem_wait(sem);

        if (received < shm->count) {
            // Convert ShmMessage → Message
            ShmMessage raw = shm->messages[received];
            Message msg{std::string(raw.sender), std::string(raw.content), raw.timestamp};
            std::cout << "[Receiver] Read message " << received << ", count was: " << shm->count << std::endl;
            std::cout << "[Receiver] Got: " << msg.content << " (from " << msg.sender << ")" << std::endl;
            received++;
        }

        bool done = shm->sender_done;
        sem_post(sem);

        if (done && received >= shm->count) break;
        usleep(100000); // sleep 0.1 sec
    }

    // 4. Cleanup
    sem_close(sem);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, nullptr); // Remove shared memory
    sem_unlink(SEM_NAME);             // Remove semaphore
    std::cout << "[Receiver] Finished.\n";
    return 0;
}