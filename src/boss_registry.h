#pragma once
#include <string>
#include <vector>
#include "difficulty.h"

namespace tebf {

// Boss archetypes — each a distinct fight script.
enum class BossArchetype : uint8_t {
    Gatekeeper,         // Ember Watcher style: telegraph/strike/recover
    Berserker,          // Aggressive, fast, low poise
    Trickster,          // Teleports, feints, punishes greed
    Colossus,           // Slow, massive AoE, high poise
    Duelist,            // Parry-focused, rewards precision
};

struct BossDef {
    std::string id;
    std::string name;
    std::string epithet;
    std::string zone_id;            // zone this boss gates
    BossArchetype archetype = BossArchetype::Gatekeeper;
    int hp = 200;
    int damage = 12;
    int poise = 40;
    int hit_chance = 70;
    int soul_reward = 500;
    // archetype-specific tuning
    float aggression = 0.5f;        // higher = more frequent attacks
    bool uses_guard_break = false;
    bool uses_aoe = false;
    bool uses_teleport = false;
    std::vector<std::string> loot_table;
};

// Resolve the boss for a zone. nullptr if zone has no boss.
const BossDef* boss_for_zone(const std::string& zone_id);

// Flat registry.
size_t boss_count();
const BossDef* boss(size_t index);

// Multi-phase boss fight: extends BossFight with archetype behavior.
class BossEncounter {
public:
    BossEncounter(const BossDef& def, const DifficultyProfile& prof);

    // Player actions
    void player_attack();
    void player_guard();
    void player_rest();
    void player_dodge();            // new: i-frames, costs stamina

    // Boss tick
    void boss_round();

    // State
    bool over() const;
    bool won() const;
    int boss_hp() const;
    int boss_max_hp() const;
    int player_hp() const;
    int stamina() const;
    int max_hp() const;
    int round() const;
    const std::vector<std::string>& log() const;
    const BossDef& def() const;

private:
    unsigned m_rand_state = 0x9E3779B9u;
    unsigned next_rand() {
        m_rand_state = m_rand_state * 1664525u + 1013904223u;
        return m_rand_state >> 8;
    }
    bool hit_chance();

    const BossDef& m_def;
    DifficultyProfile m_prof;
    int m_hp = 100, m_max_hp = 100;
    int m_stamina = 100, m_stamina_max = 100;
    int m_boss_hp = 0;
    int m_boss_max_hp = 0;
    unsigned m_hit_chance = 70;
    int m_boss_phase = 0;
    int m_phase_left = 1;
    bool m_guarding = false;
    bool m_dodging = false;
    int m_round = 0;
    bool m_over = false, m_won = false;
    std::vector<std::string> m_log;
};

} // namespace tebf
