#include "ProgramCache.h"

#include <stdexcept>

namespace phi {
using namespace std;

ProgramCache::ProgramCache() : m_cache() {}

void ProgramCache::Insert(const string &name, const shared_ptr<Program> &program) {
    try {
        (void) Get(name);
    } catch (out_of_range &) {
        m_cache[name] = program;
        return;
    }
    throw invalid_argument("Entry already exists. You should use Get() first.");
}

shared_ptr<Program> ProgramCache::Get(const string &name) {
    weak_ptr<Program> entry;
    try {
        entry = m_cache.at(name);
    } catch (out_of_range &) {
        throw out_of_range("No such entry exists.");
    }

    if (auto&& result = entry.lock()) {
        return result;
    } else {
        m_cache.erase(name);
        throw out_of_range("Entry expired.");
    }
}

} // namespace phi
