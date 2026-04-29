#include <memory>

struct record_t;

class buffering_writer {

public:
    explicit buffering_writer(std::string const& filename);
    ~buffering_writer();

    void write(const record_t& record);

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    const Impl* pimpl() const { return m_pImpl.get(); }
    Impl* pimpl() { return m_pImpl.get(); }
};
