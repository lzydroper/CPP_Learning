#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>

class minheap
{
private:
    std::vector<int> values;
private:
    void siftDown(int p)
    {
        int n = values.size();
        while (2 * p + 1 < n)
        {
            // 找到更小的孩子
            int c = 2 * p + 1;
            if (c + 1 < n && values[c + 1] < values[c])
                c = c + 1;
            // 如果父节点更小，说明合规，退出
            if (values[p] <= values[c])
                break;
            // 否则持续将大的节点下沉
            std::swap(values[p], values[c]);
            p = c;
        }
    }
    void siftUp(int c)
    {
        while (c > 0)
        {
            // 找到父节点
            int p = (c - 1) / 2;
            // 如果父节点更小，说明合规，退出
            if (values[p] <= values[c])
                break;
            // 否则持续将小的节点上浮
            std::swap(values[p], values[c]);
            c = p;
        }
    }
public:
    minheap() {}
    
    explicit minheap(const std::vector<int>& _values)
        : values(_values)
    {
        if (values.empty()) return;
        // 从最后一个非叶子节点开始，从后往前把大元素下沉
        for (int i = values.size() / 2 - 1; i >= 0; --i)
            siftDown(i);
    }

    void push(int x)
    {
        values.push_back(x);
        siftUp(values.size() - 1);
    }

    void pop()
    {
        if (values.empty())
            throw std::out_of_range("heap is empty");
        // 将末尾元素覆盖到首元素，然后更新
        values[0] = values.back();
        values.pop_back();
        if (!values.empty())
            siftDown(0);
    }

    int top() const
    {
        if (values.empty())
            throw std::out_of_range("heap is empty");
        return values[0];
    }

    bool empty() const
    {
        return values.empty();
    }

    size_t size() const
    {
        return values.size();
    }

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i) oss << ", ";
            oss << values[i];
        }
        oss << "]";
        return oss.str();
    }
};

#include <iostream>
int main()
{
    minheap pq;
    for (int x : {5, 3, 8, 1, 9, 2})
        pq.push(x);

    std::cout << pq.to_string() << '\n';

    for (int x : {5, 3, 8, 1, 9, 2})
        pq.push(x);

    std::cout << pq.to_string() << '\n';

    while (!pq.empty())
    {
        std::cout << pq.top() << ' ';
        pq.pop();
    }
    // 输出：1 2 3 5 8 9
}