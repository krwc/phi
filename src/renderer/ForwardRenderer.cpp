#include "ForwardRenderer.h"
#include "Material.h"

#include "device/Buffer.h"
#include "device/Layout.h"
#include "device/Prototypes.h"
#include "device/Shader.h"
#include "device/Utils.h"

#include "scene/Camera.h"
#include "scene/Scene.h"

#include <stdexcept>

namespace phi {
using namespace std;
using namespace glm;

ForwardRenderer::ForwardRenderer(int width, int height)
        : m_proj(), m_vao(GL_NONE) {
    Resize(width, height);
    CheckedCall(phi::glCreateVertexArrays, 1, &m_vao);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void ForwardRenderer::BindGlobals(Program *program,
                                  const glm::mat4 &view,
                                  const glm::mat4 &model) {
    if (!!program->FindConstant("g_MVP")) {
        program->SetConstant("g_MVP", m_proj * view * model);
    }
}

namespace {

int NumAttributeComponents(GLenum type) {
    switch (type) {
    case GL_INT:
    case GL_BOOL:
    case GL_FLOAT:
        return 1;
    case GL_FLOAT_VEC2:
        return 2;
    case GL_FLOAT_VEC3:
        return 3;
    case GL_FLOAT_VEC4:
        return 4;
    default:
        throw invalid_argument("Not yet implemented");
    }
}

}

void ForwardRenderer::BindProgram(Program *program) {
    if (m_last.program == program) {
        return;
    }
    CheckedCall(phi::glUseProgram, program->GetId());
    m_last.program = program;
}

void ForwardRenderer::BindLayout(Program *program,
                                 const Layout *layout) {
    if (m_last.layout == layout) {
        return;
    }
    CheckedCall(phi::glBindVertexArray, m_vao);
    /* Clear enabled attributes state, to prepare for rendering data of
     * different layout */
    for (int enabled_array : m_last.arrays) {
        CheckedCall(phi::glDisableVertexAttribArray, enabled_array);
    }
    m_last.arrays.clear();

    for (const auto &entry : layout->GetEntries()) {
        auto info = program->FindAttribute(entry.name);
        if (!info) {
            PHI_LOG(WARNING, "Ignoring layout <-> shader attribute '%s' as "
                             "shader does not specify it",
                    entry.name.c_str());
            continue;
        }

        m_last.arrays.push_back(info->location);
        CheckedCall(phi::glEnableVertexAttribArray, info->location);
        CheckedCall(phi::glVertexAttribPointer, info->location,
                    NumAttributeComponents(info->type), (GLenum) entry.type,
                    GL_FALSE, entry.stride, (void *) (intptr_t) entry.offset);
    }
    m_last.layout = layout;
}

void ForwardRenderer::BindVbo(Buffer *buffer) {
    if (m_last.vbo == buffer) {
        return;
    }
    assert(buffer);
    assert(buffer->GetType() == BufferType::Vertex);
    CheckedCall(phi::glBindBuffer, GL_ARRAY_BUFFER, buffer->GetId());
}

void ForwardRenderer::BindIbo(Buffer *buffer) {
    if (m_last.ibo == buffer) {
        return;
    } else if (!buffer) {
        CheckedCall(phi::glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
        m_last.ibo = nullptr;
    } else {
        assert(buffer->GetType() == BufferType::Index);
        CheckedCall(phi::glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, buffer->GetId());
    }
}

void ForwardRenderer::Draw(PrimitiveType type, int start, int count) {
    if (!m_last.ibo) {
        CheckedCall(phi::glDrawArrays, (GLenum) type, start, count);
    } else {
#warning "TODO: sometimes incorrect assumption that indices are unsigned ints"
        CheckedCall(phi::glDrawElements, (GLenum) type, count, GL_UNSIGNED_INT,
                    (void *) (intptr_t) start);
    }
}

void ForwardRenderer::Render(phi::Scene *scene) {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    CommandQueue Q;
    scene->Render(&Q);
    auto camera = scene->GetCamera();
    auto view = camera->GetViewMatrix();
    for (const Command &command : Q.GetCommands()) {
        auto program = command.material->GetProgram();
        BindProgram(program);
        BindGlobals(program, view, *command.model);
        command.material->PrepareForRendering();
        BindVbo(command.vbo);
        BindLayout(program, command.layout);
        BindIbo(command.ibo);
        Draw(command.primitive, command.offset, command.count);
    }
    m_last = ForwardRenderer::State{};
}

void ForwardRenderer::Resize(int width, int height) {
    m_proj = perspectiveFov(radians(70.0f), float(width), float(height), 0.1f,
                            10000.0f);
    glViewport(0, 0, width, height);
}

} // namespace phi
