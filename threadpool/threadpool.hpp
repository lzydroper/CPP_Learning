#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <future>
#include <memory>
#include <condition_variable>
#include <stdexcept>
#include <type_traits>
#include <utility>

class ThreadPool
{
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_cancelled = false;
    // 工作循环
    void work()
    {
        while (true)
        {   // 所有的线程处于循环中：等待任务，拿到任务，执行任务
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                // 等待任务
                m_cv.wait(lock,
                    [this]()
                    {
                        return m_cancelled || !m_tasks.empty();
                    }
                );
                // cancel只是不允许再接任务，正在执行的任务不会被打断
                if (m_cancelled)
                    return;
                // 取得任务
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            // 执行任务
            task();
        }
    }
public:
    // 构造函数，禁止隐式转换拷贝构造
    explicit ThreadPool(int numOfThreads)
    {
        // 创建指定数量的线程
        for (int i = 0; i < numOfThreads; ++i)
        {
            m_workers.emplace_back(
                [this]
                {
                    work();
                });
        }
    }
    // 析构函数
    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_cancelled = true;
        }
        m_cv.notify_all();
        // 把正在执行的任务执行完
        for (auto& worker : m_workers)
            if (worker.joinable())
                worker.join();
    }
    // 发布任务
    template<typename Func, typename... Args>
    auto post(Func&& func, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>>
    {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        // 封装成void()
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
            // [func = std::forward<Func>(func),
            //  ...args = std::forward<Args>(args)]() mutable
            // {
            //     return std::invoke(
            //         std::forward<Func>(func),
            //         std::forward<Args>(args)...
            //     );
            // }
        );
        auto future = task->get_future();
        {   // 压入任务队列
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_cancelled)
                throw std::runtime_error("pool is stopped");
            m_tasks.push(
                [task]()
                {
                    (*task)();
                }
            );
        }
        m_cv.notify_one();
        return future;
    }
};