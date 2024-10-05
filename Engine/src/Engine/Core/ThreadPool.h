#pragma once

namespace SOF
{
    class ThreadPool
    {
        public:
        ThreadPool(size_t thread_count = std::thread::hardware_concurrency()) : m_ExitFlag(false), m_ActiveTasks(0)
        {
            for (size_t i = 0; i < thread_count; ++i) {
                m_Threads.emplace_back([this]() { this->ThreadLoop(); });
            }
        }

        ~ThreadPool() { Shutdown(); }

        template<typename F, typename... Args>
        auto AddTask(F &&func, Args &&...args) -> std::future<typename std::invoke_result_t<F, Args...>>
        {
            using ReturnType = typename std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
              std::bind(std::forward<F>(func), std::forward<Args>(args)...));
            std::future<ReturnType> result = task->get_future();

            // Lock only for queue modification
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_TaskQueue.emplace([task]() {
                    SOF_PROFILE_THREAD("Thread pool task");
                    (*task)();
                });
                m_ActiveTasks.fetch_add(1, std::memory_order_relaxed);// Using atomic
            }

            m_TaskAvailable.notify_one();
            return result;
        }

        void Await()
        {
            std::unique_lock<std::mutex> lock(m_TaskMutex);
            m_AwaitCondition.wait(lock, [this]() {
                return m_ActiveTasks.load(std::memory_order_relaxed) == 0;// Only wait for all tasks to finish
            });
        }

        void Shutdown()
        {
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_ExitFlag = true;
            }
            m_TaskAvailable.notify_all();
            for (std::thread &thread : m_Threads) {
                if (thread.joinable()) { thread.join(); }
            }
        }

        private:
        void ThreadLoop()
        {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_TaskAvailable.wait(lock, [this]() { return m_ExitFlag || !m_TaskQueue.empty(); });
                    if (m_ExitFlag && m_TaskQueue.empty()) return;

                    if (!m_TaskQueue.empty()) {
                        task = std::move(m_TaskQueue.front());
                        m_TaskQueue.pop();
                    }
                }

                if (task) {
                    task();
                    m_ActiveTasks.fetch_sub(1, std::memory_order_relaxed);// Atomic decrement
                    if (m_ActiveTasks.load(std::memory_order_relaxed) == 0) {
                        std::lock_guard<std::mutex> lock(m_TaskMutex);
                        m_AwaitCondition.notify_all();// Signal when all tasks are done
                    }
                }
            }
        }

        std::vector<std::thread> m_Threads;
        std::queue<std::function<void()>> m_TaskQueue;
        std::mutex m_QueueMutex;
        std::mutex m_TaskMutex;// Separate mutex for waiting
        std::condition_variable m_TaskAvailable;
        std::condition_variable m_AwaitCondition;
        std::atomic<bool> m_ExitFlag;
        std::atomic<size_t> m_ActiveTasks;
    };
}// namespace SOF