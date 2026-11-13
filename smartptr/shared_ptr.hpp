#pragma once

#include <atomic>

template<typename T>
class shared_ptr
{
private:
    T* m_ptr;
    std::atomic<int>* m_cnt;

    void release()
    {
        // 使用acqandrel保证资源释放的同步
        if (m_cnt && m_cnt->fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            delete m_ptr;
            delete m_cnt;
        }
    }
public:
    // 默认构造
    shared_ptr()
        : m_ptr(nullptr), m_cnt(nullptr)
    {
    }
    // 构造，防止隐式转换
    explicit shared_ptr(T* ptr)
        : m_ptr(ptr), m_cnt(ptr ? new std::atomic<int>(1) : nullptr)
    {
    }
    // 析构
    ~shared_ptr()
    {
        release();
    }
    // 拷贝构造
    shared_ptr(const shared_ptr& other)
        : m_ptr(other.m_ptr), m_cnt(other.m_cnt)
    {
        if (m_cnt)
            m_cnt->fetch_add(1, std::memory_order_relaxed);
    }
    // 拷贝赋值
    shared_ptr& operator=(const shared_ptr& other)
    {
        if (this != &other)
        {
            release();
            m_ptr = other.m_ptr;
            m_cnt = other.m_cnt;
            if (m_cnt)
                m_cnt->fetch_add(1, std::memory_order_relaxed);
        }
        return *this;
    }
    // 移动构造
    shared_ptr(shared_ptr&& other) noexcept
        : m_ptr(other.m_ptr), m_cnt(other.m_cnt)
    {
        other.m_ptr = nullptr;
        other.m_cnt = nullptr;
    }
    // 移动赋值
    shared_ptr& operator=(shared_ptr&& other) noexcept
    {
        if (this != &other)
        {
            release();
            m_ptr = other.m_ptr;
            m_cnt = other.m_cnt;
            other.m_ptr = nullptr;
            other.m_cnt = nullptr;
        }
        return *this;
    }
    // 解引用
    T& operator*() const
    {
        return *m_ptr;
    }
    // 箭头
    T* operator->() const
    {
        return m_ptr;
    }
    // 原始指针
    T* get() const
    {
        return m_ptr;
    }
    // 引用计数
    int use_count() const
    {
        return m_cnt ? m_cnt->load(std::memory_order_acquire) : 0;
    }
    // 重置
    void reset(T* ptr = nullptr)
    {
        release();
        m_ptr = ptr;
        m_cnt = ptr ? new std::atomic<int>(1) : nullptr;
    }
};