#pragma once
#include <cstdint>
#include <string>

namespace tebf {

// Difficulty director: shapes every enemy/AI behavior in the game.
enum class Difficulty {
    Casual,   // clunky fighters: meaningful miss chance, slow reactions, small openings
    Legend,   // full AI within its skill budget: no mercy, on-time reactions
};

struct DifficultyProfile {
    // hit chance in percent
    uint32_t enemy_hit_chance;
    // reaction delay in ticks before an enemy commits to an attack
    uint32_t enemy_reaction_ticks;
    // fraction of max damage an enemy deals outright
    float damage_scale;
    // player stamina drain multiplier (casual easier)
    float stamina_scale;
    // exp multiplier
    float xp_scale;
};

inline DifficultyProfile profile_for(Difficulty d) {
    switch (d) {
        case Difficulty::Casual:
            return {55u, 3u, 0.5f, 0.75f, 1.25f};
        case Difficulty::Legend:
            return {95u, 0u, 1.0f, 1.0f, 0.85f};
    }
    return {70u, 1u, 0.85f, 0.9f, 1.0f};
}

inline const char* name_of(Difficulty d) {
    return d == Difficulty::Casual ? "CASUAL" : "LEGEND";
}

} // namespace tebf