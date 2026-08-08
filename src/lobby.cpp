#include "lobby.h"
#include <cstdio>

namespace te {

bool LobbyModel::can_join(LobbyRole role) const {
    switch (role) {
        case LobbyRole::Host: return m_peers.empty();
        case LobbyRole::Ally:     return count_role(LobbyRole::Ally) < kMaxAllies;
        case LobbyRole::Invader:  return count_role(LobbyRole::Invader) < kMaxInvaders;
        case LobbyRole::Recusant: return count_role(LobbyRole::Recusant) < kMaxRecusants;
    }
    return false;
}

int LobbyModel::count_role(LobbyRole role) const {
    int n = 0;
    for (const auto& p : m_peers)
        if (p.role == role) ++n;
    return n;
}

bool LobbyModel::join(const std::string& id, LobbyRole role) {
    if (!can_join(role)) return false;
    for (const auto& p : m_peers)
        if (p.id == id) return false;
    m_peers.push_back({id, role});
    return true;
}

void LobbyModel::leave(const std::string& id) {
    for (size_t i = 0; i < m_peers.size(); ++i) {
        if (m_peers[i].id == id) {
            m_peers.erase(m_peers.begin() + (long)i);
            return;
        }
    }
}

std::string LobbyModel::summary() const {
    char b[96];
    std::snprintf(b, sizeof(b), "lobby: %d peer(s) - allies %d/3, invaders %d/2, recusants %d/3",
                  count(), count_role(LobbyRole::Ally), count_role(LobbyRole::Invader),
                  count_role(LobbyRole::Recusant));
    return b;
}

} // namespace te