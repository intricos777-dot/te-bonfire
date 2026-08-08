#pragma once
#include <cstdint>
#include <string>

namespace tebf {

// The three hidden almost-unbeatable bosses. Each has a strict unlock
// condition, a huge stat block, and a distinct music mix clip.
struct HiddenBoss {
    std::string id;
    std::string name;
    std::string epithet;
    // how the player unlocks the fight
    std::string unlock_hint;
    // music clip id (mixed from living_sin desktop tracks)
    std::string music;
    int hp;
    int damage;
    int poise;
    int hit_chance;      // overridden by Difficulty director
    bool uses_guard_break;
};

// Registry of the three secret fights.
const HiddenBoss* hidden_boss(size_t index);   // 0..2, nullptr if out of range
const HiddenBoss* find_hidden_boss(const std::string& id);
size_t hidden_boss_count();

// Hidden-boss unlock states (persisted across runs in data/progress.json).
struct HiddenUnlocks {
    bool beaten_any_boss = false;   // requires finishing the tutorial boss
    int deaths = 0;                 // "die 25 times" - devotion to the dark
    int bonfires_lit = 0;           // "light every checkpoint"
};

} // namespace tebf