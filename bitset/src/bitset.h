#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <climits>

class bitset {
    private:

    using Word = uint64_t;
    static constexpr size_t WORD_BITS = sizeof(Word) * CHAR_BIT; 
    
    [[nodiscard]] static constexpr size_t words_for_bits(size_t bit_count) noexcept {
        return (bit_count + WORD_BITS - 1) / WORD_BITS;
    }
    
    [[nodiscard]] static constexpr size_t bit_pos_in_word(size_t bit_pos) noexcept {
        return WORD_BITS - 1 - (bit_pos % WORD_BITS);
    }
    
    std::vector<Word> bits_;
    size_t true_bits_count_;
    
    public:

        bitset();
        explicit bitset(size_t initial_capacity);

        void set(size_t k, bool b);
        [[nodiscard]] bool test(size_t k) const noexcept;
        [[nodiscard]] bool operator[](size_t k) const noexcept;

        [[nodiscard]] bitset union_with(const bitset& other) const;
        [[nodiscard]] bitset intersection(const bitset& other) const;
        [[nodiscard]] bool is_subset(const bitset& other) const;

        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] bool empty() const noexcept;
        void clear();
};
