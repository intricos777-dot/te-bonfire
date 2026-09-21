#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tebf {

// Enemy archetypes — each a distinct AI behavior slot.
enum class EnemyKind : uint8_t {
    Hollow,         // slow, telegraphed lunge — tutorial trash
    Lizard,         // fast, darting strikes — erratic
    BlackKnight,    // tanky, guard-break capable — elite
    BellGuard,      // patrols, rings alarm — triggers adds
    Witch,          // caster: ranged bolts, keeps distance
    Sentinel,       // heavy poise, wide sweeps — mini-boss
};

struct Enemy {
    std::string id;
    std::string name;
    std::string room_id;
    EnemyKind kind = EnemyKind::Hollow;
    int hp = 50;
    int max_hp = 50;
    int damage = 8;
    int stamina = 100;
    int max_stamina = 100;
    int poise = 20;
    int hit_chance = 60;
    bool alive = true;
    float aggression = 0.5f;        // 0 passive, 1 relentless
    int soul_reward = 50;           // souls dropped on death
    int weapon_upgrade_chance = 15; // % chance to drop a titanite shard
    std::vector<std::string> loot_table; // guaranteed item drops
};

// Zone encounter roster: the enemies populating a zone.
struct Encounter {
    std::string zone_id;
    std::vector<Enemy> enemies;
};

// Resolve the authored encounter for a zone. nullptr if zone has none.
const Encounter* encounter_for_zone(const std::string& zone_id);

// Flat index of every authored encounter.
size_t encounter_count();
const Encounter* encounter(size_t index);

} // namespace tebf
