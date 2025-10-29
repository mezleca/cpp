#include "pool.hpp"

ThreadPool::ThreadPool(int count) {
    for (int i = 0; i < count; i++) {
        // add new worker
        workers.emplace_back([this]() {
            // initialize task loop
            while (true) {
                std::function<void()> task;
                // wait for new task
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    // use condition variable to make threads go zzz until we actually have a task to do
                    cv.wait(lock, [this] { 
                        return !tasks.empty();
                    });

                    if (tasks.empty()) {
                        return;
                    }

                    // move task from queue to var
                    task = std::move(tasks.front());

                    // since we moved the task data to the var
                    // queue will still hold the pointer but now the pointer points to nothing
                    // so lets remove it to prevent future errors
                    tasks.pop();
                }

                // execute it :)
                task();
            }
        });
    }
};

ThreadPool::~ThreadPool() {
    // wake up all lazy ass threads
    cv.notify_all();

    // free them
    for (std::thread& t : workers) {
        t.join();
    }
};

template<class T>
void ThreadPool::enqueue(T&& f) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::forward<T>(f));
    }
    cv.notify_one();
};