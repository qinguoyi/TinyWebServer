// LockFreeQueue.h
#pragma once
#include <atomic>
#include <vector>
#include <memory>

// 行情数据结构定义
struct MarketData {
    char topic[32];     // 标的名称，如 "BTC-USDT"
    double price;       // 价格
    double volume;      // 数量
    int64_t timestamp;  // 时间戳
};

template<typename T, size_t Capacity>
class LockFreeRingBuffer {
public:
    LockFreeRingBuffer() : head_(0), tail_(0) {}

    // 生产者调用：无锁入队
    bool push(const T& item) {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Capacity;

        // acquire 保证读取 head 时获取到最新的消费状态
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // 队列满
        }

        buffer_[current_tail] = item;
        // release 保证数据写入后，tail 更新对消费者可见
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    // 消费者调用：无锁出队
    bool pop(T& item) {
        size_t current_head = head_.load(std::memory_order_relaxed);
        
        // acquire 保证读取 tail 时获取到最新的生产状态
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return false; // 队列空
        }

        item = buffer_[current_head];
        // release 保证数据读取后，head 更新对生产者可见
        head_.store((current_head + 1) % Capacity, std::memory_order_release);
        return true;
    }

private:
    T buffer_[Capacity];
    // 强制缓存行对齐，避免伪共享 (False Sharing)
    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
};
