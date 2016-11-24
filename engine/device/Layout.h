#ifndef PHI_DEVICE_LAYOUT_H
#define PHI_DEVICE_LAYOUT_H
#include <initializer_list>
#include <string>
#include <vector>

#include "Prototypes.h"
#include "Resource.h"

namespace phi {

/**
 * This class purpose is to connect vertex attributes to a specified shader program
 * input variables.
 *
 * For example, let's analyze following GLSL code:
 * @code
 *  layout(location = POSITION) in vec4 Position;
 *  layout(location = NORMAL) in vec3 Normal;
 *  ...
 *
 * Assume also that we're working with following vertex format structure:
 * @code
 *  struct Vertex {
 *      glm:;vec3 random_data;
 *      glm::vec4 position;
 *      glm::vec3 normal;
 *  };
 *
 * Now, we can specify our Vertex layout via this class instance:
 * @code
 *  const phi::Layout layout(sizeof(Vertex), {
 *      { phi::Layout::Position, phi::Layout::Float4, offsetof(Vertex, position) },
 *      { phi::Layout::Normal  , phi::Layout::Float3, offsetof(Vertex, normal) }
 *  };
 *
 * And at this point, we are ready to rendering with VBO containing some Vertex
 * instances in it.
 *
 * For more information refer to @ref phi::Device::BindLayout .
 */
class Layout final : public Resource {
public:
    enum Type {
        Float,
        Float2,
        Float3,
        Float4
    };

    /* NOTE: this should be keep up synchronized within assets/shaders/Layout.h */
    enum Location {
        Position = 0u,
        Normal   = 1u
    };

    struct Entry {
        Layout::Location location;
        Layout::Type type;
        unsigned offset;
    };

    Layout(size_t stride, std::initializer_list<Layout::Entry> layout);
    virtual ~Layout();
    GLuint GetId() const;
    size_t GetStride() const;

private:
    GLuint m_id;
    size_t m_stride;
};

} // namespace phi

#endif
