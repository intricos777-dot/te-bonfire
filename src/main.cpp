#include <engine/engine.h>
#include <engine/math_types.h>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

// te-bonfire — a stamina-driven souls-like action RPG core on Twilight Elysium.
// Original code only; the sealed manifest in data/manifest.json points at
// assets you supply from your own copy of a disc game.

namespace {

struct Stats {
    int vit = 10;   // HP scale
    int end = 10;   // stamina scale
    int str = 10;   // melee power
    int dex = 10;   // evade/parry window
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

} // namespace

int run_sim() {
    // identify two distance each other
    Fighter hero;
    hero.swing(8);
    hero.tick(1.0f);
    std::printf("OK sim: hero hp=%d stamina=%d\n", hero.hp, hero.stamina);
    return 0;
}

int main(int argc, char** argv) {
    bool sim = argc > 1 && std::strcmp(argv[1], "--sim") == 0;
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