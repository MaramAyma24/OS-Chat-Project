#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @class ThreadSafeQueue
 * @brief A thread-safe queue for passing messages between threads.
 *        Used by both Socket and Shared Memory chat systems.
 * @tparam T The type of data stored (e.g., Message)
 */
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    /**
     * @brief Push a value into the queue.
     * @param new_value The value to push (moved into queue)
     */
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }

    /**
     * @brief Wait until an item is available, then pop it.
     * @param value Output parameter to store the popped item
     */
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    /**
     * @brief Try to pop an item without waiting.
     * @param value Output parameter
     * @return true if an item was popped, false if queue was empty
     */
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) {
            return false;
        }
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }

    /**
     * @brief Check if the queue is empty.
     * @return true if empty
     */
    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

#endif // THREAD_SAFE_QUEUE_H