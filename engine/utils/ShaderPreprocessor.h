#ifndef PHI_UTILS_SHADER_PREPROCESSOR_H
#define PHI_UTILS_SHADER_PREPROCESSOR_H
#include <map>
#include <set>
#include <string>

namespace phi {

class ShaderPreprocessor {
    std::string m_filename;
    std::string m_include_dir;
    std::map<std::string, std::string> m_defines;
    std::set<std::string> m_undefs;
public:
    /**
     * Initializes shader preprocessor.
     *
     * @param source        Path to the shader to preprocess.
     * @param include_dir   Directory from where include requests shall be resolved.
     */
    ShaderPreprocessor(const std::string &filename,
                       const std::string &include_dir = "assets/shaders");

    /**
     * Performs preprocessing on a shader source. Throws a std::logic_error,
     * if source contains errors.
     *
     * @returns preprocessed shader source.
     */
    std::string Preprocess();

    /**
     * Defines constant value used during preprocessing.
     * @param name  Name of the constant to define.
     * @param value Optional value of the constant to define.
     */
    void Define(const std::string &name, const std::string &value = "");

    /**
     * Undefines constant.
     *
     * @param name  Name of the constant to undefine.
     */
    void Undefine(const std::string &name);
};

} // namespace phi

#endif
