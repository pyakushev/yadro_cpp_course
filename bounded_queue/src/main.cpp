#include "bounded_queue.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>
#include <vector>

constexpr int CAPACITY = 50;
constexpr int AMOUNT = 10000;

int main() {

    BoundedQueue<int, CAPACITY> q;
    std::mutex mx;
    std::vector<int> produced, consumed;

    std::thread producer([&q, &mx, &produced]() {
        for (int i = 0; i < AMOUNT; ++i) {
            q.push(i);
            {
                std::lock_guard lock(mx);
                produced.push_back(i);
            }
        }
    });

    std::thread consumer([&q, &mx, &consumed]() {
        for (int i = 0; i < AMOUNT; ++i) {
            auto value = q.pop();
            {
                std::lock_guard lock(mx);
                consumed.push_back(value);
            }
        }
    });

    consumer.join();
    producer.join();

    assert(consumed.size() == AMOUNT);
    assert(produced.size() == AMOUNT);

    std::sort(produced.begin(), produced.end());
    std::sort(consumed.begin(), consumed.end());

    assert(produced == consumed);
    std::cout << "success" << std::endl;
}
