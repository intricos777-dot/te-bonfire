#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tebf {

// A craftable/upgradeable item archetype.
enum class ItemKind : uint8_t {
    Weapon,         // melee weapon — can be upgraded with titanite
    Armor,          // armor piece — reduces incoming damage
    Consumable,     // one-shot: estus flask, resin, bomb
    KeyItem,        // quest items (keys, embers)
};

struct ItemDef {
    std::string id;
    std::string name;
    ItemKind kind = ItemKind::Consumable;
    int stack_max = 1;
    int base_power = 0;             // weapon damage or armor reduction
    std::string description;
};

// Material needed for an upgrade.
struct UpgradeMaterial {
    std::string item_id;            // e.g. "titanite_shard"
    int quantity = 1;
};

// A weapon upgrade tier (0 → 1 → ... → max).
struct UpgradeTier {
    int tier = 0;
    int power_bonus = 0;
    std::vector<UpgradeMaterial> materials;
};

// Full upgrade track for a weapon.
struct WeaponTrack {
    std::string weapon_id;
    std::vector<UpgradeTier> tiers;
    int max_tier() const { return static_cast<int>(tiers.size()) - 1; }
};

// Resolve the item definition for an id. nullptr if not found.
const ItemDef* find_item(const std::string& id);

// Resolve the upgrade track for a weapon id. nullptr if not found.
const WeaponTrack* find_weapon_track(const std::string& id);

// Player inventory: simple id → count map with upgrade tier overlay.
struct Inventory {
    // item id -> count
    std::vector<std::pair<std::string, int>> items;
    // weapon id -> current upgrade tier
    std::vector<std::pair<std::string, int>> upgrades;

    int count(const std::string& id) const;
    void add(const std::string& id, int n = 1);
    bool consume(const std::string& id, int n = 1);
    int weapon_tier(const std::string& weapon_id) const;
    void set_weapon_tier(const std::string& weapon_id, int tier);
};

// Attempt to upgrade a weapon by one tier. Returns true on success.
bool upgrade_weapon(Inventory& inv, const std::string& weapon_id);

// Resolve a weapon's current power (base + upgrade bonus).
int weapon_power(const Inventory& inv, const std::string& weapon_id);

// Flat registries.
size_t item_count();
const ItemDef* item(size_t index);

size_t weapon_track_count();
const WeaponTrack* weapon_track(size_t index);

} // namespace tebf
