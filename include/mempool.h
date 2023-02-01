#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "subpooldescriptor.h"
#include "statistic.h"

#include <cstdlib>
#include <map>
#include <mutex>
#include <vector>

typedef std::vector<SubPoolDescriptor> MempoolConfig;

class Mempool
{
public:
    Mempool() = delete;
    Mempool(const Mempool&) = delete;
    Mempool(Mempool&&) = delete;
    Mempool& operator=(const Mempool&) = delete;
    Mempool& operator=(Mempool&&) = delete;

    Mempool(MempoolConfig config, std::size_t alignment);
    virtual ~Mempool();

    void* alloc(std::size_t size);
    void free(void* ptr);

protected:
    std::map<std::size_t, std::vector<void*>> subPools_;
    std::map<void*, std::size_t> allocs_;
    std::map<std::size_t, Statistic> stats_;

private:
    static constexpr std::size_t maxSize_ = 2048;
    char* pMemoryPool_;
    std::mutex mtx_;

    std::size_t calcTotalSize(const MempoolConfig& config);
    void initStats(const MempoolConfig& config);
    bool verifyChunkSizes(const MempoolConfig& config, const std::size_t& alignment);
};

#endif // MEMPOOL_H
