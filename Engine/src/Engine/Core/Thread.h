#pragma once

namespace SOF
{
    struct ThreadData
    {
        std::thread::id MainThread;
        std::condition_variable ConditionMainThread;
        std::atomic<bool> DoneFlag{ false };
    };

    // Use this class directly if you have a need of a continous thread like our render thread.
    // If you wanna use a one off task or multiple simple one of tasks, refer to the ThreadPool class

    template<typename BufferData> class Thread
    {
        public:
        Thread(std::string_view thread_name)
          : m_ThreadName(std::string(thread_name)), m_ReadPtr(&m_Buffer1), m_WritePtr(&m_Buffer2), m_ExitFlag(false)
        {
            m_Thread = std::thread(&Thread::ThreadMain, this);
            SOF_INFO("Thread", "Spun up thread \"{0}\"", m_ThreadName);
        }

        ~Thread()
        {
            SOF_INFO("Thread", "Shutting down thread \"{0}\"", m_ThreadName);
            Stop();
        }

        template<typename F, typename... Args> void Run(F &&func, Args &&...args)
        {
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_QueueEmptyFlag = false;
                m_TaskQueue.push(
                  [func = std::forward<F>(func), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                      std::apply(func, std::move(args_tuple));
                  });
            }
            m_Condition.notify_one();
        }
        void Stop()
        {
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_ExitFlag = true;
            }
            m_Condition.notify_all();

            if (m_Thread.joinable()) { m_Thread.join(); }
        }

        template<typename F, typename... Args> void SetShutdownTask(F &&func, Args &&...args)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (!m_ExitFlag) {
                m_ShutdownTask = [func = std::forward<F>(func),
                                   args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                    std::apply(func, std::move(args_tuple));
                };
            }
        }

        void WaitForAllTasks()
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            if (m_TaskQueue.empty() && m_QueueEmptyFlag) { return; }
            m_QueueEmptyCondition.wait(lock, [this]() { return m_QueueEmptyFlag && m_TaskQueue.empty(); });
        }

        BufferData *GetReadBuffer() { return m_ReadPtr; }
        BufferData *GetWriteBuffer() const { return m_WritePtr; }

        void SwapBuffers() { std::swap(m_ReadPtr, m_WritePtr); }

        private:
        void ThreadMain()
        {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m_Mutex);
                    m_Condition.wait(lock, [this]() { return !m_TaskQueue.empty() || m_ExitFlag; });
                    if (m_ExitFlag && m_TaskQueue.empty()) { break; }
                    if (!m_TaskQueue.empty()) {
                        task = std::move(m_TaskQueue.front());
                        m_TaskQueue.pop();
                    }
                }
                if (task) { task(); }
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    if (m_TaskQueue.empty()) {
                        m_QueueEmptyFlag = true;
                        m_QueueEmptyCondition.notify_one();
                    }
                }
            }

            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_ShutdownTask) { m_ShutdownTask(); }
        }

        private:
        std::string m_ThreadName = "A Thread";
        std::mutex m_Mutex;
        std::thread m_Thread;
        std::atomic<bool> m_ExitFlag{ false };
        std::atomic<bool> m_FunctionReady{ false };
        std::atomic<bool> m_DoneFlag{ false };
        BufferData m_Buffer1, m_Buffer2;
        BufferData *m_ReadPtr = nullptr, *m_WritePtr = nullptr;
        std::condition_variable m_Condition;
        std::queue<std::function<void()>> m_TaskQueue;
        std::function<void()> m_ShutdownTask;
        std::condition_variable m_QueueEmptyCondition;
        std::atomic<bool> m_QueueEmptyFlag{ true };
    };
}// namespace SOF