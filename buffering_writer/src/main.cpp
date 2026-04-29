#include "buffering_writer.h"
#include "record.h"

#include <string>
#include <thread>
#include <vector>

int main() {
    const std::string filename = "out.txt";
    buffering_writer writer(filename);

    const int threads_count = 5;
    const int recs_per_thread = 10;

    std::vector<std::thread> threads;
    threads.reserve(threads_count);

    for (int i = 0; i < threads_count; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < recs_per_thread; ++j) {
                writer.write(record_t{"Hello from thread " + std::to_string(i) + "!\n"});
            }
        });
    }

    for (auto& thr : threads) {
        thr.join();
    }
    return 0;
}
