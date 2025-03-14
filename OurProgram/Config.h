#pragma once
#include <string>

struct Config
{
    int screenWidth;
    int screenHeight;
    bool exampleBool;
    float exampleFloat;
    int exampleInt;
};

bool ReadConfig(Config& config);
bool SaveConfig(const Config& config);
