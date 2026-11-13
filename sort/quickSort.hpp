#include <bits/stdc++.h>
using namespace std;

int partition(vector<int>& nums, int l, int r)
{
    int randIdx = l + rand() % (r - l + 1);
    int pivot = nums[randIdx];
    swap(nums[l], nums[randIdx]);
    int i = l + 1;
    int j = r;
    while (true)
    {   // 从左到右找第一个大于等于pivot的数
        while (i <= j && nums[i] < pivot) ++i;
        // 从右到左找第一个小于pivot的数，最终升序
        while (i <= j && nums[j] > pivot) --j;
        if (i >= j) break;
        swap(nums[i], nums[j]);
        ++i;
        --j;
    }
    swap(nums[l], nums[j]);
    return j;
}

void quickSort(vector<int>& nums, int l, int r)
{
    if (l >= r)
        return;
    int p = partition(nums, l, r);
    quickSort(nums, l, p - 1);
    quickSort(nums, p + 1, r);
}

void quickSort(vector<int>& nums)
{
    quickSort(nums, 0, nums.size() - 1);
}