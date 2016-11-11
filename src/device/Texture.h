#ifndef PHI_DEVICE_TEXTURE_H
#define PHI_DEVICE_TEXTURE_H
#include "Prototypes.h"
#include "Resource.h"

namespace phi {

enum class TextureFormat {
    R_8 = GL_R8,
    R_16F = GL_R16F,
    R_32F = GL_R32F,
    RG_16F = GL_RG16F,
    RG_32F = GL_RG32F,
    RGB_888 = GL_RGB8,
    RGBA_8888 = GL_RGBA8,
    RGBA_16F = GL_RGBA16F,
    RGBA_32F = GL_RGBA32F,
    DEPTH_16 = GL_DEPTH_COMPONENT16,
    DEPTH_24 = GL_DEPTH_COMPONENT24,
    DEPTH_32 = GL_DEPTH_COMPONENT32
};

enum class TextureType {
    Texture2D = GL_TEXTURE_2D,
    TextureArray = GL_TEXTURE_2D_ARRAY,
};

struct TextureDesc {
    TextureType type;
    TextureFormat format;
    int depth;
    int width;
    int height;
};

class Texture : virtual public Resource {
private:
    void Destroy();

protected:
    GLuint m_id;
    TextureType m_type;
    TextureFormat m_format;
    int m_depth;
    int m_width;
    int m_height;

public:
    Texture &operator=(const Texture &) = delete;
    Texture(const Texture &) = delete;

    Texture &operator=(Texture &&);
    Texture(Texture &&);
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

    virtual TextureFormat GetFormat() const {
        return m_format;
    }

    virtual TextureType GetType() const {
        return m_type;
    }

    virtual GLuint GetId() const {
        return m_id;
    }

    virtual void GenerateMipmaps() = 0;
};

class Texture2D : public Texture {
public:
    Texture2D &operator=(Texture2D &&) = default;
    Texture2D(Texture2D &&) = default;
    Texture2D(int width, int height, TextureFormat format);
    virtual ~Texture2D();

    virtual void Write(int level, int x, int y, int w, int h, const void *data);
    virtual void GenerateMipmaps();
};

} // namespace phi

#endif
