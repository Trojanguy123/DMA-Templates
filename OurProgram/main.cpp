#include <Memory.h>
#include "Menu.h"
#include "Config.h"
#include "UnrealEngine.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

void DMAExample()
{
    if (!mem.Init("TL.exe", false, false))
    {
        std::cerr << "Failed to initialize DMA" << std::endl;
        exit(1);
    }

    std::cout << "DMA initialized" << std::endl;

    if (!mem.GetKeyboard()->InitKeyboard())
    {
        std::cerr << "Failed to initialize keyboard hotkeys through kernel." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    uintptr_t base = mem.GetBaseDaddy("TL.exe");

    int value = 0;
    if (mem.Read(base + 0x66, &value, sizeof(value)))
        std::cout << "Read Value" << std::endl;
    else
        std::cout << "Failed to read Value" << std::endl;
    std::cout << "Value: " << value << std::endl;
}

void ReadGameOffsets()
{
    // Create an instance of our UnrealEngine class
    UnrealEngine ue(mem);
    
    // Dump basic information
    std::cout << "\n--- Unreal Engine Memory Information ---\n";
    ue.DumpInfo();
    
    // If the engine data is valid, try to navigate the object structure
    if (ue.IsValid())
    {
        // Get the PersistentLevel
        uintptr_t persistentLevel = ue.GetPersistentLevel();
        if (persistentLevel != 0)
        {
            std::cout << "\n--- PersistentLevel Information ---\n";
            std::cout << "PersistentLevel Address: 0x" << std::hex << persistentLevel << std::dec << std::endl;
            
            // Get actors from the level
            std::vector<uintptr_t> actors = ue.GetActors();
            std::cout << "Found " << actors.size() << " actors in the level" << std::endl;
            
            // Display information about the first few actors
            int maxActorsToShow = 5;
            if (actors.size() < 5) {
                maxActorsToShow = static_cast<int>(actors.size());
            }
            
            for (int i = 0; i < maxActorsToShow; i++)
            {
                if (actors[i] == 0)
                    continue;
                    
                std::cout << "Actor[" << i << "] Address: 0x" << std::hex << actors[i] << std::dec << std::endl;
                
                // Get the position of the actor
                FVector position = ue.GetActorPosition(actors[i]);
                std::cout << "  Position: X=" << position.X << ", Y=" << position.Y << ", Z=" << position.Z << std::endl;
            }
        }
        
        // Get the GameState
        uintptr_t gameState = ue.GetGameState();
        if (gameState != 0)
        {
            std::cout << "\n--- GameState Information ---\n";
            std::cout << "GameState Address: 0x" << std::hex << gameState << std::dec << std::endl;
            
            // Get players from the game state
            std::vector<uintptr_t> players = ue.GetPlayers();
            std::cout << "Found " << players.size() << " players in the game" << std::endl;
            
            // Display information about all players
            for (size_t i = 0; i < players.size(); i++)
            {
                if (players[i] == 0)
                    continue;
                    
                std::cout << "Player[" << i << "] Address: 0x" << std::hex << players[i] << std::dec << std::endl;
                
                // Get the player name
                std::string playerName = ue.GetPlayerName(players[i]);
                if (!playerName.empty())
                    std::cout << "  Name: " << playerName << std::endl;
            }
        }
        
        // Get the GameInstance
        uintptr_t gameInstance = ue.GetGameInstance();
        if (gameInstance != 0)
        {
            std::cout << "\n--- GameInstance Information ---\n";
            std::cout << "GameInstance Address: 0x" << std::hex << gameInstance << std::dec << std::endl;
            
            // Get local players from the game instance
            std::vector<uintptr_t> localPlayers = ue.GetLocalPlayers();
            std::cout << "Found " << localPlayers.size() << " local players" << std::endl;
            
            // Display information about local players
            for (size_t i = 0; i < localPlayers.size(); i++)
            {
                if (localPlayers[i] == 0)
                    continue;
                    
                std::cout << "LocalPlayer[" << i << "] Address: 0x" << std::hex << localPlayers[i] << std::dec << std::endl;
            }
        }
        
        // Dump memory around GWorld to help with further analysis
        std::cout << "\n--- Memory Dump Around GWorld ---\n";
        ue.DumpMemory(ue.GetGWorld(), 0x100);
        
        // Dump memory around PersistentLevel to help with further analysis
        if (persistentLevel != 0)
        {
            std::cout << "\n--- Memory Dump Around PersistentLevel ---\n";
            ue.DumpMemory(persistentLevel, 0x100);
        }
    }
    else
    {
        std::cout << "\nUnreal Engine data is not fully valid. Let's try to dump some memory around the expected addresses to help debug.\n" << std::endl;
        
        // Dump memory around the GWorld address to help debug
        std::cout << "\n--- Memory Dump Around GWorld Address ---\n";
        ue.DumpMemory(ue.GetGWorld(), 0x100);
    }
}

int main()
{
    Config config;
    if (!ReadConfig(config))
    {
        std::cout << "Continuing with default config values..." << std::endl;
        // We'll continue with the default values set in ReadConfig
    }
    else
    {
        std::cout << "Config loaded successfully" << std::endl;
    }

    // DMA example
    DMAExample();
    std::cout << "DMA example completed" << std::endl;

    // Read game offsets
    std::cout << "\n--- Reading Game Offsets ---\n";
    ReadGameOffsets();
    std::cout << "--- Game Offsets Read Complete ---\n\n";

    std::cout << "Made By F0RSV1NNA" << std::endl;

    // Menu/Overlay(fuser) Example
    Render(config);

    return 0;
}


