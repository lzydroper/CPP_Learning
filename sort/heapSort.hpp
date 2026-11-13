#include <bits/stdc++.h>
using namespace std;

void siftDown(vector<int>& arr, int p, int n)
{
    while (2 * p + 1 < n)
    {
        // 找到更大的孩子
        int c = 2 * p + 1;
        if (c + 1 < n && arr[c + 1] > arr[c])
            c = c + 1;
        // 如果父节点更大，说明合规，退出
        if (arr[p] > arr[c])
            break;
        // 否则持续将小的节点下沉
        swap(arr[p], arr[c]);
        p = c;
    }
}

void heapSort(vector<int>& arr)
{
    int n = arr.size();
    // 建堆
    for (int i = n / 2 - 1; i >= 0; --i)
        siftDown(arr, i, n);
    // 排序，大顶堆，所以是升序
    for (int i = n - 1; i > 0; --i)
    {
        swap(arr[0], arr[i]);
        siftDown(arr, 0, i);
    }
}