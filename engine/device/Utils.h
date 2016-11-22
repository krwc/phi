#ifndef RENDERER_UTILS_H
#define RENDERER_UTILS_H
#include <cassert>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include "Prototypes.h"
#include "utils/Logging.h"

namespace phi {

static inline void DieOnError(const char *file, const int line) {
    GLenum error = glGetError();
    const char *error_string = "unknown";
    switch (error) {
    case GL_NO_ERROR:
        return;
    case GL_INVALID_ENUM:
        error_string = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        error_string = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        error_string = "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        error_string = "GL_OUT_OF_MEMORY";
        break;
    case GL_STACK_UNDERFLOW:
        error_string = "GL_STACK_UNDERFLOW";
        break;
    case GL_STACK_OVERFLOW:
        error_string = "GL_STACK_OVERFLOW";
        break;
    default:
        break;
    }
    PHI_LOG(ERROR, "OpenGL error %s at %s:%d", error_string, file, line);
    std::terminate();
}

#ifndef NDEBUG
#define CheckedCall(...) _CheckedCall(__FILE__, __LINE__, __VA_ARGS__)

/* Checked call for OpenGL function that does return something. */
template <typename FuncType,
          typename... Args,
          typename RetType = typename std::result_of<FuncType(Args...)>::type>
typename std::enable_if<!std::is_void<RetType>::value, RetType>::type
_CheckedCall(const char *file, const int line, FuncType func, Args &&... args) {
    RetType retval = func(std::forward<Args>(args)...);
    DieOnError(file, line);
    return retval;
}

/* Checked call for OpenGL function that does return void. */
template <typename FuncType,
          typename... Args,
          typename RetType = typename std::result_of<FuncType(Args...)>::type>
typename std::enable_if<std::is_void<RetType>::value, RetType>::type
_CheckedCall(const char *file, const int line, FuncType func, Args &&... args) {
    func(std::forward<Args>(args)...);
    DieOnError(file, line);
}

#else
#define CheckedCall(func, ...) (func)(__VA_ARGS__)
#endif

} // namespace phi

#endif
