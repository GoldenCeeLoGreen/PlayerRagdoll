#pragma once

#include <string>
#include <fstream>
#include <sstream>

inline void InitializeLogFile()
{
    std::ofstream file(
        ".\\PlayerRagdoll.log",
        std::ios::trunc
    );

    file.close();
}

template <typename T>
inline void Log(const T& value)
{
    static std::ofstream file(
        ".\\PlayerRagdoll.log",
        std::ios::app
    );

    file << value << std::endl;
    file.flush();
}

template <typename T>
inline std::string ToString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
