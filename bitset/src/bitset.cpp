#include "bitset.h"
#include <algorithm>
#include <bit>

bitset::bitset() : bits_(0), true_bits_count_(0) {}

bitset::bitset(size_t initial_capacity) : 
    bits_(words_for_bits(initial_capacity), Word(0)),
    true_bits_count_(0) {}

[[nodiscard]] size_t bitset::size() const noexcept {
    return bits_.size() * WORD_BITS;
}

void bitset::set(size_t k, bool b) {
    const size_t word_idx = k / WORD_BITS;
    if (word_idx >= bits_.size()) {
        bits_.resize(word_idx + 1);
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

void bitset::clear() {
    std::fill(bits_.begin(), bits_.end(), Word(0));
    true_bits_count_ = 0;
}

[[nodiscard]] bool bitset::test(size_t k) const noexcept {
    if (k >= size()) return false;

    Word mask = Word(1) << bit_pos_in_word(k);
    return bits_[k / WORD_BITS] & mask;
}

[[nodiscard]] bool bitset::empty() const noexcept {
    return true_bits_count_ == 0;
}

[[nodiscard]] bool bitset::operator[](size_t k) const noexcept {
    return test(k);
}

[[nodiscard]] bitset bitset::union_with(const bitset& other) const { 
    const auto cmp = [](const bitset& self, const bitset& other) {
        return self.bits_.size() < other.bits_.size();
    };
    const auto& shorter_bitset = std::min(*this, other, cmp);
    const auto& longer_bitset = std::max(*this, other, cmp);

    bitset union_bitset = bitset(longer_bitset.size());
    for (size_t i = 0; i < shorter_bitset.bits_.size(); ++i) {
        Word union_word = longer_bitset.bits_[i] | shorter_bitset.bits_[i];
        union_bitset.bits_[i] = union_word;
        union_bitset.true_bits_count_ += std::popcount(union_word);
    }
    for (size_t i = shorter_bitset.bits_.size(); i < longer_bitset.bits_.size(); ++i) {
        union_bitset.bits_[i] = longer_bitset.bits_[i];
        union_bitset.true_bits_count_ += std::popcount(longer_bitset.bits_[i]);
    }

    return union_bitset;
}

[[nodiscard]] bitset bitset::intersection(const bitset& other) const {
    const auto shorter_size = std::min(bits_.size(), other.bits_.size());
    bitset intersection_bitset = bitset(shorter_size * WORD_BITS);
    for (size_t i = 0; i < shorter_size; ++i) {
        Word intersection_word = bits_[i] & other.bits_[i];
        intersection_bitset.bits_[i] = intersection_word;
        intersection_bitset.true_bits_count_ += std::popcount(intersection_word);
    }
    return intersection_bitset;
}

[[nodiscard]] bool bitset::is_subset(const bitset& other) const {
    if (other.size() < size()) return false;
    
    for (size_t i = 0; i < bits_.size(); ++i) {
        if (bits_[i] & ~other.bits_[i]) {
            return false;
        }
    }
    return true;
}

