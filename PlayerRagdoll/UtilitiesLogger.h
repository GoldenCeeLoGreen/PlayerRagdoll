#pragma once

#include <string>
#include <fstream>
#include <sstream>

void InitializeLogFile()
{
    std::ofstream file(
        ".\\PlayerRagdoll.log",
        std::ios::trunc
    );

    file.close();
}

template <typename T>
void Log(const T& value)
{
    if (settingDebugLogger)
    {
        std::ofstream file(
            ".\\PlayerRagdoll.log",
            std::ios::app
        );
        file << value << std::endl;
		file.flush();
    }
}

template <typename T>
std::string ToString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
