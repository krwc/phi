#include "File.h"
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace phi {
namespace io {

std::string FileContents(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::invalid_argument("Cannot open: " + path);
    }
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

} // namespace io
} // namespace phi
