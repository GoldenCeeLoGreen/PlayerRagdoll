#pragma once

#include "../../inc/main.h"

bool ConvertIntToBool(int input)
{
    return input == 1;
}

int GetIniInt(const char* key)
{
    return GetPrivateProfileIntA(
        "settings",
        key,
        0,
        ".\\PlayerRagdoll.ini"
    );
}

float GetIniFloat(const char* key)
{
    char buffer[64]{};

    GetPrivateProfileStringA(
        "settings",
        key,
        "0",
        buffer,
        sizeof(buffer),
        ".\\PlayerRagdoll.ini"
    );

    if (buffer[0] == '\0')
        return 0.0f;

    return strtof(buffer, nullptr);
}