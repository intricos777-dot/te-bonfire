#include "boss_registry.h"
#include <cstdio>
#include <cstring>

namespace tebf {

namespace {

const BossDef kBosses[] = {
    // Ember Keep — the gatekeeper tutorial boss
    {
        "ember_watcher", "Ember Watcher",
        "It keeps the bonfire that keeps the door.",
        "ember_keep", BossArchetype::Gatekeeper,
        /*hp=*/220, /*dmg=*/12, /*poise=*/40, /*hit=*/70,
        /*souls=*/500, /*aggro=*/0.5f,
        /*guard_break=*/false, /*aoe=*/false, /*teleport=*/false,
        /*loot=*/{"ember_watcher_soul"},
    },
    // Blackthorns — berserker mini-boss
    {
        "thorn_berserker", "Thorn Berserker",
        "A knight driven mad by the bramble. It does not stop swinging.",
        "blackthorns", BossArchetype::Berserker,
        /*hp=*/320, /*dmg=*/18, /*poise=*/25, /*hit=*/75,
        /*souls=*/800, /*aggro=*/0.85f,
        /*guard_break=*/true, /*aoe=*/false, /*teleport=*/false,
        /*loot=*/{"titanite_chunk"},
    },
    // Salt Marches — trickster witch-queen
    {
        "brine_witch_queen", "Brine Witch-Queen",
        "She steps between the raindrops. Strike where she was.",
        "salt_marches", BossArchetype::Trickster,
        /*hp=*/280, /*dmg=*/14, /*poise=*/20, /*hit=*/80,
        /*souls=*/1200, /*aggro=*/0.6f,
        /*guard_break=*/false, /*aoe=*/false, /*teleport=*/true,
        /*loot=*/{"large_titanite_shard"},
    },
    // Bell Gate — colossus final boss
    {
        "bell_colossus", "The Bell Colossus",
        "A tower of iron and grief. The bell tolls for you.",
        "bell_gate", BossArchetype::Colossus,
        /*hp=*/600, /*dmg=*/24, /*poise=*/120, /*hit=*/85,
        /*souls=*/2500, /*aggro=*/0.4f,
        /*guard_break=*/true, /*aoe=*/true, /*teleport=*/false,
        /*loot=*/{"bell_key", "titanite_chunk"},
    },
};

constexpr size_t kCount = sizeof(kBosses) / sizeof(kBosses[0]);

} // namespace

const BossDef* boss_for_zone(const std::string& zone_id) {
    for (size_t i = 0; i < kCount; ++i)
        if (kBosses[i].zone_id == zone_id) return &kBosses[i];
    return nullptr;
}

size_t boss_count() { return kCount; }

const BossDef* boss(size_t index) {
    return index < kCount ? &kBosses[index] : nullptr;
}

// ----- BossEncounter -----

BossEncounter::BossEncounter(const BossDef& def, const DifficultyProfile& prof)
    : m_def(def), m_prof(prof) {
    m_max_hp = 100;
    m_hp = m_max_hp;
    m_stamina_max = 100;
    m_stamina = m_stamina_max;
    m_boss_max_hp = def.hp;
    m_boss_hp = def.hp;
    m_hit_chance = (unsigned)((def.hit_chance * prof.enemy_hit_chance) / 100u);
    m_phase_left = 1 + prof.enemy_reaction_ticks;
    m_log.push_back(def.name + std::string(" bars the gate. ") + def.epithet);
}

void BossEncounter::player_attack() {
    if (m_over) return;
    const int cost = 20;
    if (m_stamina < cost) {
        m_log.push_back("stamina dry - blow refused");
        return;
    }
    m_stamina -= cost;
    int dmg = cost;
    if ((next_rand() % 100u) > 60) dmg += 6;
    // Colossus reduces incoming damage due to massive poise
    if (m_def.archetype == BossArchetype::Colossus && dmg > 8) dmg = dmg * 3 / 4;
    m_boss_hp -= dmg;
    m_log.push_back("dusksteel bites for " + std::to_string(dmg) +
                    " (boss hp " + std::to_string(m_boss_hp < 0 ? 0 : m_boss_hp) + ")");
    if (m_boss_hp <= 0) { m_over = true; m_won = true; }
    m_guarding = false;
    m_dodging = false;
}

void BossEncounter::player_guard() {
    if (m_over) return;
    m_stamina -= 5;
    if (m_stamina < 0) m_stamina = 0;
    m_guarding = true;
    m_dodging = false;
    m_log.push_back("guard raised");
}

void BossEncounter::player_rest() {
    if (m_over) return;
    m_stamina += 20;
    if (m_stamina > m_stamina_max) m_stamina = m_stamina_max;
    m_guarding = false;
    m_dodging = false;
    m_log.push_back("recovery breath");
}

void BossEncounter::player_dodge() {
    if (m_over) return;
    const int cost = 12;
    if (m_stamina < cost) {
        m_log.push_back("stamina dry - dodge refused");
        return;
    }
    m_stamina -= cost;
    m_dodging = true;
    m_guarding = false;
    m_log.push_back("roll! i-frames active");
}

void BossEncounter::boss_round() {
    if (m_over) return;
    ++m_round;
    const bool guarded = m_guarding;
    const bool dodged  = m_dodging;
    m_guarding = false;
    m_dodging  = false;

    if (m_phase_left > 1) {
        --m_phase_left;
        m_log.push_back("the boss coils...");
        return;
    }

    // Archetype-specific behavior
    if (m_def.archetype == BossArchetype::Berserker) {
        // Berserker: always attacks, fast recovery
        if (dodged) {
            m_log.push_back("the blow sails past — you rolled clear");
        } else if (guarded) {
            const unsigned brk = next_rand() % 100u;
            if (brk < (m_def.uses_guard_break ? 30u : 10u)) {
                const int dmg = (int)(m_def.damage * m_prof.damage_scale) / 2;
                m_hp -= dmg;
                m_log.push_back("GUARD BREAK! " + std::to_string(dmg) + " through the stance");
            } else {
                m_log.push_back("blow sparks off the guard");
            }
        } else if (hit_chance()) {
            const int dmg = (int)(m_def.damage * m_prof.damage_scale);
            m_hp -= dmg;
            m_log.push_back("a savage blow lands for " + std::to_string(dmg));
        } else {
            m_log.push_back("a wild blow whistles past");
        }
        m_phase_left = 1; // berserker attacks every round
    }
    else if (m_def.archetype == BossArchetype::Trickster) {
        // Trickster: teleports, feints, punishes greed
        if (m_boss_phase == 0) {
            // feint: 50% chance to whiff entirely
            if ((next_rand() % 100u) < 50u) {
                m_log.push_back("a feint! the witch-queen flickers");
            } else if (dodged) {
                m_log.push_back("you rolled through the witch-queen");
            } else if (guarded) {
                m_log.push_back("the witch-queen's blade slides off your guard");
            } else if (hit_chance()) {
                const int dmg = (int)(m_def.damage * m_prof.damage_scale);
                m_hp -= dmg;
                m_log.push_back("the witch-queen reappears behind you! " + std::to_string(dmg));
            } else {
                m_log.push_back("a wild blow whistles past");
            }
            m_boss_phase = 1;
            m_phase_left = 1 + (int)m_prof.enemy_reaction_ticks;
        } else {
            // recover: teleport to safety
            m_boss_hp += 4;
            if (m_boss_hp > m_boss_max_hp) m_boss_hp = m_boss_max_hp;
            m_log.push_back("the witch-queen teleports away (" + std::to_string(m_boss_hp) + " hp)");
            m_boss_phase = 0;
            m_phase_left = 2;
        }
    }
    else if (m_def.archetype == BossArchetype::Colossus) {
        // Colossus: slow, massive AoE, high poise
        if (m_boss_phase == 0) {
            if (dodged) {
                m_log.push_back("you roll clear of the colossus's quake");
            } else if (guarded && !m_def.uses_aoe) {
                m_log.push_back("the colossus's blow dents your guard");
            } else {
                // AoE hits regardless of guard
                const int dmg = (int)(m_def.damage * m_prof.damage_scale * (guarded ? 0.6f : 1.0f));
                m_hp -= dmg;
                m_log.push_back("the colossus brings the hammer down! " + std::to_string(dmg));
            }
            m_boss_phase = 1;
            m_phase_left = 2 + (int)m_prof.enemy_reaction_ticks;
        } else {
            m_boss_hp += 8;
            if (m_boss_hp > m_boss_max_hp) m_boss_hp = m_boss_max_hp;
            m_log.push_back("the colossus staggers (" + std::to_string(m_boss_hp) + " hp)");
            m_boss_phase = 0;
            m_phase_left = 3;
        }
    }
    else {
        // Gatekeeper (default, Ember Watcher style)
        if (m_boss_phase == 0) {
            if (dodged) {
                m_log.push_back("you rolled clear");
            } else if (guarded) {
                const unsigned brk = next_rand() % 100u;
                if (brk < 18) {
                    const int dmg = (int)(m_def.damage * m_prof.damage_scale) / 2;
                    m_hp -= dmg;
                    m_log.push_back("GUARD BREAK - " + std::to_string(dmg) + " through the stance");
                } else {
                    m_log.push_back("blow sparks off the guard");
                }
            } else if (hit_chance()) {
                const int dmg = (int)(m_def.damage * m_prof.damage_scale);
                m_hp -= dmg;
                m_log.push_back("a heavy blow lands for " + std::to_string(dmg));
            } else {
                m_log.push_back("a wild blow whistles past");
            }
            m_boss_phase = 1;
            m_phase_left = 1 + (int)m_prof.enemy_reaction_ticks;
        } else {
            m_boss_hp += 6;
            if (m_boss_hp > m_boss_max_hp) m_boss_hp = m_boss_max_hp;
            m_log.push_back("the watcher staggers back (" + std::to_string(m_boss_hp) + " hp)");
            m_boss_phase = 0;
            m_phase_left = 3;
        }
    }

    if (m_hp <= 0) { m_over = true; m_won = false; }
}

bool BossEncounter::hit_chance() { return (next_rand() % 100u) < m_hit_chance; }

bool BossEncounter::over()   const { return m_over; }
bool BossEncounter::won()    const { return m_won; }
int BossEncounter::boss_hp() const { return m_boss_hp; }
int BossEncounter::boss_max_hp() const { return m_boss_max_hp; }
int BossEncounter::player_hp() const { return m_hp; }
int BossEncounter::stamina() const { return m_stamina; }
int BossEncounter::max_hp()  const { return m_max_hp; }
int BossEncounter::round()   const { return m_round; }
const std::vector<std::string>& BossEncounter::log() const { return m_log; }
const BossDef& BossEncounter::def() const { return m_def; }

} // namespace tebf
