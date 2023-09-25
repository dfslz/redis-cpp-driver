/*
* Created by luozhi on 2023/09/10.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_THREAD_POOL_H
#define REDIS_CPP_DRIVER_THREAD_POOL_H

#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
    explicit ThreadPool(uint num) {
        while(num--) {
            pool_.emplace_back(&ThreadPool::TaskUnit, this);
        }
    }

    ~ThreadPool() {
        exit_ = true;
        wakeup_.notify_all();

        for (auto& unit : pool_) {
            if (unit.joinable()) {
                unit.join();
            }
        }
    }

    void TaskUnit() {
        while(!exit_) {
            Task task;
            { // 限制锁的范围，防止任务执行期间阻塞其他线程获取任务
                std::unique_lock<std::mutex> queLocker(taskLocker_);
                wakeup_.wait(queLocker, [&]() {
                    return exit_ || !taskQueue_.empty();
                });
                if (exit_) {
                    break;
                }
                task = std::move(taskQueue_.front());
                taskQueue_.pop();
            }

            task();
        }
    }

    template<class Func, class... Args>
    auto commit(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {
        using RetType = decltype(func(args...)); // 获取返回值类型
        auto pack = std::make_shared<std::packaged_task<RetType(Args&&...)>>(func); // 打包任务
        {
            std::unique_lock<std::mutex> lock(taskLocker_);
            taskQueue_.emplace([=]() { (*pack)(std::forward<Args>(args)...); }); // 包装成void返回值lambda塞进任务队列
        }
        wakeup_.notify_one();
        return pack->get_future(); // 返回future以便获取执行返回值
    }

private:
    using Task = std::function<void()>;
    std::vector<std::thread> pool_;
    std::queue<Task> taskQueue_;
    std::mutex taskLocker_;
    std::atomic_bool exit_{false};
    std::condition_variable wakeup_;
};

#endif // REDIS_CPP_DRIVER_THREAD_POOL_H
