#include <stdexcept>

#include "Buffer.h"
#include "Device.h"
#include "FrameBuffer.h"
#include "Layout.h"
#include "Program.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"
#include "Utils.h"

namespace phi {

namespace {

#define PHI_QUOTE(word) #word
#define PHI_LOAD_PROC(name)                                         \
    do {                                                            \
        phi::name = (decltype(phi::name))(loader(PHI_QUOTE(name))); \
        assert(phi::name != nullptr);                               \
        PHI_LOG(INFO, "Loaded procedure '%s'", PHI_QUOTE(name));    \
    } while (0)

void LoadProcedures(const phi::ProcLoader &loader) {
    PHI_LOAD_PROC(glColorMaski);
    PHI_LOAD_PROC(glBlendFuncSeparate);
    PHI_LOAD_PROC(glBlendEquationSeparate);
    PHI_LOAD_PROC(glAttachShader);
    PHI_LOAD_PROC(glBindTexture);
    PHI_LOAD_PROC(glBlitNamedFramebuffer);
    PHI_LOAD_PROC(glCheckNamedFramebufferStatus);
    PHI_LOAD_PROC(glBindFramebuffer);
    PHI_LOAD_PROC(glNamedFramebufferDrawBuffers);
    PHI_LOAD_PROC(glBindVertexBuffer);
    PHI_LOAD_PROC(glCreateVertexArrays);
    PHI_LOAD_PROC(glDeleteVertexArrays);
    PHI_LOAD_PROC(glVertexArrayAttribFormat);
    PHI_LOAD_PROC(glVertexArrayAttribBinding);
    PHI_LOAD_PROC(glBindVertexArray);
    PHI_LOAD_PROC(glEnableVertexArrayAttrib);
    PHI_LOAD_PROC(glDisableVertexAttribArray);
    PHI_LOAD_PROC(glBindBuffer);
    PHI_LOAD_PROC(glDrawArrays);
    PHI_LOAD_PROC(glDrawElements);
    PHI_LOAD_PROC(glUseProgram);
    PHI_LOAD_PROC(glCompileShader);
    PHI_LOAD_PROC(glCreateBuffers);
    PHI_LOAD_PROC(glCreateSamplers);
    PHI_LOAD_PROC(glDeleteSamplers);
    PHI_LOAD_PROC(glBindSampler);
    PHI_LOAD_PROC(glSamplerParameteri);
    PHI_LOAD_PROC(glSamplerParameterf);
    PHI_LOAD_PROC(glCreateFramebuffers);
    PHI_LOAD_PROC(glCreateProgram);
    PHI_LOAD_PROC(glCreateShader);
    PHI_LOAD_PROC(glDeleteBuffers);
    PHI_LOAD_PROC(glDeleteFramebuffers);
    PHI_LOAD_PROC(glDeleteProgram);
    PHI_LOAD_PROC(glDeleteShader);
    PHI_LOAD_PROC(glDeleteTextures);
    PHI_LOAD_PROC(glDetachShader);
    PHI_LOAD_PROC(glGenerateMipmap);
    PHI_LOAD_PROC(glCreateTextures);
    PHI_LOAD_PROC(glGetActiveAttrib);
    PHI_LOAD_PROC(glGetActiveUniform);
    PHI_LOAD_PROC(glGetAttribLocation);
    PHI_LOAD_PROC(glGetIntegerv);
    PHI_LOAD_PROC(glGetProgramInfoLog);
    PHI_LOAD_PROC(glGetProgramInterfaceiv);
    PHI_LOAD_PROC(glGetProgramResourceiv);
    PHI_LOAD_PROC(glGetProgramResourceName);
    PHI_LOAD_PROC(glGetProgramiv);
    PHI_LOAD_PROC(glGetShaderInfoLog);
    PHI_LOAD_PROC(glGetShaderiv);
    PHI_LOAD_PROC(glGetUniformLocation);
    PHI_LOAD_PROC(glLinkProgram);
    PHI_LOAD_PROC(glNamedBufferData);
    PHI_LOAD_PROC(glNamedBufferSubData);
    PHI_LOAD_PROC(glNamedFramebufferTexture);
    PHI_LOAD_PROC(glProgramUniform1fv);
    PHI_LOAD_PROC(glProgramUniform1i);
    PHI_LOAD_PROC(glProgramUniform2fv);
    PHI_LOAD_PROC(glProgramUniform3fv);
    PHI_LOAD_PROC(glProgramUniform4fv);
    PHI_LOAD_PROC(glProgramUniformMatrix3fv);
    PHI_LOAD_PROC(glProgramUniformMatrix4fv);
    PHI_LOAD_PROC(glShaderSource);
    PHI_LOAD_PROC(glTexImage2D);
    PHI_LOAD_PROC(glTexSubImage2D);
}
#undef PHI_LOAD_PROC
#undef PHI_QUOTE

} // namespace

Device::Device(const phi::ProcLoader &loader,
               int viewport_width,
               int viewport_height)
        : m_vao(GL_NONE), m_state() {
    LoadProcedures(loader);
    Shader::AddGlobalInclude("assets/shaders/Layout.h");

    CheckedCall(phi::glCreateVertexArrays, 1, &m_vao);
    SetDepthTest(true);
    SetDepthWrite(true);
    glEnable(GL_SCISSOR_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    SetViewport({ 0, 0, viewport_width, viewport_height });
    SetScissor({ 0, 0, viewport_width, viewport_height });
}

void Device::BlitDepthBuffer(const phi::FrameBuffer *src) {
    const int w = src->GetWidth();
    const int h = src->GetHeight();
    CheckedCall(phi::glBlitNamedFramebuffer, src->GetId(), m_state.default_fbo,
                0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Device::SetDefaultFrameBuffer(GLuint default_fbo) {
    m_state.default_fbo = default_fbo;
}

void Device::Draw(phi::Primitive type, int start, int count) {
    if (!m_state.ibo) {
        CheckedCall(phi::glDrawArrays, (GLenum) type, start, count);
    } else {
#warning "TODO: sometimes incorrect assumption that indices are unsigned ints"
        CheckedCall(phi::glDrawElements, (GLenum) type, count, GL_UNSIGNED_INT,
                    (void *) (intptr_t) start);
    }
}

void Device::BindProgram(const phi::Program *program) {
    if (m_state.program == program) {
        return;
    } else if (!program) {
        CheckedCall(phi::glUseProgram, GL_NONE);
    } else {
        CheckedCall(phi::glUseProgram, program->GetId());
    }
    m_state.program = program;
}

void Device::BindVbo(const phi::Buffer *buffer) {
    if (m_state.vbo == buffer) {
        return;
    } else if (buffer) {
        assert(buffer->GetType() == BufferType::Vertex);
        assert(m_state.layout);
        CheckedCall(phi::glBindVertexBuffer, 0u, buffer->GetId(), 0,
                    m_state.layout->GetStride());
    }
    m_state.vbo = buffer;
}

void Device::BindIbo(const phi::Buffer *buffer) {
    if (m_state.ibo == buffer) {
        return;
    } else if (!buffer) {
        CheckedCall(phi::glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    } else {
        assert(buffer->GetType() == BufferType::Index);
        CheckedCall(phi::glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, buffer->GetId());
    }
    m_state.ibo = buffer;
}

void Device::BindLayout(const phi::Layout *layout) {
    if (m_state.layout == layout) {
        return;
    }
    if (!layout) {
        CheckedCall(phi::glBindVertexArray, GL_NONE);
    } else if (layout != m_state.layout) {
        CheckedCall(phi::glBindVertexArray, layout->GetId());
    }
    m_state.layout = layout;
}

void Device::BindTexture(int texture_unit, const phi::Texture *texture) {
    assert(texture);
    if (m_state.textures[texture_unit] == texture) {
        return;
    }
    CheckedCall(glActiveTexture, GL_TEXTURE0 + texture_unit);
    CheckedCall(phi::glBindTexture, (GLenum) texture->GetType(), texture->GetId());
    m_state.textures[texture_unit] = texture;
}

void Device::BindSampler(int texture_unit, const phi::Sampler *sampler) {
    assert(sampler);
    if (m_state.samplers[texture_unit] == sampler) {
        return;
    }
    CheckedCall(phi::glBindSampler, texture_unit, sampler->GetId());
    m_state.samplers[texture_unit] = sampler;
}

void Device::SetViewport(const phi::Rect2D &viewport) {
    m_viewport = viewport;
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
}

void Device::SetScissor(const phi::Rect2D &scissor) {
    m_scissor = scissor;
    glScissor(scissor.x, scissor.y, scissor.w, scissor.h);
}

void Device::BindFrameBuffer(phi::FrameBuffer *target) {
    if (!target) {
        CheckedCall(phi::glBindFramebuffer, GL_FRAMEBUFFER, m_state.default_fbo);
    } else {
        CheckedCall(phi::glBindFramebuffer, GL_FRAMEBUFFER, target->GetId());
    }
    m_state.fbo = target;
}

const phi::Rect2D &Device::GetViewport() const {
    return m_viewport;
}

const phi::Rect2D &Device::GetScissor() const {
    return m_scissor;
}

void Device::ClearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Device::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Device::SetDepthWrite(bool enabled) {
    m_state.zwrite = enabled;
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}

void Device::SetDepthTest(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    m_state.ztest = enabled;
}

bool Device::GetDepthWrite() const {
    return m_state.zwrite;
}

bool Device::GetDepthTest() const {
    return m_state.ztest;
}

} // namespace phi
