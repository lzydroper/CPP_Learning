class SingletonStatic
{
private:
    SingletonStatic();
    ~SingletonStatic();
    // 拷贝构造
    SingletonStatic(const SingletonStatic& o) = delete;
    // 拷贝赋值
    SingletonStatic& operator=(const SingletonStatic& o) = delete;
    // 移动构造
    SingletonStatic(SingletonStatic&& o) = delete;
    // 移动赋值
    SingletonStatic& operator=(SingletonStatic&& o) = delete;
public:
    static SingletonStatic& getInstance()
    {
        static SingletonStatic init;
        return init;
    }
};

#include <mutex>
#include <atomic>
#include <memory>

class SingletonThread
{
private:
    SingletonThread();
    ~SingletonThread();
    // 拷贝构造
    SingletonThread(const SingletonThread& o) = delete;
    // 拷贝赋值
    SingletonThread& operator=(const SingletonThread& o) = delete;
    // 移动构造
    SingletonThread(SingletonThread&& o) = delete;
    // 移动赋值
    SingletonThread& operator=(SingletonThread&& o) = delete;
    inline static std::atomic<SingletonThread*> I{nullptr};
    inline static std::mutex mtx{};
private:
    class Deletor
    {
    public:
        ~Deletor()
        {
            auto tmp = SingletonThread::I.load(std::memory_order_acquire);
            if (tmp != nullptr)
                delete tmp;
            SingletonThread::I.store(nullptr, std::memory_order_release);
        }
    };
    inline static Deletor deletor{};
public:
    static SingletonThread* getInstance()
    {
        auto tmp = I.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(mtx);
            tmp = I.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                tmp = new SingletonThread();
                std::atomic_thread_fence(std::memory_order_release);
                I.store(tmp, std::memory_order_relaxed);
            }
        }
        return tmp;
    }
};

template<typename T>
class SingletonT
{
protected:
    SingletonT() = default;
    ~SingletonT() = default;
private:
    // 拷贝构造
    SingletonT(const SingletonT& o) = delete;
    // 拷贝赋值
    SingletonT& operator=(const SingletonT& o) = delete;
    // 移动构造
    SingletonT(SingletonT&& o) = delete;
    // 移动赋值
    SingletonT& operator=(SingletonT&& o) = delete;
public:
    static T& getInstance()
    {
        static T init;
        return init;
    }
};

class SingletonTDerive : public SingletonT<SingletonTDerive>
{
private:
    friend class SingletonT<SingletonTDerive>;
    SingletonTDerive() {}
    ~SingletonTDerive() {}
};