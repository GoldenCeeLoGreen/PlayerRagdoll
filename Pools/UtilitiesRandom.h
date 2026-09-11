#pragma once

#include <random>

std::random_device rd;
std::mt19937 rng(rd());

inline int Random1To100()
{
    std::uniform_int_distribution<int> dist(1, 100);
    return dist(rng);
}