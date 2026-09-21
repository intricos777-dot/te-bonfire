#pragma once
#include <string>
#include <vector>

namespace tebf {

// Souls-like leveling: spending "souls" at a bonfire to raise stats.
struct PlayerVitals {
    int level = 1;
    int souls = 0;                  // current currency
    int souls_spent = 0;            // total spent (for tracking)

    int vit = 10;                   // vigor     -> HP
    int end = 10;                   // endurance -> stamina
    int str = 10;                   // strength  -> melee damage
    int dex = 10;                   // dexterity -> speed/crit

    int hp() const { return 50 + (vit - 10) * 10; }
    int stamina_max() const { return 80 + (end - 10) * 8; }
    int melee_power() const { return (str - 10) * 2; }
    int crit_bonus() const { return (dex - 10) * 1; }

    // Souls required to raise a stat to its next level.
    // Classic souls-like curve: cost scales with target level.
    static int level_cost(int current_stat_value);
};

// Attempt to spend souls to raise a stat. Returns true on success.
// Stat index: 0=vit, 1=end, 2=str, 3=dex
bool spend_souls(PlayerVitals& v, int stat_index);

// Soul economy: compute drop based on enemy + difficulty.
int soul_drop(int base_souls, float xp_scale);

} // namespace tebf
