#include <iostream>
#include <thread>
#include <chrono>

void worker()
{
    std::cout << "[子线程] 1. 我已经开始干活了！\n";
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟耗时操作
    std::cout << "[子线程] 2. 我的活干完了！\n";
}

int main()
{
    std::cout << "[主线程] 准备创建子线程...\n";

    // 【关键点1】代码执行到这里，worker() 立刻在后台开始跑了！
    std::thread t(worker);

    std::cout << "[主线程] 我继续干我自己的事，不急着等子线程。\n";
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 主线程自己忙1秒

    std::cout << "[主线程] 我的事忙完了，现在调用 join() 等子线程收工...\n";

    // 【关键点2】主线程卡在这里，等待子线程剩下的 1 秒跑完
    t.join();

    std::cout << "[主线程] 子线程终于结束了，我们一起安全退出。\n";
    return 0;
}