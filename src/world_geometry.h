#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tebf {

// A rectangular room in a zone.
struct Room {
    std::string id;
    std::string name;
    float x = 0.0f, y = 0.0f;       // center position (world units)
    float w = 10.0f, h = 10.0f;     // half-extents
    bool cleared = false;
    bool has_bonfire = false;
    bool has_boss_exit = false;
};

// A bidirectional path linking two rooms.
struct Path {
    std::string a;                  // room id
    std::string b;                  // room id
    float length = 1.0f;            // traversal cost
    bool locked = false;            // requires a key / cleared flag
};

// Interactable object inside a room (item pickup, lever, door, lore).
struct Interactable {
    std::string id;
    std::string name;
    std::string room_id;
    enum class Type : uint8_t { Item, Lever, Door, Lore, Trader, Bonfire } type = Type::Item;
    bool consumed = false;
    std::string payload_id;         // item id / door target / lore text id
};

// Full world geometry for one zone.
struct WorldGeometry {
    std::string zone_id;
    std::vector<Room> rooms;
    std::vector<Path> paths;
    std::vector<Interactable> interactables;
};

// Resolve (or author-default) the geometry for a zone id.
const WorldGeometry* geometry_for_zone(const std::string& zone_id);

// Flat index of every authored geometry.
size_t geometry_count();
const WorldGeometry* geometry(size_t index);

} // namespace tebf
