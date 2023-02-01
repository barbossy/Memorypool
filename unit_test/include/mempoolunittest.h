#ifndef MEMPOOLUNITTEST_H
#define MEMPOOLUNITTEST_H

#include "mempool.h"

class MempoolUnitTest : public Mempool
{
public:
    MempoolUnitTest() = delete;
    MempoolUnitTest(const MempoolUnitTest&) = delete;
    MempoolUnitTest(MempoolUnitTest&&) = delete;
    MempoolUnitTest& operator=(const MempoolUnitTest&) = delete;
    MempoolUnitTest& operator=(MempoolUnitTest&&) = delete;

    MempoolUnitTest(MempoolConfig config, std::size_t alignment)
        : Mempool(config, alignment) {}

    virtual ~MempoolUnitTest() = default;

    inline auto getSubPools() const { return subPools_; }
    inline auto getStats() const { return stats_; }
};

#endif // MEMPOOLUNITTESTT_H
