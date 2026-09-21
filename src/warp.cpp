#include "warp.h"

namespace tebf {

WarpNetwork build_warp_network() {
    WarpNetwork net;
    net.nodes = {
        {"bonfire_ashfields",   "ashfields",   "Courtyard Bonfire",   true},
        {"bonfire_bth_cistern", "blackthorns", "Cistern Bonfire",     false},
        {"bonfire_ek_vestibule","ember_keep",  "Vestibule Bonfire",   false},
        {"bonfire_sm_brine",    "salt_marches","Brine Bonfire",       false},
        {"bonfire_bg_belfry",   "bell_gate",   "Belfry Bonfire",      false},
    };
    return net;
}

BonfireNode* WarpNetwork::find(const std::string& id) {
    for (auto& n : nodes) if (n.id == id) return &n;
    return nullptr;
}

const BonfireNode* WarpNetwork::find(const std::string& id) const {
    for (const auto& n : nodes) if (n.id == id) return &n;
    return nullptr;
}

std::vector<const BonfireNode*> WarpNetwork::destinations() const {
    std::vector<const BonfireNode*> out;
    for (const auto& n : nodes)
        if (n.lit) out.push_back(&n);
    return out;
}

bool light_bonfire(WarpNetwork& net, const std::string& id) {
    BonfireNode* n = net.find(id);
    if (!n) return false;
    n->lit = true;
    return true;
}

} // namespace tebf
