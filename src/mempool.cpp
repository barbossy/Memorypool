#include "mempool.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <thread>

Mempool::Mempool(MempoolConfig config, std::size_t alignment)
{
    std::size_t totalSize = calcTotalSize(config);
    assert(verifyChunkSizes(config, alignment) && totalSize <= maxSize_);

    totalSize = (totalSize + alignment - 1) & ~(alignment - 1);
    pMemoryPool_ = (char*)aligned_alloc(alignment, totalSize);
    assert(pMemoryPool_);

    initStats(config);

    auto pMemoryPool = pMemoryPool_;
    for (const auto& configElement : config)
    {
        for (int32_t no = 0; no < configElement.amount_; ++no)
        {
            subPools_[configElement.size_].push_back(pMemoryPool);
            pMemoryPool += configElement.size_;
        }
    }
}

Mempool::~Mempool()
{
    for (const auto& [size, statistic] : stats_)
    {
        std::cout << "Subpool[" << size << "]: amout of unfreed chunks: " << statistic.count_
                  << ", peak usage: " << statistic.peak_ << std::endl;
    }
    free(pMemoryPool_);
}

void* Mempool::alloc(std::size_t size)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (subPools_.empty() || (--subPools_.cend()) -> first < size)
    {
        return nullptr;
    }
    auto subPoolIt = subPools_.lower_bound(size);
    for (auto it = subPoolIt; it != subPools_.cend(); ++it)
    {
        if (!(it -> second.empty()))
        {
            void* ptr = it -> second.back();
            it -> second.pop_back();
            stats_[it -> first].peak_ = std::max(stats_[it -> first].peak_,
                                                 ++stats_[it -> first].count_);
            allocs_[ptr] = it -> first;
            return ptr;
        }
    }
    return nullptr;
}

void Mempool::free(void* ptr)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (allocs_.count(ptr) == 0)
    {
        return;
    }
    subPools_[allocs_[ptr]].push_back(ptr);
    --stats_[allocs_[ptr]].count_;
    allocs_.erase(ptr);
}

std::size_t Mempool::calcTotalSize(const MempoolConfig& config)
{
    return std::accumulate(config.cbegin(), config.cend(), 0,
                           [](std::size_t totalSize, const SubPoolDescriptor& configElement)
    {
        return totalSize + configElement.size_ * configElement.amount_;
    });
}

void Mempool::initStats(const MempoolConfig& config)
{
    for (auto const& configElement : config)
    {
        stats_[configElement.size_];
    }
}

bool Mempool::verifyChunkSizes(const MempoolConfig& config, const std::size_t& alignment)
{
    return std::all_of(config.cbegin(), config.cend(),
                       [alignment](const SubPoolDescriptor& subPool)
    {
        return alignment % subPool.size_ == 0;
    });
}
