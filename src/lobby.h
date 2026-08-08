#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace tebf {

// Lobby roles (souls-style multiplayer).
// - Allies   : cooperative summons, up to 3
// - Invaders : hostile phantoms, up to 2
// - Recusants: hostile-but-neutral phantoms (fights both sides), up to 3
enum class LobbyRole { Host, Ally, Invader, Recusant };

inline const char* role_name(LobbyRole r) {
    switch (r) {
        case LobbyRole::Host:     return "host";
        case LobbyRole::Ally:     return "ally";
        case LobbyRole::Invader:  return "invader";
        case LobbyRole::Recusant: return "recusant";
    }
    return "?";
}

constexpr int kMaxAllies = 3;
constexpr int kMaxInvaders = 2;
constexpr int kMaxRecusants = 3;

// A remote player in the lobby.
struct LobbyPeer {
    std::string id;
    LobbyRole role = LobbyRole::Ally;
};

// Thread-safe lobby model enforcing the slot brackets. The socket layer
// (server + client) lives in te-net; this is the pure capability model so
// it is fully unit-testable without sockets.
class LobbyModel {
public:
    // Returns true if the role slot is available.
    bool can_join(LobbyRole role) const;

    // Joins the lobby. Returns false if the role is full.
    bool join(const std::string& id, LobbyRole role);
    void leave(const std::string& id);

    int count() const { return (int)m_peers.size(); }
    int count_role(LobbyRole role) const;

    // Invaders may set a target: the host (default). The host is "you".
    void set_target_invader(const std::string& id);
    std::string target_of_invader() const { return "host"; }

    const std::vector<LobbyPeer>& peers() const { return m_peers; }
    std::string summary() const;

private:
    std::vector<LobbyPeer> m_peers;
    int m_target_invader = 0;
};

} // namespace te