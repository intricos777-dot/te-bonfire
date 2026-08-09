#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "difficulty.h"

namespace tebf {

// Gate boss of Ember Keep - the first fight the player must actually win.
struct GateBoss {
    std::string id = "ember_watcher";
    std::string name = "Ember Watcher";
    std::string epithet = "It keeps the bonfire that keeps the door.";
    int hp = 220;
    int damage = 12;
    int poise = 40;
    int hit_chance = 70;   // % ; scaled by DifficultyProfile
    int damage_variance = 6;
};

// One-shot stamina-loop session against a gate boss.
// The player spends stamina to swing; the boss runs a telegraph > strike >
// recover pattern. Guarding blocks the whole swing, at a 18% break chance.
class BossFight {
public:
    BossFight(const GateBoss& boss, const DifficultyProfile& prof);

    void player_attack();                 // swing: stamina cost, hit roll
    void player_guard();                // brace: blocks this round
    void player_rest();                 // recover stamina, open window
    void boss_round();                  // boss telegraph/strike/recover
    bool over() const { return m_over; }
    bool won() const { return m_won; }
    int boss_hp() const { return m_boss_hp; }
    int player_hp() const { return m_hp; }
    int stamina() const { return m_stamina; }
    int max_hp() const { return m_max_hp; }
    int round() const { return m_round; }
    const std::vector<std::string>& log() const { return m_log; }

private:
    unsigned m_rand_state = 0x9E3779B9u;
    unsigned next_rand() {
        m_rand_state = m_rand_state * 1664525u + 1013904223u;
        return m_rand_state >> 8;
    }
    bool hit_chance() { return (next_rand() % 100u) < m_hit_chance; }

    const GateBoss& m_boss;
    DifficultyProfile m_prof;
    int m_hp = 100, m_max_hp = 100;
    int m_stamina = 100, m_stamina_max = 100;
    int m_boss_hp = 0;
    unsigned m_hit_chance = 70;
    int m_boss_phase = 0;   // 0 strike-pending, 1 recover
    int m_phase_left = 1;
    bool m_guarding = false;
    int m_round = 0;
    bool m_over = false, m_won = false;
    std::vector<std::string> m_log;
};

} // namespace tebf