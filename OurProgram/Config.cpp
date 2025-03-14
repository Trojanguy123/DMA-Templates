#include "Config.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <Windows.h>

bool ReadConfig(Config& config)
{
    // Set default values in case config file can't be found
    config.screenWidth = 1920;
    config.screenHeight = 1080;
    config.exampleBool = false;
    config.exampleFloat = 10.0f;
    config.exampleInt = 2;

    // Try to open config file in current directory
    std::ifstream configFile("config.cfg");
    if (!configFile.is_open())
    {
        // If not found, try in the executable's directory
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string exeDir = std::string(exePath);
        exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));
        std::string configPath = exeDir + "\\config.cfg";
        
        configFile.open(configPath);
        if (!configFile.is_open())
        {
            std::cerr << "Failed to open config.cfg in current directory or executable directory" << std::endl;
            std::cerr << "Using default values instead" << std::endl;
            return false;
        }
    }

    std::string line;
    while (std::getline(configFile, line))
    {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "screenWidth") config.screenWidth = std::stoi(value);
        else if (key == "screenHeight") config.screenHeight = std::stoi(value);
        else if (key == "ExampleBool") config.exampleBool = (value == "1");
        else if (key == "ExampleFloat") config.exampleFloat = std::stof(value);
        else if (key == "ExampleInt") config.exampleInt = std::stoi(value);
    }

    return true;
}

bool SaveConfig(const Config& config)
{
    std::ofstream configFile("config.cfg");
    if (!configFile.is_open())
    {
        std::cerr << "Failed to open config.cfg for writing" << std::endl;
        return false;
    }

    configFile << "screenWidth=" << config.screenWidth << std::endl;
    configFile << "screenHeight=" << config.screenHeight << std::endl;
    configFile << "ExampleBool=" << (config.exampleBool ? "1" : "0") << std::endl;
    configFile << "ExampleFloat=" << config.exampleFloat << std::endl;
    configFile << "ExampleInt=" << config.exampleInt << std::endl;

    return true;
}
