#include "boss.h"
#include <cstdio>

namespace tebf {

BossFight::BossFight(const GateBoss& boss, const DifficultyProfile& prof)
    : m_boss(boss), m_prof(prof) {
    m_max_hp = 100;
    m_hp = m_max_hp;
    m_stamina_max = 100;
    m_stamina = m_stamina_max;
    m_boss_hp = boss.hp;
    // boss hit chance in percent, scaled down on casual
    m_hit_chance = (unsigned)((boss.hit_chance * prof.enemy_hit_chance) / 100u);
    m_phase_left = 1 + prof.enemy_reaction_ticks;
    m_log.push_back(m_boss.name + " bars the gate. " + m_boss.epithet);
}

void BossFight::player_attack() {
    if (m_over) return;
    const int cost = 20;
    if (m_stamina < cost) {
        m_log.push_back("stamina dry - blow refused");
        return;
    }
    m_stamina -= cost;
    // hero strikes always connect; damage scaled by the success of the roll
    int dmg = cost;
    if ((next_rand() % 100u) > 60) dmg += 6;
    m_boss_hp -= dmg;
    m_log.push_back("dusksteel bites for " + std::to_string(dmg) +
                    " (boss hp " + std::to_string(m_boss_hp < 0 ? 0 : m_boss_hp) + ")");
    if (m_boss_hp <= 0) { m_over = true; m_won = true; }
    m_guarding = false;
}

void BossFight::player_guard() {
    if (m_over) return;
    m_stamina -= 5;
    if (m_stamina < 0) m_stamina = 0;
    m_guarding = true;
    m_log.push_back("guard raised");
}

void BossFight::player_rest() {
    if (m_over) return;
    m_stamina += 20;
    if (m_stamina > m_stamina_max) m_stamina = m_stamina_max;
    m_guarding = false;
    m_log.push_back("recovery breath");
}

void BossFight::boss_round() {
    if (m_over) return;
    ++m_round;
    const bool guarded = m_guarding;
    m_guarding = false;

    if (m_phase_left > 1) {
        --m_phase_left;
        m_log.push_back("the watcher coils...");
        return;
    }

    if (m_boss_phase == 0) {
        // strike: beaten by guard, or the roll beats the player's stance
        if (guarded) {
            const unsigned break_roll = next_rand() % 100u;
            if (break_roll < 18) {
                const int dmg = (int)(m_boss.damage * m_prof.damage_scale) / 2;
                m_hp -= dmg;
                m_log.push_back("GUARD BREAK - " + std::to_string(dmg) + " through the stance");
            } else {
                m_log.push_back("blow sparks off the guard");
            }
        } else if (hit_chance()) {
            const int dmg = (int)(m_boss.damage * m_prof.damage_scale);
            m_hp -= dmg;
            m_log.push_back("a heavy blow lands for " + std::to_string(dmg));
        } else {
            m_log.push_back("a wild blow whistles past");
        }
        m_boss_phase = 1;
        m_phase_left = 1 + (int)m_prof.enemy_reaction_ticks;
    } else {
        // recover: the watcher vents, opening itself briefly
        m_boss_hp += 6;
        if (m_boss_hp > m_boss.hp) m_boss_hp = m_boss.hp;
        m_log.push_back("the watcher staggers back (" + std::to_string(m_boss_hp) + " hp)");
        m_boss_phase = 0;
        m_phase_left = 3;
    }

    if (m_hp <= 0) { m_over = true; m_won = false; }
}

} // namespace tebf