#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "tracy/Tracy.hpp"

class ThreadPool {
public:
  ThreadPool()
      : threadCount_(std::thread::hardware_concurrency()) {
    threads_.resize(threadCount_);
    start();
  }

  ~ThreadPool() {
    wait();
    stop();
  }

  void ThreadPool::queue(const std::function<void()>&& task) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      jobs_.push(task);
    }
    TracyPlot("NumJobsInQueue", (int64_t)jobs_.size());
    jobQueued_.notify_one();
  }

  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    jobDone_.wait(lock, [this] {
      return jobs_.empty();
    });
  }

private:
  void start() {
    running_ = true;
    for (std::thread& thread : threads_) {
      thread = std::thread(&ThreadPool::loop, this);
    }
  }

  void stop() {
    running_ = false;
    for (auto& thread : threads_) {
      thread.join();
    }
  }

  void loop() {
    static int index{0};
    tracy::SetThreadName((std::string("Thread") + std::to_string(index)).c_str());
    index++;

    while (running_) {
      ZoneScopedN("ThreadLoop");
      std::unique_lock<std::mutex> lock(mutex_);

      jobQueued_.wait(lock, [this] {
        return !jobs_.empty();
      });
      auto task = std::move(jobs_.front());
      jobs_.pop();
      lock.unlock();
      task();
      lock.lock();
      jobDone_.notify_one();
    }
  }
  std::queue<std::function<void()>> jobs_;
  std::vector<std::thread>          threads_;
  size_t                            threadCount_{0};

  std::atomic<bool> running_{false};

  mutable std::mutex      mutex_;
  std::condition_variable jobQueued_;
  std::condition_variable jobDone_;
};
