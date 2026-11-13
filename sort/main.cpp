#include <bits/stdc++.h>
#include "heapSort.hpp"
#include "quickSort.hpp"
#include "mergeSort.hpp"
#include "insertSort.hpp"
#include "radixSort.hpp"

using namespace std;

enum class Dist { Random, Sorted, Reversed, NearlySorted, FewUnique };

const char* distName(Dist d) {
    switch (d) {
    case Dist::Random:       return "Random";
    case Dist::Sorted:       return "Sorted";
    case Dist::Reversed:     return "Reversed";
    case Dist::NearlySorted: return "NearlySorted";
    case Dist::FewUnique:    return "FewUnique";
    }
    return "?";
}

std::vector<int> makeData(size_t n, Dist d, unsigned seed = 42) {
    std::mt19937 rng(seed);
    std::vector<int> v(n);
    switch (d) {
    case Dist::Random:
        for (auto& x : v) x = static_cast<int>(rng()) & 0x7FFFFFFF;  // 保证非负
        break;
    case Dist::Sorted:
        std::iota(v.begin(), v.end(), 0);
        break;
    case Dist::Reversed:
        std::iota(v.rbegin(), v.rend(), 0);
        break;
    case Dist::NearlySorted:
        std::iota(v.begin(), v.end(), 0);
        for (size_t k = 0; k < n / 100 + 1; ++k) {
            size_t i = rng() % n, j = rng() % n;
            std::swap(v[i], v[j]);
        }
        break;
    case Dist::FewUnique:
        for (auto& x : v) x = static_cast<int>(rng() % 10);
        break;
    }
    return v;
}

template <typename F>
double timeIt(const std::vector<int>& base, F&& f, int repeats = 3) {
    std::vector<int> work(base.size());
    double best = 1e300;
    for (int k = 0; k < repeats; ++k) {
        std::copy(base.begin(), base.end(), work.begin());  // 拷贝不计时
        auto start = std::chrono::steady_clock::now();
        f(work);
        auto end = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        best = std::min(best, ms);
    }
    return best;
}

struct Algorithm {
    const char* name;
    void (*fn)(std::vector<int>&);
};

const Algorithm kAlgorithms[] = {
    {"insert",           insertSort},
    {"merge",            mergeSort},
    {"quick",            quickSort},
    {"heap",             heapSort},
    {"radix",            radixSort},
};

void runBenchmark(size_t n, Dist d) {
    auto base = makeData(n, d);
    std::cout << "\n=== n = " << n << ", distribution = " << distName(d) << " ===\n";
    std::cout << std::left << std::setw(20) << "algorithm"
        << std::right << std::setw(12) << "best(ms)" << "   result\n";
    for (const auto& alg : kAlgorithms) {
        double ms = timeIt(base, alg.fn);
        auto copy = base;
        alg.fn(copy);
        bool ok = std::is_sorted(copy.begin(), copy.end());
        std::cout << std::left << std::setw(20) << alg.name
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(12) << ms << "   " << (ok ? "ok" : "WRONG!") << "\n";
    }
}

int main() {
    const size_t sizes[] = { 10000, 200000 };
    const Dist dists[] = { Dist::Random, Dist::Sorted, Dist::Reversed,
                          Dist::NearlySorted, Dist::FewUnique };
    for (size_t n : sizes)
        for (Dist d : dists)
            runBenchmark(n, d);
    return 0;
}