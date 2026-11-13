#include <bits/stdc++.h>
using namespace std;

void merge(vector<int>& arr, int l, int m, int r)
{
    if (l >= r)
        return;
    // 复制临时数组
    vector<int> larr(arr.begin() + l, arr.begin() + m + 1);
    vector<int> rarr(arr.begin() + m + 1, arr.begin() + r + 1);
    // 合并临时数组
    int lidx = 0, ridx = 0;
    int lsize = larr.size(), rsize = rarr.size();
    while (lidx < lsize && ridx < rsize)
        if (larr[lidx] <= rarr[ridx])
            arr[l++] = larr[lidx++];
        else
            arr[l++] = rarr[ridx++];
    // 复制剩余元素
    while (lidx < lsize)
        arr[l++] = larr[lidx++];
    while (ridx < rsize)
        arr[l++] = rarr[ridx++];
}

void mergeSort(vector<int>& arr, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void mergeSort(vector<int>& arr)
{
    mergeSort(arr, 0, arr.size() - 1);
}