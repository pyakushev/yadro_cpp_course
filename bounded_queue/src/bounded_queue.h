#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

template <typename ItemType, std::size_t max_capacity>
class BoundedQueue {
    static_assert(max_capacity > 0, "invalid capacity, should be > 0");
private:
    std::mutex m_mx;
    std::condition_variable m_cv;
    std::queue<ItemType> m_queue;

public:
    BoundedQueue() = default;
    BoundedQueue(const BoundedQueue& bq) = delete;
    BoundedQueue(BoundedQueue&&) = delete;
    BoundedQueue& operator=(const BoundedQueue& bq) = delete;
    BoundedQueue& operator=(BoundedQueue&& bq) = delete; 

    void push(ItemType value) {
        std::unique_lock<std::mutex> lock(m_mx);
        m_cv.wait(lock, [this]() { return m_queue.size() < max_capacity; });
        m_queue.push(std::move(value));
        lock.unlock();
        m_cv.notify_one();
    }

    [[nodiscard]] ItemType pop() {
        std::unique_lock<std::mutex> lock(m_mx);
        m_cv.wait(lock, [this]() { return !m_queue.empty(); });
        auto item = std::move(m_queue.front());
        m_queue.pop();
        lock.unlock();
        m_cv.notify_one();
        return item;
    }

    [[nodiscard]] bool try_push(ItemType value) {
        std::unique_lock<std::mutex> lock(m_mx);
        if (m_queue.size() >= max_capacity) {
            return false;
        }
        m_queue.push(std::move(value));
        lock.unlock();
        m_cv.notify_one();
        return true;
    }

    [[nodiscard]] std::optional<ItemType> try_pop() {
        std::unique_lock<std::mutex> lock(m_mx);
        if (m_queue.empty()) {
            return std::nullopt;
        }
        auto item = std::move(m_queue.front());
        m_queue.pop();
        lock.unlock();
        m_cv.notify_one();
        return item;
    }
};
