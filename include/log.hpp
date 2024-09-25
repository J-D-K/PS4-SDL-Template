#pragma once

namespace logger
{
    bool initialize(void);
    void log(const char *format, ...);
} // namespace logger
