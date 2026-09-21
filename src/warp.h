#pragma once
#include <string>
#include <vector>

namespace tebf {

// A bonfire warp node — once lit, the player can warp to it from any bonfire.
struct BonfireNode {
    std::string id;                 // unique id, e.g. "bonfire_ashfields"
    std::string zone_id;            // owning zone
    std::string display_name;       // shown in the warp menu
    bool lit = false;
};

// The warp network: all bonfires and their light-state registry.
struct WarpNetwork {
    std::vector<BonfireNode> nodes;

    // Find a node by id. nullptr if absent.
    BonfireNode* find(const std::string& id);
    const BonfireNode* find(const std::string& id) const;

    // Return only the lit nodes (the player can warp to these).
    std::vector<const BonfireNode*> destinations() const;
};

// Build the canonical warp network (hardcoded for alpha).
WarpNetwork build_warp_network();

// Light a bonfire by id. Returns true if found.
bool light_bonfire(WarpNetwork& net, const std::string& id);

} // namespace tebf
