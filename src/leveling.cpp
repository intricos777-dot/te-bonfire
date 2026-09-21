#include "leveling.h"

namespace tebf {

namespace {

// Souls-like level cost curve: each stat level costs roughly
// (target^1.5) * 10, rounded. This rewards early levels and punishes min-maxing.
int level_cost(int current) {
    if (current < 1) current = 1;
    // We approximate (int)(10 * current^1.5) without <cmath> pow drift.
    float v = static_cast<float>(current);
    int cost = static_cast<int>(10.0f * v * (v <= 1.0f ? 1.0f : (v <= 4.0f ? 1.5f : (v <= 9.0f ? 2.0f : (v <= 16.0f ? 2.5f : (v <= 25.0f ? 3.0f : 3.5f))))));
    // fallback: if still 0, base it on v
    if (cost == 0) cost = static_cast<int>(10.0f * v * 1.5f);
    return cost;
}

} // namespace

int PlayerVitals::level_cost(int current_stat_value) {
    return tebf::level_cost(current_stat_value);
}

bool spend_souls(PlayerVitals& v, int stat_index) {
    int current = 0;
    switch (stat_index) {
        case 0: current = v.vit; break;
        case 1: current = v.end; break;
        case 2: current = v.str; break;
        case 3: current = v.dex; break;
        default: return false;
    }
    int cost = level_cost(current + 1);
    if (v.souls < cost) return false;
    v.souls -= cost;
    v.souls_spent += cost;
    switch (stat_index) {
        case 0: v.vit += 1; break;
        case 1: v.end += 1; break;
        case 2: v.str += 1; break;
        case 3: v.dex += 1; break;
    }
    v.level += 1;
    return true;
}

int soul_drop(int base_souls, float xp_scale) {
    return static_cast<int>(static_cast<float>(base_souls) * xp_scale);
}

} // namespace tebf
