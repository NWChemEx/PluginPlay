#pragma once
#include <array>

namespace pluginplay_examples {

using Point = std::array<double, 3>;

struct PointCharge {
    double m_charge;
    Point m_r;
};

} // namespace pluginplay_examples

// These are down here to get this to compile (objects appearing in APIs need to
// be hashable and comparable) without complicating the tutorial
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {
inline bool operator==(const pluginplay_examples::PointCharge& q0,
                       const pluginplay_examples::PointCharge& q1) {
    return (q0.m_charge == q1.m_charge) && (q0.m_r == q1.m_r);
}

inline void hash_object(const pluginplay_examples::PointCharge& q,
                        pluginplay::Hasher& h) {
    h(q.m_charge, q.m_r);
}
} // namespace pluginplay_examples
