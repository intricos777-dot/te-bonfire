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

// te-bonfire — a stamina-driven souls-like action RPG on Twilight Elysium.
// Original code only — no retail assets; data/manifest.json points at assets
// you mount from your own legally-owned copies of a disc game.

namespace {

struct Stats {
    int vit = 10; // HP scale
    int end = 10; // stamina scale
    int str = 10; // melee power
    int dex = 10; // evade/parry window
    int hp() const { return vit * 10; }
    int stamina_max() const { return end * 10; }
};

struct Fighter {
    Stats stats;
    int hp = stats.hp();
    int stamina = stats.stamina_max();
    float poise = 0.0f;      // 0 unsheathed
    bool guarding = false;

    // returns true if the attack landed
    bool swing(float cost) {
        if (stamina < (int)cost) return false;
        stamina -= (int)cost;
        return true;
    }
    void tick(float dt) { stamina += (int)(12.0f*dt); poise = 0.3f; }
};

// checkpoint lit-bonfire model
struct Bonfire {
    te::Vec3 pos;
    bool lit = false;
};

int run_sim() {
    // world progression: a chain of zones; clearing one lights its bonfire
    // and unbolts the next. State persists to data/progress.json.
    tebf::WorldProgress prog;
    const bool had = tebf::load_progress(prog, "data/progress.json");
    std::printf("world: %d zones in the chain (progress file %s)\n",
                (int)tebf::zone_count(), had ? "loaded" : "new");
    if (had)
        std::printf("world: deaths=%d bonfires_lit=%d legendary=%s\n",
                    prog.deaths, prog.bonfires_lit,
                    prog.tutorial_defeated_legend ? "yes" : "no");

    // the first real boss loop: Ember Watcher gate on Legend difficulty.
    tebf::GateBoss watcher;
    tebf::BossFight fight(watcher, tebf::profile_for(tebf::Difficulty::Legend));
    while (!fight.over()) {
        // autopilot: guard on the 4th, 8th... round; attack while stamina-rich;
        // rest otherwise
        if (fight.round() % 4 == 3) fight.player_guard();
        else if (fight.stamina() >= 40) fight.player_attack();
        else fight.player_rest();
        fight.boss_round();
    }
    const bool woke = fight.won();
    if (woke) prog.tutorial_defeated_legend = true;
    std::printf("boss: %s %s in %d rounds (hero hp %d, stamina %d)\n",
                watcher.name.c_str(), woke ? "VICTORY" : "DEFEAT",
                fight.round(), fight.player_hp(), fight.stamina());

    // map the result onto the world chain
    const size_t total = tebf::zone_count();
    if (prog.tutorial_defeated_legend) tebf::clear_zone(prog, "ember_keep");
    for (size_t i = 0; i < total; ++i) {
        const tebf::ZoneDef* z = tebf::zone(i);
        if (!z) continue;
        if (i == 0 || i == 1 || (i == 2 && prog.tutorial_defeated_legend) || i == 3 || i == 4)
            tebf::clear_zone(prog, z->id);
    }
    std::printf("world: %d/%d zones lit, next pool after ember_keep: %s\n",
                prog.bonfires_lit, (int)total,
                tebf::next_zone("ember_keep").c_str());
    if (!tebf::save_progress(prog, "data/progress.json"))
        std::fprintf(stderr, "world: could not persist progress\n");

    // hidden-boss hint ladder (registry stays as authored)
    if (const tebf::HiddenBoss* um = tebf::find_hidden_boss("the_unmirrored"))
        if (prog.tutorial_defeated_legend)
            std::printf("unlock hint: %s - %s\n", um->name.c_str(), um->unlock_hint.c_str());
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