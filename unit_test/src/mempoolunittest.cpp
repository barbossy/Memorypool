#include "mempoolunittest.h"

#include <gtest/gtest.h>
#include <numeric>

TEST(memPoolTest, invalidChunkSizes)
{
    MempoolConfig config{{20, 8}, {15, 16}, {10, 21}, {5, 32}};
    ASSERT_DEATH({ MempoolUnitTest pool(config, 256); }, "");
}

TEST(memPoolTest, invalidTotalSize)
{
    MempoolConfig config{{1000, 8}};
    ASSERT_DEATH({ MempoolUnitTest pool(config, 256); }, "");
}
TEST(memPoolTest, allocSizeBiggerThanBiggestChunk)
{
    MempoolConfig config{{20, 8}, {15, 16}, {10, 32}};
    MempoolUnitTest pool{config, 2048};

    const std::size_t invalidSize = (config.cend() -> size_) + 1;

    void *ptr = pool.alloc(invalidSize);
    EXPECT_EQ(ptr, nullptr);
}
TEST(memPoolTest, allocValidSizeAndFreeValidPtr)
{
    const MempoolConfig config{{10, 32}, {15, 16}, {20, 8}};
    MempoolUnitTest pool{config, 2048};

    const std::size_t validSize = config[1].size_;
    constexpr std::size_t expectedSubPoolSizeAfterAlloc = 14;
    constexpr std::size_t expectedSubPoolSizeAfterFree = 15;

    void* ptr = pool.alloc(validSize);
    EXPECT_EQ((pool.getSubPools()[validSize]).size(), expectedSubPoolSizeAfterAlloc);

    pool.free(ptr);
    EXPECT_EQ((pool.getSubPools()[validSize]).size(), expectedSubPoolSizeAfterFree);
}

TEST(memPoolTest, allocAndFreeEntireSubPool)
{
    const MempoolConfig config{{10, 32}, {15, 16}, {20, 8}};
    MempoolUnitTest pool{config, 2048};

    const int32_t noOfAlloc = config[1].amount_ + 1;
    const std::size_t validSize = config[1].size_;
    constexpr std::size_t expectedSubPoolSize16AfterAlloc = 0;
    constexpr std::size_t expectedSubPoolSize32AfterAlloc = 9;
    constexpr std::size_t expectedSubPoolSize16AfterFree = 15;
    constexpr std::size_t expectedSubPoolSize32AfterFree = 10;

    void* allocs[noOfAlloc];
    for (int idx = 0; idx < noOfAlloc; ++idx)
    {
        allocs[idx] = pool.alloc(validSize);
    }

    EXPECT_EQ((pool.getSubPools()[validSize]).size(), expectedSubPoolSize16AfterAlloc);
    // When a given subpool is empty, the next subpool is used.
    EXPECT_EQ((pool.getSubPools()[32]).size(), expectedSubPoolSize32AfterAlloc);

    for (int idx = 0; idx < noOfAlloc; ++idx)
    {
        pool.free(allocs[idx]);
    }
    EXPECT_EQ((pool.getSubPools()[validSize]).size(), expectedSubPoolSize16AfterFree);
    EXPECT_EQ((pool.getSubPools()[32]).size(), expectedSubPoolSize32AfterFree);
}

TEST(memPoolTest, allocAndFreeEntirePool)
{
    MempoolConfig config{{20, 8}, {15, 16}, {10, 32}};
    MempoolUnitTest pool{config, 2048};

    const int32_t noOfAlloc = std::accumulate(config.cbegin(), config.cend(), 0,
                                              [](std::size_t totalSize, const SubPoolDescriptor& configElement)
                              {
                                  return totalSize + configElement.size_ * configElement.amount_;
                              });

    const std::size_t validSize = config[1].size_;

    void* allocs[noOfAlloc + 1];
    int idx = 0;
    for (auto const& [amount, size] : config)
    {
        for (int32_t no = 0; no < amount; ++no)
        {
            allocs[idx++] = pool.alloc(size);
        }
    }
    // Allocate in the empty pool, should return nullptr.
    allocs[idx] = pool.alloc(validSize);
    EXPECT_EQ(allocs[idx], nullptr);

    for (auto const& [amount, size] : config)
    {
        EXPECT_TRUE((pool.getSubPools()[size]).empty());
    }

    for (int idx = 0; idx < noOfAlloc; ++idx)
    {
        pool.free(allocs[idx]);
    }

    for (auto const& [amount, size] : config)
    {
        EXPECT_EQ((pool.getSubPools()[size]).size(), amount);
    }
}

TEST(memPoolTest, freeInvalidPtr)
{
    MempoolConfig config{{20, 8}, {15, 16}, {10, 32}};
    MempoolUnitTest pool{config, 2048};

    char* invalidPtr = new char();

    pool.free(invalidPtr);
    for (auto const& [amount, size] : config)
    {
        EXPECT_EQ((pool.getSubPools()[size]).size(), amount);
    }
}

TEST(memPoolTest, verifyStatistic)
{
    MempoolConfig config{{20, 8}, {15, 16}, {10, 32}};
    MempoolUnitTest pool{config, 2048};

    const int32_t noOfAlloc = config[2].amount_;
    const std::size_t validSize = config[2].size_;
    const std::map<int32_t, Statistic> expectedStats{
        {config[0].size_, {0, 0}},
        {config[1].size_, {0, 0}},
        {config[2].size_, {3, 8}}
    };

    // Example series of allocation and deallocation to collect statistics.
    void* allocs[noOfAlloc];
    for (int idx = 0; idx < 5; ++idx)
    {
        allocs[idx] = pool.alloc(validSize);
    }
    for (int idx = 0; idx < 2; ++idx)
    {
        pool.free(allocs[idx]);
    }
    for (int idx = 5; idx < 10; ++idx)
    {
        allocs[idx] = pool.alloc(validSize);
    }
    for (int idx = 5; idx < 10; ++idx)
    {
        pool.free(allocs[idx]);
    }

    for (auto [size, statistic] : pool.getStats())
    {
        EXPECT_EQ(statistic.count_, expectedStats.at(size).count_);
    }
}
