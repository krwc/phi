#include "ShaderPreprocessor.h"
#include "Logging.h"

extern "C" {
#include "mcpp_lib.h"
#include "mcpp_out.h"
}

#include <stdexcept>
#include <vector>
#include <sstream>

namespace phi {

ShaderPreprocessor::ShaderPreprocessor(const std::string &filename,
                                       const std::string &include_dir)
        : m_filename(filename), m_include_dir(include_dir) {}

std::string ShaderPreprocessor::Preprocess() {
    std::vector<const char *> argv = {
        "",
        "-I",
        m_include_dir.c_str(),
        m_filename.c_str()
    };

    std::vector<std::string> define_commands;
    for (const auto &define : m_defines) {
        std::stringstream s;
        s << "-D" << define.first;
        if (define.second.length()) {
            s << "=" << define.second;
        }
        define_commands.emplace_back(s.str());
    }
    for (const auto &define : define_commands) {
        argv.emplace_back(define.c_str());
    }

    for (const auto &undef : m_undefs) {
        argv.emplace_back("-U");
        argv.emplace_back(undef.c_str());
    }
    mcpp_use_mem_buffers(1);
    int retval = mcpp_lib_main(argv.size(), const_cast<char **>(argv.data()));
    char *out = mcpp_get_mem_buffer(OUT);
    char *err = mcpp_get_mem_buffer(ERR);

    if (retval) {
        PHI_LOG(ERROR, "Shader preprocessing failed:\n%s", const_cast<const char*>(err));
        // Free the buffers.
        mcpp_use_mem_buffers(1);
        throw std::invalid_argument("Preprocessing failed");
    }
    // TODO: maybe sometimes one would like to support other versions too
    std::string result = "#version 430\n" + std::string(out);
    // Free the buffers. It may seem strange, but mcpp source investigation leads
    // to conclusion that this is a way to go.
    mcpp_use_mem_buffers(1);
    return result;
}

void ShaderPreprocessor::Define(const std::string &name, const std::string &value) {
    m_defines.insert(std::make_pair(name, value));
}

void ShaderPreprocessor::Undefine(const std::string &name) {
    auto it = m_defines.find(name);
    if (it != m_defines.end()) {
        m_defines.erase(it);
    }
    m_undefs.insert(name);
}

} // namespace phi
