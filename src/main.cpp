#include <engine/engine.h>
#include <engine/math_types.h>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>

#include "difficulty.h"
#include "world.h"
#include "boss.h"
#include "hidden_boss.h"
#include "world_geometry.h"
#include "encounter.h"
#include "leveling.h"
#include "crafting.h"
#include "warp.h"
#include "boss_registry.h"

namespace {

struct Stats {
    int vit = 10;
    int end = 10;
    int str = 10;
    int dex = 10;
    int hp() const { return vit * 10; }
    int stamina_max() const { return end * 10; }
};

struct Fighter {
    Stats stats;
    int hp = stats.hp();
    int stamina = stats.stamina_max();
    float poise = 0.0f;
    bool guarding = false;

    bool swing(float cost) {
        if (stamina < (int)cost) return false;
        stamina -= (int)cost;
        return true;
    }
    void tick(float dt) { stamina += (int)(12.0f*dt); poise = 0.3f; }
};

// run the encounter: autopilot through all enemies in a zone
int run_encounter(const tebf::Encounter& enc, Fighter& hero, tebf::PlayerVitals& vit, tebf::Inventory& inv, const tebf::DifficultyProfile& prof, unsigned& rand_state) {
    auto next_rand = [&]() {
        rand_state = rand_state * 1664525u + 1013904223u;
        return rand_state >> 8;
    };
    int total_souls = 0;
    for (const auto& en : enc.enemies) {
        if (!en.alive) continue;
        std::printf("  encounter: %s (%s) hp=%d dmg=%d\n", en.name.c_str(), enc.zone_id.c_str(), en.hp, en.damage);
        int ehp = en.hp;
        int estamina = en.max_stamina;
        hero.stamina = hero.stats.stamina_max();
        while (ehp > 0 && hero.hp > 0) {
            // enemy turn: telegraphed attack
            bool en_guarding = false;
            if (estamina >= 10) {
                estamina -= 10;
                unsigned roll = next_rand() % 100u;
                if (roll < (unsigned)en.hit_chance) {
                    int dmg = (int)((float)en.damage * prof.damage_scale);
                    if (hero.guarding) {
                        // 18% guard break chance
                        if ((next_rand() % 100u) < 18u) {
                            dmg = dmg / 2;
                            hero.hp -= dmg;
                            hero.guarding = false;
                        }
                    } else {
                        hero.hp -= dmg;
                    }
                }
            }
            // hero turn: attack if stamina-rich, rest otherwise
            if (hero.stamina >= 30) {
                hero.swing(20);
                int dmg = 20 + vit.melee_power();
                if ((next_rand() % 100u) > 60) dmg += 6;
                ehp -= dmg;
                hero.guarding = false;
            } else if (hero.stamina < 15) {
                hero.stamina += 15;
                if (hero.stamina > hero.stats.stamina_max()) hero.stamina = hero.stats.stamina_max();
                hero.guarding = true;
            } else {
                hero.swing(15);
                int dmg = 15 + vit.melee_power();
                ehp -= dmg;
                hero.guarding = false;
            }
        }
        if (hero.hp <= 0) {
            std::printf("  HERO DIED against %s\n", en.name.c_str());
            return -1;
        }
        // collect loot
        int drop = tebf::soul_drop(en.soul_reward, prof.xp_scale);
        vit.souls += drop;
        total_souls += drop;
        std::printf("    +%d souls (total: %d)\n", drop, vit.souls);
        // chance to drop titanite
        if ((next_rand() % 100u) < (unsigned)en.weapon_upgrade_chance) {
            inv.add("titanite_shard", 1);
            std::printf("    +1 titanite shard dropped\n");
        }
        for (const auto& loot : en.loot_table) {
            inv.add(loot, 1);
            std::printf("    +1 %s dropped\n", loot.c_str());
        }
    }
    return total_souls;
}

void run_boss_fight(const tebf::BossDef& boss, Fighter& hero, tebf::PlayerVitals& vit, tebf::Inventory& inv, const tebf::DifficultyProfile& prof, unsigned& rand_state) {
    tebf::BossEncounter fight(boss, prof);
    std::printf("  BOSS: %s — \"%s\"\n", boss.name.c_str(), boss.epithet.c_str());
    while (!fight.over()) {
        // autopilot: dodge on berserker/colossus telegraph, guard on gatekeeper, attack on openings
        if (boss.archetype == tebf::BossArchetype::Berserker || boss.archetype == tebf::BossArchetype::Colossus) {
            if (fight.stamina() >= 40) fight.player_attack();
            else if (fight.stamina() >= 15) fight.player_dodge();
            else fight.player_rest();
        } else if (boss.archetype == tebf::BossArchetype::Trickster) {
            if (fight.stamina() >= 30) fight.player_attack();
            else fight.player_rest();
        } else {
            if (fight.round() % 4 == 3) fight.player_guard();
            else if (fight.stamina() >= 40) fight.player_attack();
            else fight.player_rest();
        }
        fight.boss_round();
    }
    if (fight.won()) {
        std::printf("  BOSS VICTORIOUS: %s in %d rounds (hp=%d)\n", boss.name.c_str(), fight.round(), fight.player_hp());
        vit.souls += boss.soul_reward;
        std::printf("  +%d souls (total: %d)\n", boss.soul_reward, vit.souls);
        for (const auto& loot : boss.loot_table) {
            inv.add(loot, 1);
            std::printf("  +1 %s\n", loot.c_str());
        }
    } else {
        std::printf("  HERO SLAIN by %s in %d rounds\n", boss.name.c_str(), fight.round());
    }
}

int run_sim() {
    tebf::DifficultyProfile prof = tebf::profile_for(tebf::Difficulty::Legend);
    std::printf("=== te-bonfire ALPHA sim === (difficulty: %s)\n\n", tebf::name_of(tebf::Difficulty::Legend));

    // 1. World geometry overview
    std::printf("[WORLDS]\n");
    for (size_t i = 0; i < tebf::geometry_count(); ++i) {
        const tebf::WorldGeometry* g = tebf::geometry(i);
        if (!g) continue;
        std::printf("  %s (%d rooms, %d paths, %d interactables)\n",
                    g->zone_id.c_str(), (int)g->rooms.size(), (int)g->paths.size(), (int)g->interactables.size());
        for (const auto& r : g->rooms) {
            std::printf("    room: %s \"%s\" at (%.0f,%.0f) %.0fx%.0f%s%s\n",
                        r.id.c_str(), r.name.c_str(), r.x, r.y, r.w*2, r.h*2,
                        r.cleared ? " [cleared]" : "", r.has_bonfire ? " [bonfire]" : "");
        }
    }
    std::printf("\n");

    // 2. Encounters
    std::printf("[ENCOUNTERS]\n");
    unsigned rand_state = 0xDEADBEEFu;
    Fighter hero;
    tebf::PlayerVitals vit;
    tebf::Inventory inv;
    inv.add("dusksteel_sword", 1);
    inv.add("estus_flask", 3);

    for (size_t i = 0; i < tebf::encounter_count(); ++i) {
        const tebf::Encounter* enc = tebf::encounter(i);
        if (!enc) continue;
        std::printf("Zone: %s (%d enemies)\n", enc->zone_id.c_str(), (int)enc->enemies.size());
        hero.hp = hero.stats.hp();
        hero.stamina = hero.stats.stamina_max();
        int souls_gained = run_encounter(*enc, hero, vit, inv, prof, rand_state);
        if (souls_gained < 0) {
            std::printf("  ** hero died — encounter chain broken **\n");
            break;
        }
        std::printf("  zone cleared! +%d souls\n\n", souls_gained);
    }

    // 3. Leveling
    std::printf("[LEVELING]\n");
    std::printf("  start: level=%d vit=%d end=%d str=%d dex=%d souls=%d\n",
                vit.level, vit.vit, vit.end, vit.str, vit.dex, vit.souls);
    // spend souls: 3 into vit, 2 into str
    for (int i = 0; i < 3; ++i) {
        if (tebf::spend_souls(vit, 0)) std::printf("    -> vit raised to %d\n", vit.vit);
        else { std::printf("    -> not enough souls for vit\n"); break; }
    }
    for (int i = 0; i < 2; ++i) {
        if (tebf::spend_souls(vit, 2)) std::printf("    -> str raised to %d\n", vit.str);
        else { std::printf("    -> not enough souls for str\n"); break; }
    }
    std::printf("  end:   level=%d vit=%d end=%d str=%d dex=%d souls=%d (spent=%d)\n\n",
                vit.level, vit.vit, vit.end, vit.str, vit.dex, vit.souls, vit.souls_spent);

    // 4. Crafting
    std::printf("[CRAFTING]\n");
    std::printf("  inventory: titanite_shard=%d, dusksteel_sword (base pwr=%d)\n",
                inv.count("titanite_shard"), tebf::weapon_power(inv, "dusksteel_sword"));
    int upgrades = 0;
    for (int i = 0; i < 5; ++i) {
        if (tebf::upgrade_weapon(inv, "dusksteel_sword")) {
            upgrades++;
            std::printf("  upgrade %d -> tier=%d power=%d titanite=%d\n", upgrades,
                        inv.weapon_tier("dusksteel_sword"),
                        tebf::weapon_power(inv, "dusksteel_sword"),
                        inv.count("titanite_shard"));
        } else {
            std::printf("  (no more materials for upgrade)\n");
            break;
        }
    }
    std::printf("\n");

    // 5. Warp network
    std::printf("[WARP NETWORK]\n");
    tebf::WarpNetwork net = tebf::build_warp_network();
    auto dests = net.destinations();
    std::printf("  lit bonfires: %d\n", (int)dests.size());
    // light the rest
    for (const auto* n : dests) {
        std::printf("    @ %s — %s\n", n->display_name.c_str(), n->zone_id.c_str());
    }
    tebf::light_bonfire(net, "bonfire_bth_cistern");
    tebf::light_bonfire(net, "bonfire_ek_vestibule");
    tebf::light_bonfire(net, "bonfire_sm_brine");
    tebf::light_bonfire(net, "bonfire_bg_belfy");
    dests = net.destinations();
    std::printf("  after lighting: %d bonfires\n", (int)dests.size());
    for (const auto* n : dests) {
        std::printf("    @ %s — %s\n", n->display_name.c_str(), n->zone_id.c_str());
    }
    std::printf("\n");

    // 6. World progression
    std::printf("[WORLD PROGRESSION]\n");
    tebf::WorldProgress prog;
    const bool had = tebf::load_progress(prog, "data/progress.json");
    std::printf("  progress file: %s (deaths=%d bonfires=%d legend=%s)\n",
                had ? "loaded" : "new", prog.deaths, prog.bonfires_lit,
                prog.tutorial_defeated_legend ? "yes" : "no");

    // clear zones in order
    for (size_t i = 0; i < tebf::zone_count(); ++i) {
        const tebf::ZoneDef* z = tebf::zone(i);
        if (!z) continue;
        tebf::clear_zone(prog, z->id);
        std::printf("  cleared: %s (bonfires_lit=%d)\n", z->id.c_str(), prog.bonfires_lit);
    }
    prog.tutorial_defeated_legend = true;
    prog.deaths = 3;
    if (!tebf::save_progress(prog, "data/progress.json"))
        std::fprintf(stderr, "  could not persist progress\n");
    else
        std::printf("  progress saved\n");
    std::printf("\n");

    // 7. Boss fights
    std::printf("[BOSSES]\n");
    hero.hp = hero.stats.hp();
    for (size_t i = 0; i < tebf::boss_count(); ++i) {
        const tebf::BossDef* b = tebf::boss(i);
        if (!b) continue;
        std::printf("--- %s (%s) ---\n", b->name.c_str(), b->zone_id.c_str());
        run_boss_fight(*b, hero, vit, inv, prof, rand_state);
        std::printf("\n");
    }

    // 8. Hidden bosses
    std::printf("[HIDDEN BOSSES]\n");
    for (size_t i = 0; i < tebf::hidden_boss_count(); ++i) {
        const tebf::HiddenBoss* hb = tebf::hidden_boss(i);
        if (!hb) continue;
        std::printf("  %s — \"%s\"\n", hb->name.c_str(), hb->epithet.c_str());
        std::printf("    unlock: %s\n", hb->unlock_hint.c_str());
        std::printf("    hp=%d dmg=%d poise=%d guard_break=%s\n",
                    hb->hp, hb->damage, hb->poise, hb->uses_guard_break ? "yes" : "no");
    }

    std::printf("\n=== sim complete ===\n");
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    const bool sim = argc > 1 && std::strcmp(argv[1], "--sim") == 0;
    if (sim) return run_sim();

    te::EngineConfig cfg{};
    cfg.window_title = "te-bonfire";
    cfg.window_width = 1280;
    cfg.window_height = 720;
    auto& engine = te::Engine::instance();
    if (!engine.initialize(cfg)) {
        std::fprintf(stderr, "engine init failed\n");
        return 1;
    }
    std::printf("te-bonfire running...\n");
    engine.run();
    engine.shutdown();
    return 0;
}
