#ifndef PHI_RENDERER_TEXTURE_BINDING_H
#define PHI_RENDERER_TEXTURE_BINDING_H

namespace phi {

struct TextureBinding {
    const std::string name;
    const phi::Texture2D *const texture;

    TextureBinding(const std::string &name, const phi::Texture2D *texture)
            : name(name), texture(texture) {}
};

} // namespace phi

#endif
