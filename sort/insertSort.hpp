#include <bits/stdc++.h>
using namespace std;

void insertSort(vector<int>& arr)
{
    int n = arr.size();
    // 从第二个开始，把比当前元素大的往后移动
    for (int i = 1; i < n; ++i)
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}