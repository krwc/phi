#ifndef PHI_UTILS_TEXTURE_IMPORTER_H
#define PHI_UTILS_TEXTURE_IMPORTER_H
#include <memory>
#include <string>

#include "device/Texture.h"

namespace phi {

struct TextureImporter {
    static std::unique_ptr<phi::Texture2D> FromFile(const std::string &file);
};

} // namespace phi

#endif
