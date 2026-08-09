#pragma once
#include <cstdint>
#include <string>

namespace tebf {

// The world is a linked chain of bonfire zones. Lighting a zone's bonfire
// clears it and opens the one beyond.
struct ZoneDef {
    std::string id;
    std::string name;
};

const ZoneDef* zone(size_t index);                 // 0..4, nullptr past the end
size_t zone_count();
const ZoneDef* zone_by_id(const std::string& id);
// The zone unlocked once `id` is cleared ("" when the chain is finished).
std::string next_zone(const std::string& id);

// Persistent run state, kept in data/progress.json next to the manifest.
struct WorldProgress {
    int deaths = 0;                 // "die 25 times" - devotion to the dark
    int bonfires_lit = 0;           // "light every checkpoint"
    bool tutorial_defeated_legend = false;  // "beat the tutorial boss on Legend"
    std::string cleared = "";       // comma-joined cleared zone ids
};

bool zone_cleared(const WorldProgress& p, const std::string& id);
void clear_zone(WorldProgress& p, const std::string& id);

bool load_progress(WorldProgress& p, const std::string& path);
bool save_progress(const WorldProgress& p, const std::string& path);

} // namespace tebf