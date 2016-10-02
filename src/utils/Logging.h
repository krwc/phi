#ifndef PHI_UTILS_LOGGING_H
#define PHI_UTILS_LOGGING_H
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace phi {

enum LogLevel { TRACE, INFO, DEBUG, WARNING, ERROR };

static const char *RootdirRelativeFilename(const char *filename) {
    static const char SRC_PREFIX[] = { 's', 'r', 'c', '/', '\0' };
    static const long SRC_PREFIX_LEN = sizeof(SRC_PREFIX) - 1;
    const size_t flen = strlen(filename);
    for (const char *ch = filename; ch && *ch; ++ch) {
        if (!strncmp(ch, SRC_PREFIX,
                     std::min(SRC_PREFIX_LEN, filename + flen - ch))) {
            return ch + SRC_PREFIX_LEN;
        }
    }
    return filename;
}

static inline void LogMethod(LogLevel level,
                             const char *file,
                             const int line,
                             const char *format,
                             ...) {
    static const char *levels[] = { "TRACE", "INFO", "DEBUG", "WARNING",
                                    "ERROR" };
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[%s](%s:%d) ", levels[static_cast<int>(level)],
            RootdirRelativeFilename(file), line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

#define PHI_LOG(level, ...) \
    LogMethod((level), __FILE__, __LINE__, __VA_ARGS__)

} // namespace

#endif
