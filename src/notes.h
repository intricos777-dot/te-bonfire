#pragma once
#include <string>
#include <vector>

namespace tebf {

// A note a player leaves in the world (souls-style message system).
struct Note {
    std::string id;        // persistent identifier
    std::string author;    // player-supplied name or lobby id
    std::string text;      // player's own written words
    float x = 0, y = 0, z = 0;   // world position (from the lobby host)
    long long created_at = 0;
};

// In-world note store, persisted to data/notes.json (never committed).
// Players add their own text; the game renders notes at their coordinates.
class NoteStore {
public:
    bool load(const std::string& path);
    bool save(const std::string& path) const;

    std::string add(const std::string& author, const std::string& text,
                    float x, float y, float z);
    bool remove(const std::string& id);
    void clear();

    const std::vector<Note>& notes() const { return m_notes; }
    std::string list() const;

private:
    std::vector<Note> m_notes;
    unsigned long long m_next_id = 1;
};

} // namespace te} // namespace te