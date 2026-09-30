#include "ofcl/lob/lockfree_book.hpp"

namespace ofcl {

void LockFreeBookSide::publish_level(std::size_t idx, Price px, Quantity qty) noexcept {
    if (idx >= kMaxLevels) return;
    auto& lvl = levels_[idx];
    // Simple sequential consistency publish (research prototype)
    lvl.price.store(px, std::memory_order_relaxed);
    lvl.quantity.store(qty, std::memory_order_relaxed);
    lvl.version.fetch_add(1, std::memory_order_release);
}

bool LockFreeBookSide::read_level(std::size_t idx, Price& px, Quantity& qty) const noexcept {
    if (idx >= kMaxLevels) return false;
    const auto& lvl = levels_[idx];
    std::uint64_t v1 = lvl.version.load(std::memory_order_acquire);
    px = lvl.price.load(std::memory_order_relaxed);
    qty = lvl.quantity.load(std::memory_order_relaxed);
    std::uint64_t v2 = lvl.version.load(std::memory_order_acquire);
    return v1 == v2;  // consistent snapshot
}

}  // namespace ofcl
