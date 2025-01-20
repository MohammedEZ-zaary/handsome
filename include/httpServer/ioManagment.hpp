#ifndef IO_CONTEXT
#define IO_CONTEXT

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class io_context {
public:
  // Constructor that starts the worker threads
  io_context(size_t num_threads = std::thread::hardware_concurrency())
      : stopped_(false) {
    // Start the threads that will execute tasks
    for (size_t i = 0; i < num_threads; ++i) {
      workers_.emplace_back(&io_context::worker_thread, this);
    }
  }

  // Destructor to join all threads when stopping
  ~io_context() { stop(); }

  // Post a task to be executed asynchronously
  void post(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      task_queue_.push(task);
    }
    cv_.notify_one(); // Notify one worker thread to execute the task
  }

  // Stop the execution and join all threads
  void stop() {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      stopped_ = true;
    }
    cv_.notify_all(); // Notify all threads to exit
    for (auto &worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

private:
  // Worker function that executes tasks from the queue
  void worker_thread() {
    while (true) {
      std::function<void()> task;

      {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        cv_.wait(lock, [this] { return stopped_ || !task_queue_.empty(); });

        if (stopped_ && task_queue_.empty()) {
          return; // Exit the thread if stopped and queue is empty
        }

        task = std::move(task_queue_.front());
        task_queue_.pop();
      }

      task(); // Execute the task
    }
  }

  bool stopped_;
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> task_queue_;
  std::mutex queue_mutex_;
  std::condition_variable cv_;
};

#endif
