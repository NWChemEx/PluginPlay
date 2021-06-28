#pragma once
#include <array>

namespace sde_examples {

using Point = std::array<double, 3>;

struct PointCharge {
    double m_charge;
    Point m_r;
};

} // namespace sde_examples

// These are down here to get this to compile (objects appearing in APIs need to
// be hashable and comparable) without complicating the tutorial
#include <sde/sde.hpp>

namespace sde_examples {
inline bool operator==(const sde_examples::PointCharge& q0,
                       const sde_examples::PointCharge& q1) {
    return (q0.m_charge == q1.m_charge) && (q0.m_r == q1.m_r);
}

inline void hash_object(const sde_examples::PointCharge& q,
                        runtime::Hasher& h) {
    h(q.m_charge, q.m_r);
}
} // namespace sde_examples