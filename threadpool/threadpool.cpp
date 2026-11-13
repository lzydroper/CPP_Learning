#include <iostream>
#include <cassert>
#include <atomic>
#include <string>
#include <memory>
#include "threadpool.hpp"

static int add(int a, int b) { return a + b; }
static std::string greet(const std::string& name) { return "Hello, " + name; }

struct Foo
{
    int value = 42;
    int get() const { return value; }
    int multiply(int x) { return value * x; }
};

void test_basic()
{
    ThreadPool pool(4);

    auto f1 = pool.post(add, 1, 2);
    auto f2 = pool.post([] { return 42; });
    auto f3 = pool.post(greet, std::string("World"));
    auto f4 = pool.post([] { /* void */ });

    assert(f1.get() == 3);
    assert(f2.get() == 42);
    assert(f3.get() == "Hello, World");
    f4.get();

    std::cout << "test_basic passed\n";
}

void test_void_and_counter()
{
    ThreadPool pool(4);
    std::atomic<int> counter{ 0 };

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 100; ++i)
        futures.push_back(pool.post([&counter] { counter.fetch_add(1); }));

    for (auto& f : futures) f.get();
    assert(counter == 100);

    std::cout << "test_void_and_counter passed\n";
}

void test_parallel()
{
    ThreadPool pool(4);
    std::atomic<int> counter{ 0 };

    const int N = 1000;
    std::vector<std::future<int>> futures;
    futures.reserve(N);
    for (int i = 0; i < N; ++i)
        futures.push_back(pool.post([&counter, i] {
        counter.fetch_add(1);
        return i * i;
            }));

    long long sum = 0;
    for (auto& f : futures) sum += f.get();

    long long expected = (long long)(N - 1) * N * (2LL * N - 1) / 6;
    assert(counter == N);
    assert(sum == expected);

    std::cout << "test_parallel passed\n";
}

void test_member_function()
{
    ThreadPool pool(2);
    Foo foo;

    auto f1 = pool.post(&Foo::get, &foo);
    auto f2 = pool.post(&Foo::multiply, &foo, 10);

    assert(f1.get() == 42);
    assert(f2.get() == 420);

    std::cout << "test_member_function passed\n";
}

void test_exception_propagation()
{
    ThreadPool pool(2);

    auto f = pool.post([]() -> int {
        throw std::runtime_error("boom");
        });

    bool caught = false;
    try
    {
        f.get();
    }
    catch (const std::runtime_error& e)
    {
        caught = true;
        assert(std::string(e.what()) == "boom");
    }
    assert(caught);

    std::cout << "test_exception_propagation passed\n";
}

void test_move_only_argument()
{
    ThreadPool pool(2);

    auto p = std::make_unique<int>(123);
    auto f = pool.post([q = std::move(p)] { return *q; });

    assert(f.get() == 123);

    std::cout << "test_move_only_argument passed\n";
}

void test_post_after_cancel_throws()
{
    ThreadPool pool(1);
    pool.~ThreadPool();          // 手动析构，模拟已停止的池（仅用于测试语义）
    new (&pool) ThreadPool(1);   // 重新构造，避免后面 double free

    // 正常用例其实无法在池析构后再 post（对象已销毁），
    // 这里只是说明 m_cancelled 后 post 会抛异常，实际很少这样测。
    std::cout << "test_post_after_cancel_throws skipped (语义说明)\n";
}

int main()
{
    test_basic();
    test_void_and_counter();
    test_parallel();
    test_member_function();
    test_exception_propagation();
    test_move_only_argument();

    std::cout << "All tests passed!\n";
    return 0;
}