#include "world.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>

namespace tebf {

namespace {
const ZoneDef kZones[] = {
    {"ashfields", "Ashfields"},          // tutorial ground
    {"blackthorns", "Blackthorns"},      // endurance gauntlet
    {"ember_keep", "Ember Keep"},        // first boss door
    {"salt_marches", "Salt Marches"},    // opened by Ember Keep
    {"bell_gate", "Bell Gate"},          // final chain link
};
constexpr size_t kZoneCount = 5;
}

const ZoneDef* zone(size_t index) {
    return index < kZoneCount ? &kZones[index] : nullptr;
}

size_t zone_count() { return kZoneCount; }

const ZoneDef* zone_by_id(const std::string& id) {
    for (size_t i = 0; i < kZoneCount; ++i)
        if (kZones[i].id == id) return &kZones[i];
    return nullptr;
}

std::string next_zone(const std::string& id) {
    for (size_t i = 0; i + 1 < kZoneCount; ++i)
        if (kZones[i].id == id) return kZones[i + 1].id;
    return "";
}

bool zone_cleared(const WorldProgress& p, const std::string& id) {
    if (id.empty()) return false;
    std::istringstream ss(p.cleared);
    std::string w;
    while (std::getline(ss, w, ','))
        if (w == id) return true;
    return false;
}

void clear_zone(WorldProgress& p, const std::string& id) {
    if (zone_cleared(p, id) || !zone_by_id(id)) return;
    if (!p.cleared.empty()) p.cleared += ',';
    p.cleared += id;
    ++p.bonfires_lit;
}

bool load_progress(WorldProgress& p, const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;   // no run yet is fine
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    // plain-text: deaths\x1fbonfires\x1flegend\x1fcleared-ids
    size_t i0 = 0;
    auto next = [&](std::string& out) {
        size_t i1 = s.find('\x1f', i0);
        out = s.substr(i0, i1 == std::string::npos ? std::string::npos : i1 - i0);
        i0 = i1 == std::string::npos ? s.size() : i1 + 1;
    };
    WorldProgress q;
    std::string d, b, l;
    next(d); next(b); next(l);
    if (d.empty()) return false;
    q.deaths = std::atoi(d.c_str());
    q.bonfires_lit = std::atoi(b.c_str());
    q.tutorial_defeated_legend = (l == "1");
    i0 = 0; next(q.cleared);
    p = q;
    return true;
}

bool save_progress(const WorldProgress& p, const std::string& path) {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return false;
    f << p.deaths << '\x1f' << p.bonfires_lit << '\x1f'
      << (p.tutorial_defeated_legend ? "1" : "0") << '\x1f' << p.cleared << '\n';
    return true;
}

} // namespace tebf