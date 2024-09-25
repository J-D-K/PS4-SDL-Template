#include "log.hpp"
#include <array>
#include <cstdarg>
#include <fstream>
#include <string>

namespace
{
    const std::string LOG_FILE_PATH = "/data/SDL_Log.txt";
    constexpr int VA_BUFFER_SIZE = 0x1000;
} // namespace

bool logger::initialize(void)
{
    std::ofstream logfile(LOG_FILE_PATH);
    return logfile.is_open();
}

void logger::log(const char *format, ...)
{
    std::array<char, VA_BUFFER_SIZE> vaBuffer;

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    std::ofstream logFile(LOG_FILE_PATH, std::ios::app);
    logFile << vaBuffer.data() << std::endl;
}
