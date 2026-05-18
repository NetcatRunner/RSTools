#include "RST/threads/ThreadPool.hpp"

namespace RST::Threads {

    ThreadPool::ThreadPool(std::size_t numThreads, std::string_view name): _name(name)
    {
        if (numThreads == 0)
            throw std::invalid_argument("ThreadPool: numThreads must be > 0");

        _workers.reserve(numThreads);
        for (std::size_t i = 0; i < numThreads; i++) {
            _workers.emplace_back([this, i] { workerLoop(i); });
        }
    }

    ThreadPool::~ThreadPool() {
        shutdown();
    }

    void ThreadPool::workerLoop(std::size_t workerId) {
        setThreadName(workerId);

        while (true) {
            Task task;
            {
                std::unique_lock lock(_mutex);
                _cv.wait(lock, [this] {
                    return _stopping || (!_paused && !_queue.empty());
                });

                if (_stopping && _queue.empty()) {
                    return;
                }
                
                if (_paused || _queue.empty())
                    continue;

                task = std::move(const_cast<Task&>(_queue.top()));
                _queue.pop();
                _activeTasks.fetch_add(1, std::memory_order_relaxed);
            }

            task.fn();

            _activeTasks.fetch_sub(1, std::memory_order_release);
            _drainCv.notify_all();
        }
    }

    void ThreadPool::shutdown() noexcept {
        {
            std::unique_lock lock(_mutex);
            _stopping = true;
        }
        
        _cv.notify_all();
        
        for (auto& t : _workers) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void ThreadPool::pause() noexcept {
        _paused.store(true, std::memory_order_release);
    }

    void ThreadPool::resume() noexcept {
        _paused.store(false, std::memory_order_release);
        _cv.notify_all();
    }

    bool ThreadPool::isPaused() const noexcept {
        return _paused.load(std::memory_order_acquire);
    }

    void ThreadPool::waitForAll() {
        std::unique_lock lock(_mutex);
        _drainCv.wait(lock, [this] {
            return _queue.empty() && _activeTasks.load(std::memory_order_acquire) == 0;
        });
    }

}
