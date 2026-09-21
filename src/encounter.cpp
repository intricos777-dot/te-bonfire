#include "encounter.h"

namespace tebf {

namespace {

const Encounter kEncounters[] = {
    // ----- ashfields -----
    {
        "ashfields",
        {
            // courtyard — tutorial pack of slow hollows
            {"hollow_01", "Ashen Hollow",    "ashfields_courtyard", EnemyKind::Hollow,     40, 40,  6, 100,100, 15, 50,  true, 0.3f,  30,  0, {}},
            {"hollow_02", "Ashen Hollow",    "ashfields_courtyard", EnemyKind::Hollow,     40, 40,  6, 100,100, 15, 50,  true, 0.3f,  30,  0, {}},
            {"hollow_03", "Ashen Hollow",    "ashfields_gate",      EnemyKind::Hollow,     40, 40,  6, 100,100, 15, 55,  true, 0.35f, 30,  0, {}},
            // gate — first lizard, fast
            {"lizard_01", "Salt-Scaled Lizard","ashfields_gate",    EnemyKind::Lizard,     55, 55,  9, 140,140, 10, 65,  true, 0.75f, 60,  5, {}},
            // hollows — mixed pack
            {"hollow_04", "Ashen Hollow",    "ashfields_hollows",   EnemyKind::Hollow,     42, 42,  7, 100,100, 18, 55,  true, 0.4f,  35,  0, {}},
            {"lizard_02", "Salt-Scaled Lizard","ashfields_hollows", EnemyKind::Lizard,     55, 55,  9, 140,140, 10, 65,  true, 0.75f, 60,  5, {}},
            {"hollow_05", "Ashen Hollow",    "ashfields_hollows",   EnemyKind::Hollow,     42, 42,  7, 100,100, 18, 55,  true, 0.4f,  35,  0, {}},
        },
    },

    // ----- blackthorns (endurance gauntlet) -----
    {
        "blackthorns",
        {
            {"bth_knight_01", "Black Knight",     "bth_entrance",  EnemyKind::BlackKnight, 90, 90, 14, 120,120, 40, 60, true, 0.6f, 120, 10, {}},
            {"bth_lizard_01", "Thorn Lizard",     "bth_thornhall", EnemyKind::Lizard,      60, 60, 10, 150,150, 12, 68, true, 0.8f,  80,  5, {}},
            {"bth_lizard_02", "Thorn Lizard",     "bth_thornhall", EnemyKind::Lizard,      60, 60, 10, 150,150, 12, 68, true, 0.8f,  80,  5, {}},
            {"bth_knight_02", "Black Knight",     "bth_thornhall", EnemyKind::BlackKnight, 95, 95, 15, 120,120, 42, 62, true, 0.6f, 130, 10, {}},
            {"bth_witch_01",  "Root Witch",       "bth_roots",     EnemyKind::Witch,       50, 50, 12, 110,110,  8, 70, true, 0.5f, 150, 20, {"titanite_shard"}},
            {"bth_knight_03", "Black Knight",     "bth_cistern",   EnemyKind::BlackKnight,100,100, 16, 130,130, 45, 65, true, 0.65f,140, 15, {}},
            {"bth_sentinel",  "Thorn Sentinel",   "bth_arch",      EnemyKind::Sentinel,   160,160, 20, 140,140, 80, 70, true, 0.7f, 300, 30, {"titanite_shard"}},
        },
    },

    // ----- ember_keep (boss approach) -----
    {
        "ember_keep",
        {
            {"ek_hollow_01",  "Keeper's Hollow", "ek_vestibule", EnemyKind::Hollow,       50, 50,  8, 100,100, 20, 55, true, 0.4f,  45,  0, {}},
            {"ek_guard_01",   "Bell Guard",      "ek_hall",      EnemyKind::BellGuard,     70, 70, 12, 130,130, 35, 60, true, 0.65f, 90,  0, {}},
            {"ek_hollow_02",  "Keeper's Hollow", "ek_hall",      EnemyKind::Hollow,       50, 50,  8, 100,100, 20, 55, true, 0.4f,  45,  0, {}},
            {"ek_guard_02",   "Bell Guard",      "ek_hall",      EnemyKind::BellGuard,     70, 70, 12, 130,130, 35, 60, true, 0.65f, 90,  0, {}},
            {"ek_witch_01",   "Pyre Witch",      "ek_hall",      EnemyKind::Witch,        55, 55, 14, 110,110,  8, 72, true, 0.55f,160, 20, {}},
            // boss arena is populated only by the boss loop (handled in boss.cpp)
        },
    },

    // ----- salt_marches -----
    {
        "salt_marches",
        {
            {"sm_knight_01", "Salt Knight",      "sm_flats",  EnemyKind::BlackKnight, 105,105, 17, 130,130, 48, 68, true, 0.7f, 160, 15, {}},
            {"sm_witch_01",  "Brine Witch",      "sm_crags",  EnemyKind::Witch,        60, 60, 16, 120,120, 10, 75, true, 0.6f, 180, 25, {"large_titanite_shard"}},
            {"sm_lizard_01", "Salt Lizard",      "sm_crags",  EnemyKind::Lizard,       70, 70, 12, 160,160, 14, 72, true, 0.85f,100, 10, {}},
            {"sm_knight_02", "Salt Knight",      "sm_brine",  EnemyKind::BlackKnight, 110,110, 18, 130,130, 50, 70, true, 0.75f, 170, 20, {}},
            {"sm_sentinel",  "Brine Sentinel",   "sm_brine",  EnemyKind::Sentinel,    200,200, 24, 150,150, 90, 75, true, 0.8f, 400, 40, {"large_titanite_shard"}},
            {"sm_guard_01",  "Brine Guard",      "sm_tower",  EnemyKind::BellGuard,     80, 80, 14, 140,140, 38, 65, true, 0.7f, 120,  0, {}},
        },
    },

    // ----- bell_gate -----
    {
        "bell_gate",
        {
            {"bg_knight_01", "Gate Knight",     "bg_approach",EnemyKind::BlackKnight, 115,115, 19, 140,140, 52, 72, true, 0.75f, 180, 20, {}},
            {"bg_knight_02", "Gate Knight",     "bg_approach",EnemyKind::BlackKnight, 115,115, 19, 140,140, 52, 72, true, 0.75f, 180, 20, {}},
            {"bg_witch_01",  "Bell Witch",      "bg_gallery", EnemyKind::Witch,        70, 70, 18, 130,130, 12, 78, true, 0.65f, 220, 30, {"large_titanite_shard"}},
            {"bg_sentinel",  "Gallery Sentinel", "bg_gallery", EnemyKind::Sentinel,    240,240, 28, 160,160,100, 78, true, 0.85f, 500, 50, {"titanite_chunk"}},
            {"bg_knight_03", "Gate Knight",     "bg_belfry",  EnemyKind::BlackKnight, 120,120, 20, 140,140, 55, 75, true, 0.8f,  200, 25, {}},
            {"bg_guard_01",  "Last Bell Guard", "bg_belfry",  EnemyKind::BellGuard,     90, 90, 16, 150,150, 42, 70, true, 0.75f, 150,  0, {}},
        },
    },
};

constexpr size_t kCount = sizeof(kEncounters) / sizeof(kEncounters[0]);

} // namespace

const Encounter* encounter_for_zone(const std::string& zone_id) {
    for (size_t i = 0; i < kCount; ++i)
        if (kEncounters[i].zone_id == zone_id) return &kEncounters[i];
    return nullptr;
}

size_t encounter_count() { return kCount; }

const Encounter* encounter(size_t index) {
    return index < kCount ? &kEncounters[index] : nullptr;
}

} // namespace tebf
