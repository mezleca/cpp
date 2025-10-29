#pragma once 

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    void initialize(int);
    ~ThreadPool();

    template<class T>
    void enqueue(T&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<T>(f));
        }
        cv.notify_one();
    }
private:
    bool stop = false;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable cv;
};

inline ThreadPool pool;