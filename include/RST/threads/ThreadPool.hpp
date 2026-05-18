#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <string>
#include <stdexcept>
#include <cstdint>

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace RST::Threads {

    class ThreadPool {
    public:
        enum class Priority : uint8_t {
            Low = 0,
            Normal = 1,
            High = 2,
        };

        ThreadPool(std::size_t numThreads = std::thread::hardware_concurrency(), std::string_view name = "ThreadPool");
        ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        template<typename F, typename... Args>
        auto submit(F&& fn, Priority priority = Priority::Normal, Args&&... args)
        {
            if (_stopping.load(std::memory_order_acquire))
                throw std::runtime_error("ThreadPool: submit on stopped pool");

            using returnType = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

            auto task = std::make_shared<std::packaged_task<returnType()>>(
                [f = std::forward<F>(fn), ...a = std::forward<Args>(args)]() {
                    return std::invoke(std::move(f), std::move(a)...);
                }
            );

            std::future<returnType> future = task->get_future();
            {
                std::unique_lock lock(_mutex);
                _queue.push({ [task]{ (*task)(); }, priority, _submitCounter++ });
            }
            _cv.notify_one();
            return future;
        }

        void pause() noexcept;
        void resume() noexcept;
        bool isPaused() const noexcept;
        void waitForAll();

        std::size_t workerCount() const noexcept { return _workers.size(); };
        std::string_view name() const noexcept { return _name; };

    private:
        struct Task {
            std::function<void()> fn;
            Priority priority;
            std::uint64_t seqNum;

            bool operator<(const Task& o) const noexcept {
                if (priority != o.priority)
                    return static_cast<uint8_t>(priority) < static_cast<uint8_t>(o.priority);
                return seqNum > o.seqNum;
            }
        };

        void workerLoop(std::size_t workerId);

        void shutdown() noexcept;

        void setThreadName([[maybe_unused]] std::size_t id) noexcept {
#if defined(__linux__)
            const std::string n = _name.size() > 10 ? _name.substr(0, 10) + std::to_string(id) : _name + std::to_string(id);
            pthread_setname_np(pthread_self(), n.c_str());
#elif defined(__APPLE__)
            const std::string n = _name + std::to_string(id);
            pthread_setname_np(n.c_str());
#endif
        }

        std::string _name;

        std::vector<std::thread> _workers;
        std::priority_queue<Task> _queue;

        mutable std::mutex _mutex;
        std::condition_variable _cv;
        std::condition_variable _drainCv;

        std::atomic<bool> _stopping{ false };
        std::atomic<bool> _paused{ false };
        std::atomic<std::size_t> _activeTasks{ 0 };
        std::uint64_t _submitCounter{ 0 };
    };

}
