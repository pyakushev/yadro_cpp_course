#include "buffering_writer.h"
#include "record.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <condition_variable>
#include <stdexcept>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <cstring>
#include <thread>

struct buffering_writer::Impl {
    std::mutex m_mutex;
    std::string m_filename;
    int m_file = -1;
    unsigned m_counter = 0;
    std::list<record_t> m_pending;
    std::thread m_flusher;
    bool m_stop = false;
    bool m_failed  = false;
    std::condition_variable m_cv;

    void flush_task() {
        try {
            while (true) {
                std::unique_lock<std::mutex> lock(m_mutex);
                
                m_cv.wait(lock, [this]() { return !m_pending.empty() || m_stop; });
                if (m_stop && m_pending.empty()) {
                    return;
                }
                record_t record = std::move(m_pending.front());
                m_pending.pop_front(); 
                --m_counter;
                lock.unlock();
                write_all(std::move(record));
            }
        } catch (...) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_failed) {
                m_failed = true;
                std::cerr << "I/O error in buffering_writer while writing to " << m_filename << std::endl;
            }
        }
    }
    
    void write_all(record_t record) {
        size_t total_written = 0;
        size_t length = record.content.length();
        const char* buf = record.content.data();
        while (total_written < length) {
            ssize_t bytes_written = ::write(
                m_file,
                buf + total_written,
                length - total_written
            );
            if (bytes_written < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error("Error writing to " + m_filename);
            }
            if (bytes_written == 0) {
                throw std::runtime_error("Error writing to " + m_filename);
            }
            total_written += static_cast<size_t>(bytes_written);
        }
    }

    explicit Impl(std::string const& filename)
        : m_filename(filename),
          m_file(open(m_filename.c_str(), O_WRONLY|O_CREAT|O_APPEND, 0644))
    {
        if (m_file < 0) {
            throw std::runtime_error("Error accessing file!");
        }
        m_flusher = std::thread(&Impl::flush_task, this);
    }

    Impl(Impl const&) = delete;
    ~Impl() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_one();
        if (m_flusher.joinable()) {
            m_flusher.join();
        }
        if (m_file >= 0) {
            close(m_file);
        } 
    };
};

buffering_writer::buffering_writer(std::string const& filename)
    : m_pImpl(std::make_unique<Impl>(filename)) {}

void buffering_writer::write(const record_t& record) {
    std::lock_guard<std::mutex> lock(pimpl()->m_mutex);
    if (pimpl()->m_failed) {
        throw std::runtime_error("Can't write after failure in " + pimpl()->m_filename);
    }
    pimpl()->m_pending.push_back(record);
    ++pimpl()->m_counter;
    pimpl()->m_cv.notify_one();
}

buffering_writer::~buffering_writer() = default;
