#include <cstddef>
#include <gtest/gtest.h>

#include "bitset.h"

TEST(TestBasicOperations, TestGetSetBasic) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);

    const size_t iter_idx = 33;
    for (size_t i = 0; i < iter_idx; ++i) {
        bs.set(i, true);
    }
    for (size_t i = 0; i < iter_idx; ++i) {
        ASSERT_TRUE(bs.test(i));
        ASSERT_TRUE(bs[i]);
    }
}

TEST(TestBasicOperations, TestGetOutOfBounds) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);

    for (size_t i = 0; i < bs.size(); ++i) {
        bs.set(i, true);
    }
    ASSERT_FALSE(bs.test(bs.size() + 1));
}

TEST(TestBasicOperations, TestSetExtendsBitset) {
    const size_t initial_capacity = 42;
    const size_t ext_capacity = 2 * initial_capacity;
    bitset bs(initial_capacity);
    const size_t initial_size = bs.size();

    for (size_t i = 0; i < ext_capacity; ++i) {
        bs.set(i, true);
    }
    ASSERT_GT(bs.size(), initial_size);

    for (size_t i = 0; i < ext_capacity; ++i) {
        ASSERT_TRUE(bs.test(i));
    }
}

TEST(TestBasicOperations, TestClear) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);
    const size_t init_size = bs.size();
    for (size_t i = 0; i < initial_capacity; ++i) {
        bs.set(i, true);
    }
    bs.clear();
    for (size_t i = 0; i < bs.size(); ++i) {
        ASSERT_FALSE(bs.test(i));
    }
}

TEST(TestBasicOperations, TestEmpty) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);
    ASSERT_TRUE(bs.empty());

    for (size_t i = 0; i < initial_capacity; ++i) {
        bs.set(i, true);
    }
    ASSERT_FALSE(bs.empty());
    for (size_t i = 0; i < initial_capacity; ++i) {
        bs.set(i, false);
    }
    ASSERT_TRUE(bs.empty());

    bs.clear();
    ASSERT_TRUE(bs.empty());
}

TEST(TestBasicOperations, TestUnionWith) {
    const size_t initial_capacity1 = 42;
    bitset bs1(initial_capacity1);
    
    for (size_t i = 0; i < initial_capacity1; i += 2) {
        bs1.set(i, true);
    }

    const size_t initial_capacity2 = 84;
    bitset bs2(initial_capacity2);

    for (size_t i = 1; i < initial_capacity1; i += 2) {
        bs2.set(i, true);
    }

    
    bitset bs3 = bs1.union_with(bs2);
    ASSERT_EQ(bs3.size(), std::max(bs1.size(), bs2.size()));

    for (size_t i = 0; i < initial_capacity1; ++i) {
        ASSERT_TRUE(bs3.test(i));
    }
}

TEST(TestBasicOperations, TestIntersection) {
    const size_t initial_capacity1 = 42;
    bitset bs1(initial_capacity1);

    const size_t fill_until = 16;
    for (size_t i = 0; i < fill_until; ++i) {
        bs1.set(i, true);
    }

    const size_t initial_capacity2 = 84;
    bitset bs2(initial_capacity2);
    for (size_t i = 0; i < fill_until; i += 2) {
        bs2.set(i, true);
    }

    bitset bs3 = bs1.intersection(bs2);
    ASSERT_EQ(bs3.size(), std::min(bs1.size(), bs2.size()));

    for (size_t i = 0; i < fill_until; ++i) {
        ASSERT_EQ(bs3.test(i), i % 2 == 0);
    }

}

TEST(TestBasicOperations, TestIsSubset) {
    const size_t initial_capacity1 = 42;
    bitset bs1(initial_capacity1);
    for (size_t i = 0; i < initial_capacity1; ++i) {
        bs1.set(i, true);    
    }

    const size_t initial_capacity2 = 84;
    bitset bs2(initial_capacity2);
    for (size_t i = 0; i < initial_capacity2; ++i) {
        bs2.set(i, true);    
    }
    ASSERT_TRUE(bs1.is_subset(bs2));
    ASSERT_FALSE(bs2.is_subset(bs1));

    bs2.clear();
    for (size_t i = 0; i < initial_capacity1 - 1; ++i) {
        bs2.set(i, true);
    }
    ASSERT_TRUE(bs2.is_subset(bs1));
}

TEST(TestMemory, TestCopy) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);
    bs.set(5, true);
    bs.set(8, true);
    
    bitset bs_copy(bs);
    ASSERT_EQ(bs_copy.size(), bs.size());
    for (size_t i = 0; i < bs.size(); ++i) {
        ASSERT_EQ(bs.test(i), bs_copy.test(i));
    }
    bs_copy.set(5, false);
    ASSERT_TRUE(bs.test(5));
}

TEST(TestMemory, TestMove) {
    const size_t initial_capacity = 42;
    bitset bs(initial_capacity);
    bs.set(5, true);
    bs.set(8, true);
    
    bitset bs_moved = std::move(bs);
    ASSERT_TRUE(bs_moved.test(5) && bs_moved.test(8));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
