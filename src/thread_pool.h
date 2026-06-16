#pragma once



#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>


class ThreadPool 
{
    public:
        // constructor
        ThreadPool(std::size_t);

        //destructor
        ~ThreadPool();

        //actual working function
        template<typename F, typename... Args>
        auto enqueue(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

    private:

        // keep track of threads
        std::vector<std::thread> workers;

        // actual task queue
        std::queue<std::function<void()>> tasks;

        //multithreading primitives
        std::mutex queueMutex;
        std::condition_variable condition;

        bool stop;

};


inline ThreadPool::ThreadPool(size_t threads) 
: stop(false)
{
    for(size_t i = 0; i < threads; i++)
    {
        workers.emplace_back(
            [this]
            {
                while(true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]{return this->stop || !this->tasks.empty();});

                        if(this->stop && this->tasks.empty())
                            return;

                        task = std::move(this->tasks.front());

                        this->tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}


template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using returnType = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));


    std::future<returnType> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        if(stop)
            throw std::runtime_error("thread pool has already been stopped!");

        tasks.emplace([task]() {(*task)();});
    }

    condition.notify_one();

    return result;

}


inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();
    for(std::thread &worker : workers)
        worker.join();
}