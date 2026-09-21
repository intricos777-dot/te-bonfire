#include "world_geometry.h"

namespace tebf {

namespace {

const WorldGeometry kGeometries[] = {
    // ----- ashfields (tutorial) -----
    {
        "ashfields",
        {
            {"ashfields_courtyard", "Courtyard of Ashes",  0.0f,  0.0f, 12.0f, 8.0f,  true,  true,  false},
            {"ashfields_gate",      "Sunken Gate",        30.0f,  0.0f, 10.0f, 6.0f,  false, false, false},
            {"ashfields_hollows",   "Hollow Tunnels",     60.0f, -5.0f, 14.0f, 7.0f,  false, false, false},
            {"ashfields_pyre",      "First Pyre",         95.0f,  0.0f,  8.0f, 5.0f,  false, false, true},
        },
        {
            {"ashfields_courtyard", "ashfields_gate",      22.0f, false},
            {"ashfields_gate",      "ashfields_hollows",   28.0f, false},
            {"ashfields_hollows",   "ashfields_pyre",      18.0f, false},
        },
        {
            {"bonfire_ashfields",   "Courtyard Bonfire",   "ashfields_courtyard", Interactable::Type::Bonfire, false, ""},
            {"item_estus_shard",    "Estus Shard",         "ashfields_gate",      Interactable::Type::Item,      true,  "estus_shard"},
            {"lever_gate_south",    "Rusted Lever",        "ashfields_hollows",   Interactable::Type::Lever,     true,  "door_south"},
            {"lore_first_ashen",    "Ashen Tablet",        "ashfields_courtyard", Interactable::Type::Lore,      false, "lore_ashfields_1"},
        },
    },

    // ----- blackthorns (endurance gauntlet) -----
    {
        "blackthorns",
        {
            {"bth_entrance",    "Blackthorn Gate",      0.0f,   0.0f, 10.0f, 6.0f,  true,  false, false},
            {"bth_thornhall",   "Thornhall",           32.0f,   4.0f, 16.0f, 10.0f, false, false, false},
            {"bth_roots",       "Root Cellars",        64.0f,  -8.0f, 12.0f, 8.0f,  false, false, false},
            {"bth_cistern",     "Dry Cistern",         96.0f,   0.0f, 10.0f, 10.0f, false, true,  false},
            {"bth_arch",        "Archdruid's Approach",128.0f,  0.0f,  8.0f, 5.0f,  false, false, true},
        },
        {
            {"bth_entrance",  "bth_thornhall", 24.0f, false},
            {"bth_thornhall", "bth_roots",     26.0f, false},
            {"bth_roots",     "bth_cistern",   22.0f, false},
            {"bth_cistern",   "bth_arch",      24.0f, false},
        },
        {
            {"bonfire_bth_cistern", "Cistern Bonfire",     "bth_cistern", Interactable::Type::Bonfire, false, ""},
            {"item_titanite_1",     "Titanite Shard",      "bth_thornhall",Interactable::Type::Item,      true,  "titanite_shard"},
            {"item_titanite_2",     "Titanite Shard",      "bth_roots",    Interactable::Type::Item,      true,  "titanite_shard"},
            {"lever_cistern",       "Cistern Lever",       "bth_roots",    Interactable::Type::Lever,     true,  "door_arch"},
        },
    },

    // ----- ember_keep (first boss) -----
    {
        "ember_keep",
        {
            {"ek_vestibule",   "Vestibule of Embers",  0.0f,  0.0f, 12.0f, 8.0f,  true,  false, false},
            {"ek_hall",        "Hall of the Watcher",  36.0f,  0.0f, 20.0f, 14.0f,false, false, false},
            {"ek_arena",       "Ember Arena",          76.0f,  0.0f, 18.0f, 12.0f,false, false, true},
        },
        {
            {"ek_vestibule", "ek_hall",  28.0f, false},
            {"ek_hall",      "ek_arena", 32.0f, false},
        },
        {
            {"bonfire_ek_vestibule", "Vestibule Bonfire", "ek_vestibule", Interactable::Type::Bonfire, false, ""},
            {"item_ember",           "Kindled Ember",     "ek_hall",      Interactable::Type::Item,     true,  "kindled_ember"},
            {"lore_watcher",         "Watcher's Tablet",  "ek_hall",      Interactable::Type::Lore,     false, "lore_watcher_1"},
        },
    },

    // ----- salt_marches (opened by Ember Keep) -----
    {
        "salt_marches",
        {
            {"sm_flats",      "Salt Flats",          0.0f,   0.0f, 14.0f, 10.0f, true,  false, false},
            {"sm_crags",      "Cragspire",          38.0f,  -6.0f, 12.0f,  8.0f, false, false, false},
            {"sm_brine",      "Brine Hollow",       72.0f,   4.0f, 16.0f, 10.0f, false, true,  false},
            {"sm_tower",      "Sunken Bell Tower",  108.0f,  0.0f, 10.0f,  6.0f, false, false, true},
        },
        {
            {"sm_flats", "sm_crags", 28.0f, false},
            {"sm_crags", "sm_brine", 30.0f, false},
            {"sm_brine", "sm_tower", 26.0f, false},
        },
        {
            {"bonfire_sm_brine", "Brine Bonfire",     "sm_brine", Interactable::Type::Bonfire, false, ""},
            {"item_titanite_3",  "Large Titanite Shard","sm_crags", Interactable::Type::Item,     true,  "large_titanite_shard"},
            {"lore_salt",        "Salt-Scoured Tablet","sm_flats", Interactable::Type::Lore,     false, "lore_salt_1"},
        },
    },

    // ----- bell_gate (final chain link) -----
    {
        "bell_gate",
        {
            {"bg_approach",  "Bell Gate Approach",  0.0f,   0.0f, 14.0f, 10.0f, true,  false, false},
            {"bg_gallery",   "Gallery of Knells",   40.0f,   0.0f, 20.0f, 12.0f, false, false, false},
            {"bg_belfry",    "The Belfry",          80.0f,  -4.0f, 12.0f,  8.0f, false, true,  false},
            {"bg_summit",    "Summit of Bells",     116.0f,  0.0f, 16.0f, 14.0f, false, false, true},
        },
        {
            {"bg_approach", "bg_gallery", 32.0f, false},
            {"bg_gallery",  "bg_belfry",  34.0f, false},
            {"bg_belfry",   "bg_summit",  30.0f, false},
        },
        {
            {"bonfire_bg_belfry", "Belfry Bonfire",   "bg_belfry",  Interactable::Type::Bonfire, false, ""},
            {"item_boss_soul",    "Ember Watcher Soul","bg_gallery", Interactable::Type::Item,    true,  "ember_watcher_soul"},
            {"lore_bell",         "Last Bell Tablet",  "bg_approach",Interactable::Type::Lore,     false, "lore_bell_1"},
        },
    },
};

constexpr size_t kCount = sizeof(kGeometries) / sizeof(kGeometries[0]);

} // namespace

const WorldGeometry* geometry_for_zone(const std::string& zone_id) {
    for (size_t i = 0; i < kCount; ++i)
        if (kGeometries[i].zone_id == zone_id) return &kGeometries[i];
    return nullptr;
}

size_t geometry_count() { return kCount; }

const WorldGeometry* geometry(size_t index) {
    return index < kCount ? &kGeometries[index] : nullptr;
}

} // namespace tebf
