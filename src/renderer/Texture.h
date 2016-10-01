#ifndef PHI_RENDERER_TEXTURE_H
#define PHI_RENDERER_TEXTURE_H
#include <GL/gl.h>

namespace phi {

enum class TextureFormat {
    R_8 = GL_R8,
    R_16F = GL_R16F,
    R_32F = GL_R32F,
    RG_16F = GL_RG16F,
    RG_32F = GL_RG32F,
    RGB_888 = GL_RGB8,
    RGBA_8888 = GL_RGBA8,
    DEPTH_24 = GL_DEPTH_COMPONENT24,
    DEPTH_32 = GL_DEPTH_COMPONENT32
};

enum class TextureType {
    Texture2D,
    TextureArray,
};

struct TextureDesc {
    TextureType type;
    TextureFormat format;
    int depth;
    int width;
    int height;
};

class Texture {
protected:
    GLuint m_bind;
    TextureType m_type;
    TextureFormat m_format;
    int m_depth;
    int m_width;
    int m_height;

public:
    Texture(const TextureDesc &desc);

    virtual ~Texture();

    virtual int GetWidth() const {
        return m_width;
    }

    virtual int GetHeight() const {
        return m_height;
    }

    virtual int GetDepth() const {
        return m_depth;
    }

    virtual TextureFormat GetTextureFormat() const {
        return m_format;
    }

    virtual TextureType GetType() const {
        return m_type;
    }
};

class Texture2D : public Texture {
public:
    Texture2D(int width, int height, TextureFormat format);
    virtual ~Texture2D();
};

} // namespace phi

#endif
