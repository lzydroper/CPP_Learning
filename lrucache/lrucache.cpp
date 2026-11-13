/**
 * LRUCache，最近最少使用缓存
 * 基于时间局部性原理
 * 目的：在有限且昂贵的快速存储空间（如内存）中，最大化缓存命中率，以尽可能低的开销平衡“存储容量受限”与“数据吞吐/响应延迟要求高”之间的矛盾
 * 用于：虚拟内存页面置换、CPU 多级缓存、Redis 内存逐出策略、游戏引擎资源加载器（Assets / Shader 缓存）、本地 HTTP 请求缓存 / 图片缓存（Glide、Picasso）
 * Capacity（容量上限）：缓存可容纳的最大条目数 $C$（必须为正整数）。
 * get(key)：
 *          若 key 存在于缓存中，返回对应的 value，并将该条目刷新为“最新访问”。
 *          若 key 不存在，返回约定的未命中标志（如 -1、null 或抛出异常）。
 *          时间复杂度要求：平均 $O(1)$。
 * put(key, value)：
 *          若 key 已存在，更新其 value，并将该条目刷新为“最新访问”。
 *          若 key 不存在，将新键值对插入缓存，并标记为“最新访问”。容量溢出淘汰：
 *          若插入后总条目数超过容量 $C$，则必须在常数时间内淘汰并删除“最久未被访问”（Least Recently Used）的条目。
 *          时间复杂度要求：平均 $O(1)$。
 * 在实际工程系统（如 Linux 内核页置换、Redis、Guava Cache）中，完整的 LRU 定义通常还会补充以下约束：
 *          1. TTL / 过期时间机制：除容量驱逐外，支持基于时间的自然过期（Time To Live / Time To Idle）。
 *          2. 并发控制契约：在多线程并发读写下保证原子性与状态一致性（如分段锁或并发无锁队列）。
 *          3. 针对缓存污染的改进（LRU-K / 2Q / LIRS）：经典 LRU 存在“偶发批量扫描会导致热点数据全被洗掉”的缺陷，
 *                                                      工业级实现常结合频次（LFU 思想）演化为 LRU-2 等变体。
 */
#include <unordered_map>

class LRUCache
{
    struct Node
    {
        int key;
        int value;
        Node* prv;
        Node* nxt;
        Node(int key = 0, int value = 0)
            : key(key), value(value), prv(nullptr), nxt(nullptr)
        { }
    };
    int m_capacity;
    Node* dhead;
    Node* dtail;
    std::unordered_map<int, Node*> m_key2Node;

    void removeNode(Node* node)
    {
        node->prv->nxt = node->nxt;
        node->nxt->prv = node->prv;
    }

    void addToHead(Node* node)
    {
        Node* oldHead = dhead->nxt;
        oldHead->prv = node;
        node->nxt = oldHead;
        node->prv = dhead;
        dhead->nxt = node;
    }

    void moveToHead(Node* node)
    {
        removeNode(node);
        addToHead(node);
    }

    Node* removeTail()
    {
        Node* ret = dtail->prv;
        removeNode(ret);
        return ret;
    }
public:
    LRUCache(int capacity)
        : m_capacity(capacity)
    {
        dhead = new Node();
        dtail = new Node();
        dhead->nxt = dtail;
        dtail->prv = dhead;
    }

    ~LRUCache()
    {
        Node* cur = dhead;
        while (cur != nullptr)
        {
            Node* nxt = cur->nxt;
            delete cur;
            cur = nxt;
        }
    }

    int get(int key)
    {
        auto it = m_key2Node.find(key);
        if (it == m_key2Node.end())
            return -1;
        moveToHead(it->second);
        return it->second->value;
    }

    void put(int key, int value)
    {
        auto [it, inserted] = m_key2Node.try_emplace(key, nullptr);
        if (inserted)
        {
            Node* add = new Node(key, value);
            addToHead(add);
            it->second = add;
            if (m_key2Node.size() > m_capacity)
            {
                Node* del = removeTail();
                m_key2Node.erase(del->key);
                delete del;
            }
        }
        else
        {
            moveToHead(it->second);
            it->second->value = value;
        }
    }
};