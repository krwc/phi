#ifndef PHI_UTILS_PROGRAM_CACHE_H
#define PHI_UTILS_PROGRAM_CACHE_H
#include <memory>
#include <string>
#include <map>

namespace phi {
class Program;

class ProgramCache {
    std::map<std::string, std::weak_ptr<Program>> m_cache;

public:
    ProgramCache();

    /**
     * Inserts new program into the cache. If a program with given name already
     * exists an exception is thrown.
     */
    void Insert(const std::string &name,
                const std::shared_ptr<Program> &program);

    /**
     * Attempts to get cache program by name. If it doesn't exist then the std::out_of_range
     * exception is thrown (just as in std::map [] operator)
     */
    std::shared_ptr<Program> Get(const std::string &name);
};

} // namespace phi

#endif
