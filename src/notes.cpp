#include "notes.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

namespace tebf {

bool NoteStore::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;   // no notes yet is fine
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    // minimal plain-text persistence: author|text|x|y|z per line
    m_notes.clear();
    m_next_id = 1;
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        // fields separated by '\x1f' (unit separator) since text may contain '|'
        size_t i0 = 0;
        auto next = [&](std::string& out) {
            size_t i1 = line.find('\x1f', i0);
            out = line.substr(i0, i1 == std::string::npos ? std::string::npos : i1 - i0);
            i0 = i1 == std::string::npos ? line.size() : i1 + 1;
        };
        Note n;
        next(n.id);
        next(n.author);
        next(n.text);
        if (n.id.empty()) continue;
        m_notes.push_back(std::move(n));
        if (std::strtoull(n.id.c_str(), nullptr, 10) >= m_next_id)
            m_next_id = std::strtoull(n.id.c_str(), nullptr, 10) + 1;
    }
    return true;
}

bool NoteStore::save(const std::string& path) const {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return false;
    for (const auto& n : m_notes)
        f << n.id << '\x1f' << n.author << '\x1f' << n.text << '\n';
    return true;
}

std::string NoteStore::add(const std::string& author, const std::string& text,
                           float x, float y, float z) {
    std::string id = std::to_string(m_next_id++);
    Note n;
    n.id = id;
    n.author = author.empty() ? "traveler" : author;
    n.text = text;
    n.x = x; n.y = y; n.z = z;
    m_notes.push_back(std::move(n));
    return id;
}

bool NoteStore::remove(const std::string& id) {
    auto it = std::remove_if(m_notes.begin(), m_notes.end(),
                             [&](const Note& n) { return n.id == id; });
    if (it == m_notes.end()) return false;
    m_notes.erase(it, m_notes.end());
    return true;
}

void NoteStore::clear() { m_notes.clear(); }

std::string NoteStore::list() const {
    std::ostringstream os;
    for (const auto& n : m_notes)
        os << "  [" << n.id << "] " << n.author << ": " << n.text << "\n";
    return os.str();
}

} // namespace tebf