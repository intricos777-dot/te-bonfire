#include "hidden_boss.h"

namespace tebf {

namespace {
const HiddenBoss kHidden[] = {
    // --- THE FALLEN CANDLEBEARER ---
    {
        .id = "fallen_candlebearer",
        .name = "Candlebearer of the Gutted Flame",
        .epithet = "It carries the last light and has forgotten why.",
        .unlock_hint = "Die twenty-five times. The dark keeps a tally.",
        .music = "boss_candlebearer",
        .hp = 1400,
        .damage = 62,
        .poise = 90,
        .hit_chance = 82,
        .uses_guard_break = true,
    },
    // --- THE UNMIRRORED ---
    {
        .id = "the_unmirrored",
        .name = "The Unmirrored",
        .epithet = "A knight who sees every swing three frames early.",
        .unlock_hint = "Beat the tutorial boss on Legend difficulty.",
        .music = "boss_unmirrored",
        .hp = 1800,
        .damage = 84,
        .poise = 90,
        .hit_chance = 96,
        .uses_guard_break = true,
    },
    // --- THE SELF-SEALED ---
    {
        .id = "the_self_sealed",
        .name = "The Self-Sealed",
        .epithet = "Bound by its own chains. It moves when you heal.",
        .unlock_hint = "Light every bonfire in the world, then rest at the first.",
        .music = "boss_self_sealed",
        .hp = 2400,
        .damage = 110,
        .poise = 120,
        .hit_chance = 92,
        .uses_guard_break = true,
    },
};
constexpr size_t kCount = 3;
}

const HiddenBoss* hidden_boss(size_t index) {
    return index < kCount ? &kHidden[index] : nullptr;
}

const HiddenBoss* find_hidden_boss(const std::string& id) {
    for (size_t i = 0; i < kCount; ++i)
        if (kHidden[i].id == id) return &kHidden[i];
    return nullptr;
}

size_t hidden_boss_count() {
    return kCount;
}

} // namespace tebf