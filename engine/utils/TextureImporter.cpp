#include "TextureImporter.h"
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#include "stb_image.h"
#pragma GCC diagnostic pop
}

namespace phi {

std::unique_ptr<phi::Texture2D> TextureImporter::FromFile(const std::string &file) {
    int w, h, num_components;
    uint8_t *pixels = stbi_load(file.c_str(), &w, &h, &num_components, 3);
    if (!pixels) {
        throw std::runtime_error("Cannot load texture: " + file);
    }
    std::unique_ptr<phi::Texture2D> texture =
            std::make_unique<phi::Texture2D>(w, h,
                                             phi::TextureFormat::RGB_888);
    assert(num_components == 3);
    texture->Write(0, 0, 0, w, h, pixels);
    stbi_image_free(pixels);
    return texture;
}

} // namespace phi
