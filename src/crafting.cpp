#include "crafting.h"

namespace tebf {

namespace {

const ItemDef kItems[] = {
    // Weapons
    {"dusksteel_sword",      "Dusksteel Sword",    ItemKind::Weapon,    1,  20, "A longsword of blackened steel."},
    {"ember_claymore",       "Ember Claymore",     ItemKind::Weapon,    1,  30, "A heavy blade that glows faintly."},

    // Consumables
    {"estus_flask",          "Estus Flask",        ItemKind::Consumable,5,  0, "Restores HP on use."},
    {"estus_shard",          "Estus Shard",        ItemKind::Consumable,1,  0, "Upgrades the Estus Flask capacity."},
    {"firebomb",             "Firebomb",          ItemKind::Consumable,99,12, "A thrown explosive."},
    {"kindled_ember",        "Kindled Ember",      ItemKind::KeyItem,   1,  0, "An ember from a lit bonfire."},

    // Upgrade materials
    {"titanite_shard",       "Titanite Shard",    ItemKind::KeyItem,  99, 0, "Upgrades a standard weapon by +1."},
    {"large_titanite_shard", "Large Titanite Shard",ItemKind::KeyItem,99, 0, "Upgrades a standard weapon by +2."},
    {"titanite_chunk",       "Titanite Chunk",    ItemKind::KeyItem,  99, 0, "Upgrades a standard weapon by +3."},

    // Armor
    {"ashen_helm",           "Ashen Helm",         ItemKind::Armor,    1,  3, "Light helm scorched by fire."},
    {"ashen_mail",           "Ashen Mail",         ItemKind::Armor,    1,  8, "Light mail scorched by fire."},

    // Key items
    {"ember_watcher_soul",   "Soul of the Ember Watcher",ItemKind::KeyItem,1,0,"A crystallized soul. Grants many souls on use."},
    {"bell_key",             "Bell Gate Key",     ItemKind::KeyItem,  1,  0, "Unlocks the door to Bell Gate."},
};

constexpr size_t kItemCount = sizeof(kItems) / sizeof(kItems[0]);

const WeaponTrack kTracks[] = {
    {
        "dusksteel_sword",
        {
            {0,  0, {}},
            {1,  4, {{"titanite_shard", 2}}},
            {2,  8, {{"titanite_shard", 4}}},
            {3, 12, {{"large_titanite_shard", 2}}},
            {4, 16, {{"large_titanite_shard", 3}}},
            {5, 20, {{"titanite_chunk", 2}}},
        }
    },
    {
        "ember_claymore",
        {
            {0,  0, {}},
            {1,  5, {{"titanite_shard", 2}}},
            {2, 10, {{"titanite_shard", 4}}},
            {3, 15, {{"large_titanite_shard", 2}}},
            {4, 20, {{"large_titanite_shard", 3}}},
            {5, 26, {{"titanite_chunk", 2}}},
        }
    },
};

constexpr size_t kTrackCount = sizeof(kTracks) / sizeof(kTracks[0]);

} // namespace

const ItemDef* find_item(const std::string& id) {
    for (size_t i = 0; i < kItemCount; ++i)
        if (kItems[i].id == id) return &kItems[i];
    return nullptr;
}

const WeaponTrack* find_weapon_track(const std::string& id) {
    for (size_t i = 0; i < kTrackCount; ++i)
        if (kTracks[i].weapon_id == id) return &kTracks[i];
    return nullptr;
}

int Inventory::count(const std::string& id) const {
    for (const auto& p : items)
        if (p.first == id) return p.second;
    return 0;
}

void Inventory::add(const std::string& id, int n) {
    for (auto& p : items) {
        if (p.first == id) {
            p.second += n;
            return;
        }
    }
    items.push_back({id, n});
}

bool Inventory::consume(const std::string& id, int n) {
    for (auto& p : items) {
        if (p.first == id) {
            if (p.second < n) return false;
            p.second -= n;
            if (p.second == 0) {
                // remove from vector (compact)
                for (auto it = items.begin(); it != items.end(); ++it) {
                    if (it->first == id) { items.erase(it); break; }
                }
            }
            return true;
        }
    }
    return false;
}

int Inventory::weapon_tier(const std::string& weapon_id) const {
    for (const auto& p : upgrades)
        if (p.first == weapon_id) return p.second;
    return 0;
}

void Inventory::set_weapon_tier(const std::string& weapon_id, int tier) {
    for (auto& p : upgrades) {
        if (p.first == weapon_id) {
            p.second = tier;
            return;
        }
    }
    upgrades.push_back({weapon_id, tier});
}

bool upgrade_weapon(Inventory& inv, const std::string& weapon_id) {
    const WeaponTrack* track = find_weapon_track(weapon_id);
    if (!track) return false;
    int current_tier = inv.weapon_tier(weapon_id);
    if (current_tier >= track->max_tier()) return false;
    // next tier
    const UpgradeTier& next = track->tiers[current_tier + 1];
    // check materials
    for (const auto& m : next.materials) {
        if (inv.count(m.item_id) < m.quantity) return false;
    }
    // consume materials
    for (const auto& m : next.materials) {
        inv.consume(m.item_id, m.quantity);
    }
    inv.set_weapon_tier(weapon_id, next.tier);
    return true;
}

int weapon_power(const Inventory& inv, const std::string& weapon_id) {
    const ItemDef* def = find_item(weapon_id);
    if (!def) return 0;
    int base = def->base_power;
    const WeaponTrack* track = find_weapon_track(weapon_id);
    if (!track) return base;
    int tier = inv.weapon_tier(weapon_id);
    if (tier < 0 || tier >= static_cast<int>(track->tiers.size())) return base;
    return base + track->tiers[tier].power_bonus;
}

size_t item_count()   { return kItemCount; }
const ItemDef* item(size_t i) { return i < kItemCount ? &kItems[i] : nullptr; }

size_t weapon_track_count()   { return kTrackCount; }
const WeaponTrack* weapon_track(size_t i) { return i < kTrackCount ? &kTracks[i] : nullptr; }

} // namespace tebf
