#include "shared_ptr.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

void test_shared_ptr_thread_safety()
{
    shared_ptr<int> ptr(new int(42));
    //创建多个线程，每个线程都增加和减少引用计数
    const int num_threads = 10;
    std::vector<std::thread>threads;
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(
            [&ptr]()
            {
                for (int j = 0; j < 10000; ++j)
                {
                    shared_ptr<int> local_ptr(ptr);
                    //短暂暂停，增加线程切换的可能性
                    std::this_thread::yield();
                    // std::this_thread::sleep_for(1ms);
                }
            });
    }
    //等待所有线程完成
    for (auto& thread : threads)
    {
        thread.join();
    }
    //检查引用计数是否正确
    std::cout << "use_count: " << ptr.use_count() << std::endl;
    if (ptr.use_count() == 1)
        std::cout << "Test passed: shared_ptr is thread-safe!" << std::endl;
    else
        std::cout << "Test failed: shared ptr is not thread-safe!" << std::endl;
}

int main()
{
    std::cout << "Starting thread safety test..." << std::endl;
    test_shared_ptr_thread_safety();
    return 0;
}