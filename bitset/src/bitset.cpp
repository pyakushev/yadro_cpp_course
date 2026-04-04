#include "bitset.h"
#include <algorithm>
#include <bit>

bitset::bitset() : size_(0), bits_(nullptr), true_bits_count_(0) {}

bitset::bitset(size_t initial_capacity) : 
    size_(words_for_bits(initial_capacity)),
    bits_(new Word[size_]()),
    true_bits_count_(0) {}

[[nodiscard]] size_t bitset::size() const noexcept {
    return size_ * WORD_BITS;
}

bitset::bitset(const bitset& other) : 
    size_(other.size_),
    bits_(new Word[size_]()),
    true_bits_count_(other.true_bits_count_)
{
    std::copy(other.bits_, other.bits_ + size_, bits_);
}

bitset::bitset(bitset&& other) noexcept :
    size_(other.size_), 
    bits_(other.bits_), 
    true_bits_count_(other.true_bits_count_) 
{
    other.bits_ = nullptr;
    other.size_ = 0;
    other.true_bits_count_ = 0;
}

bitset& bitset::operator=(const bitset& other) {
    if (this == &other) return *this;

    bitset new_bitset(other);
    std::swap(size_, new_bitset.size_);
    std::swap(bits_, new_bitset.bits_);
    std::swap(true_bits_count_, new_bitset.true_bits_count_);
    return *this;
}

bitset& bitset::operator=(bitset&& other) noexcept {
    if (this == &other) return *this;

    std::swap(bits_, other.bits_);
    std::swap(size_, other.size_);
    std::swap(true_bits_count_, other.true_bits_count_);
    
    return *this;
}

bitset::~bitset() noexcept {
    delete[] bits_;
    size_ = 0;
    true_bits_count_ = 0;
}

void bitset::set(size_t k, bool b) {
    const size_t word_idx = k / WORD_BITS;
    if (word_idx >= size_) {
        extend(word_idx + 1);
    }

    Word mask = Word(1) << bit_pos_in_word(k);
    const bool bit = bits_[word_idx] & mask;
    if (b) {
        bits_[word_idx] |= mask;
        true_bits_count_ += !bit;
    }
    else {
        bits_[word_idx] &= ~mask;
        true_bits_count_ -= bit;
    }
}

void bitset::extend(size_t to_size) {
    size_t new_size = std::max(2 * size_, to_size);
    Word* new_bits = new Word[new_size]();
    std::copy(bits_, bits_ + size_, new_bits);
    delete[] bits_;
    bits_ = new_bits;
    size_ = new_size;
}

void bitset::clear() {
    if (bits_ != nullptr) {
        std::fill(bits_, bits_ + size_, Word(0));
    }
    true_bits_count_ = 0;
}

[[nodiscard]] bool bitset::test(size_t k) const {
    if (k >= size()) return false;

    Word mask = Word(1) << bit_pos_in_word(k);
    return bits_[k / WORD_BITS] & mask;
}

[[nodiscard]] bool bitset::empty() const noexcept {
    return true_bits_count_ == 0;
}

[[nodiscard]] bool bitset::operator[](size_t k) const {
    return test(k);
}

[[nodiscard]] bitset bitset::union_with(const bitset& other) const { 
    const auto cmp = [](const bitset& self, const bitset& other) {
        return self.size_ < other.size_;
    };
    const auto& shorter_bitset = std::min(*this, other, cmp);
    const auto& longer_bitset = std::max(*this, other, cmp);

    bitset union_bitset = bitset(longer_bitset.size());
    for (size_t i = 0; i < shorter_bitset.size_; ++i) {
        Word union_word = longer_bitset.bits_[i] | shorter_bitset.bits_[i];
        union_bitset.bits_[i] = union_word;
        union_bitset.true_bits_count_ += std::popcount(union_word);
    }
    for (size_t i = shorter_bitset.size_; i < longer_bitset.size_; ++i) {
        union_bitset.bits_[i] = longer_bitset.bits_[i];
        union_bitset.true_bits_count_ += std::popcount(longer_bitset.bits_[i]);
    }

    return union_bitset;
}

[[nodiscard]] bitset bitset::intersection(const bitset& other) const {
    const auto shorter_size = std::min(size_, other.size_);
    bitset intersection_bitset = bitset(shorter_size * WORD_BITS);
    for (size_t i = 0; i < shorter_size; ++i) {
        Word intersection_word = bits_[i] & other.bits_[i];
        intersection_bitset.bits_[i] = intersection_word;
        intersection_bitset.true_bits_count_ += std::popcount(intersection_word);
    }
    return intersection_bitset;
}

[[nodiscard]] bool bitset::is_subset(const bitset& other) const {
    if (other.true_bits_count_ < true_bits_count_) return false;

    for (size_t i = 0; i < size_; ++i) {
        const Word other_word = i < other.size_ ? other.bits_[i] : Word(0);
        if (bits_[i] & ~other_word) {
            return false;
        }
    }
    return true;
}

