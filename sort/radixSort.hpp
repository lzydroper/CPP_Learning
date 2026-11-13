#include <bits/stdc++.h>
using namespace std;

void radixSort(vector<int>& arr)
{
    int m = *max_element(arr.begin(), arr.end());
    for (int digit = 1; digit <= m; digit *= 10)
    {   // 10 个桶 (0-9)
        vector<vector<int>> buckets(10);
        // 分配到对应桶中
        for (auto num : arr)
            // 虽然加了abs，但是依然无法处理负数
            buckets[std::abs((long long)num / digit) % 10].push_back(num);
        // 收集
        int i = 0;
        for (auto& bucket : buckets)
            for (auto num : bucket)
                arr[i++] = num;
    }
}