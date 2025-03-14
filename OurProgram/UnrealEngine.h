#pragma once
#include <Memory.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Unreal Engine offsets - initial values
constexpr uintptr_t INITIAL_OFFSET_GOBJECTS = 0x998c9b8;  // Updated based on scan results
constexpr uintptr_t INITIAL_OFFSET_GNAMES = 0x9953b68;    // Updated based on scan results
constexpr uintptr_t INITIAL_OFFSET_GWORLD = 0x9676F30;    // Updated based on scan results

// Forward declarations
struct UClass;

// Basic Unreal Engine structures
struct FName
{
    int32_t ComparisonIndex;
    int32_t Number;
};

struct FNameEntry
{
    uint32_t Index;
    char Name[256]; // Simplified for our purposes
};

// TArray structure from Unreal Engine - simplified non-template version
struct FTArrayData
{
    uintptr_t Data;
    int32_t Count;
    int32_t Max;
};

// Vector3 structure
struct FVector
{
    float X;
    float Y;
    float Z;

    FVector()
    {
        X = Y = Z = 0.0f;
    }

    FVector(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
};

// Basic UObject structure
struct UObject
{
    uintptr_t VTable;
    int32_t ObjectFlags;
    uintptr_t InternalIndex;
    UClass* Class;
    FName Name;
    uintptr_t Outer;
};

struct UClass : UObject
{
    // Additional class properties would go here
};

// UWorld structure based on the diagram
struct UWorld
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to PersistentLevel (0x30 based on console output)
    static constexpr uint32_t OFFSET_PERSISTENT_LEVEL = 0x30;
    
    // Offset to GameState
    static constexpr uint32_t OFFSET_GAME_STATE = 0x120; // This is an example, adjust based on actual game
    
    // Offset to OwningGameInstance
    static constexpr uint32_t OFFSET_OWNING_GAME_INSTANCE = 0x180; // This is an example, adjust based on actual game
};

// ULevel structure
struct ULevel
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to Actors array
    static constexpr uint32_t OFFSET_ACTORS = 0x98; // This is an example, adjust based on actual game
};

// AGameStateBase structure
struct AGameStateBase
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to PlayerArray
    static constexpr uint32_t OFFSET_PLAYER_ARRAY = 0x238; // This is an example, adjust based on actual game
};

// UGameInstance structure
struct UGameInstance
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to LocalPlayers array
    static constexpr uint32_t OFFSET_LOCAL_PLAYERS = 0x38; // This is an example, adjust based on actual game
};

// AActor structure
struct AActor
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to RootComponent
    static constexpr uint32_t OFFSET_ROOT_COMPONENT = 0x178; // Updated with correct offset
};

// APawn structure (extends AActor)
struct APawn : AActor
{
    // Additional pawn properties
    
    // Offset to PlayerState
    static constexpr uint32_t OFFSET_PLAYER_STATE = 0x240; // This is an example, adjust based on actual game
    
    // Offset to Controller
    static constexpr uint32_t OFFSET_CONTROLLER = 0x230; // This is an example, adjust based on actual game
};

// APlayerState structure
struct APlayerState
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to PlayerName
    static constexpr uint32_t OFFSET_PLAYER_NAME = 0x300; // This is an example, adjust based on actual game
    
    // Offset to TeamID
    static constexpr uint32_t OFFSET_TEAM_ID = 0x340; // This is an example, adjust based on actual game
};

// USceneComponent structure
struct USceneComponent
{
    uintptr_t VTable;
    // ... other UObject properties ...
    
    // Offset to ComponentToWorld transform
    static constexpr uint32_t OFFSET_COMPONENT_TO_WORLD = 0x210; // Updated with correct offset
};

// FTransform structure
struct FTransform
{
    struct
    {
        float X;
        float Y;
        float Z;
        float W;
    } Rotation;
    
    struct
    {
        float X;
        float Y;
        float Z;
    } Translation;
    
    struct
    {
        float X;
        float Y;
        float Z;
    } Scale3D;
};

class UnrealEngine
{
private:
    Memory& memory;
    uintptr_t baseAddress;
    uintptr_t gObjectsAddress;
    uintptr_t gNamesAddress;
    uintptr_t gWorldAddress;
    uintptr_t gObjectsValue;
    uintptr_t gNamesValue;
    uintptr_t gWorldValue;
    bool gObjectsValid;
    bool gNamesValid;
    bool gWorldValid;
    
    // Current offsets - these can be updated during runtime
    uintptr_t offsetGObjects;
    uintptr_t offsetGNames;
    uintptr_t offsetGWorld;

public:
    UnrealEngine(Memory& mem) : memory(mem), 
                               gObjectsValid(false), 
                               gNamesValid(false), 
                               gWorldValid(false),
                               offsetGObjects(INITIAL_OFFSET_GOBJECTS),
                               offsetGNames(INITIAL_OFFSET_GNAMES),
                               offsetGWorld(INITIAL_OFFSET_GWORLD)
    {
        baseAddress = memory.GetBaseDaddy("TL.exe");
        if (baseAddress == 0)
        {
            std::cerr << "Failed to get base address for TL.exe" << std::endl;
            return;
        }

        std::cout << "[+] Found Base Address for TL.exe at 0x" << std::hex << baseAddress << std::dec << std::endl;

        // Calculate absolute addresses
        gObjectsAddress = baseAddress + offsetGObjects;
        gNamesAddress = baseAddress + offsetGNames;
        gWorldAddress = baseAddress + offsetGWorld;

        // Read memory at these addresses
        std::cout << "Reading GObjects at 0x" << std::hex << gObjectsAddress << std::dec << "..." << std::endl;
        if (memory.Read(gObjectsAddress, &gObjectsValue, sizeof(gObjectsValue)))
        {
            if (gObjectsValue != 0)
            {
                gObjectsValid = true;
                std::cout << "GObjects Value: 0x" << std::hex << gObjectsValue << std::dec << std::endl;
            }
            else
            {
                std::cerr << "GObjects value is null, trying to scan for it..." << std::endl;
                ScanForGObjects();
            }
        }
        else
        {
            std::cerr << "Failed to read GObjects" << std::endl;
        }

        std::cout << "Reading GNames at 0x" << std::hex << gNamesAddress << std::dec << "..." << std::endl;
        if (memory.Read(gNamesAddress, &gNamesValue, sizeof(gNamesValue)))
        {
            if (gNamesValue != 0)
            {
                gNamesValid = true;
                std::cout << "GNames Value: 0x" << std::hex << gNamesValue << std::dec << std::endl;
            }
            else
            {
                std::cerr << "GNames value is null, trying to scan for it..." << std::endl;
                ScanForGNames();
            }
        }
        else
        {
            std::cerr << "Failed to read GNames" << std::endl;
        }

        std::cout << "Reading GWorld at 0x" << std::hex << gWorldAddress << std::dec << "..." << std::endl;
        if (memory.Read(gWorldAddress, &gWorldValue, sizeof(gWorldValue)))
        {
            gWorldValid = true;
            std::cout << "GWorld Value: 0x" << std::hex << gWorldValue << std::dec << std::endl;
        }
        else
        {
            std::cerr << "Failed to read GWorld" << std::endl;
        }

        // Let's try to scan for potential GWorld values
        if (!gWorldValid)
        {
            std::cout << "Attempting to scan for GWorld..." << std::endl;
            ScanForGWorld();
        }
    }

    // Scan for potential GObjects values
    void ScanForGObjects()
    {
        // Try a range of offsets around the expected GObjects offset
        const int scanRange = 0x10000; // 64KB range
        const int step = sizeof(uintptr_t);
        
        for (int offset = -scanRange; offset <= scanRange; offset += step)
        {
            uintptr_t testAddress = gObjectsAddress + offset;
            uintptr_t testValue = 0;
            
            if (memory.Read(testAddress, &testValue, sizeof(testValue)))
            {
                // Check if the value looks like a valid pointer
                if (testValue > 0x10000000 && testValue < 0x7FFFFFFFFFFF)
                {
                    std::cout << "Potential GObjects at offset 0x" << std::hex << offsetGObjects + offset 
                              << " (address: 0x" << testAddress << "): 0x" << testValue << std::dec << std::endl;
                    
                    // Try to read a few values from this potential GObjects
                    uintptr_t testObj = 0;
                    if (memory.Read(testValue, &testObj, sizeof(testObj)))
                    {
                        std::cout << "  - Can read value at +0x0: 0x" << std::hex << testObj << std::dec << std::endl;
                        
                        // If we can read a value, let's use this as our GObjects
                        gObjectsValue = testValue;
                        gObjectsValid = true;
                        
                        // Update the offset for future use
                        offsetGObjects += offset;
                        gObjectsAddress = baseAddress + offsetGObjects;
                        
                        std::cout << "Using this as GObjects. Updated offset to 0x" << std::hex << offsetGObjects << std::dec << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Scan for potential GNames values
    void ScanForGNames()
    {
        // Try a range of offsets around the expected GNames offset
        const int scanRange = 0x10000; // 64KB range
        const int step = sizeof(uintptr_t);
        
        for (int offset = -scanRange; offset <= scanRange; offset += step)
        {
            uintptr_t testAddress = gNamesAddress + offset;
            uintptr_t testValue = 0;
            
            if (memory.Read(testAddress, &testValue, sizeof(testValue)))
            {
                // Check if the value looks like a valid pointer
                if (testValue > 0x10000000 && testValue < 0x7FFFFFFFFFFF)
                {
                    std::cout << "Potential GNames at offset 0x" << std::hex << offsetGNames + offset 
                              << " (address: 0x" << testAddress << "): 0x" << testValue << std::dec << std::endl;
                    
                    // Try to read a few values from this potential GNames
                    uintptr_t testName = 0;
                    if (memory.Read(testValue, &testName, sizeof(testName)))
                    {
                        std::cout << "  - Can read value at +0x0: 0x" << std::hex << testName << std::dec << std::endl;
                        
                        // If we can read a value, let's use this as our GNames
                        gNamesValue = testValue;
                        gNamesValid = true;
                        
                        // Update the offset for future use
                        offsetGNames += offset;
                        gNamesAddress = baseAddress + offsetGNames;
                        
                        std::cout << "Using this as GNames. Updated offset to 0x" << std::hex << offsetGNames << std::dec << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Scan for potential GWorld values
    void ScanForGWorld()
    {
        // Try a range of offsets around the expected GWorld offset
        const int scanRange = 0x10000; // 64KB range
        const int step = sizeof(uintptr_t);
        
        for (int offset = -scanRange; offset <= scanRange; offset += step)
        {
            uintptr_t testAddress = gWorldAddress + offset;
            uintptr_t testValue = 0;
            
            if (memory.Read(testAddress, &testValue, sizeof(testValue)))
            {
                // Check if the value looks like a valid pointer
                if (testValue > 0x10000000 && testValue < 0x7FFFFFFFFFFF)
                {
                    std::cout << "Potential GWorld at offset 0x" << std::hex << offsetGWorld + offset 
                              << " (address: 0x" << testAddress << "): 0x" << testValue << std::dec << std::endl;
                    
                    // Try to read a few values from this potential GWorld
                    uintptr_t testLevel = 0;
                    if (memory.Read(testValue + UWorld::OFFSET_PERSISTENT_LEVEL, &testLevel, sizeof(testLevel)))
                    {
                        std::cout << "  - Can read value at +0x" << std::hex << UWorld::OFFSET_PERSISTENT_LEVEL << ": 0x" << testLevel << std::dec << std::endl;
                        
                        // If we can read a value at offset 0x30, let's use this as our GWorld
                        gWorldValue = testValue;
                        gWorldValid = true;
                        
                        // Update the offset for future use
                        offsetGWorld += offset;
                        gWorldAddress = baseAddress + offsetGWorld;
                        
                        std::cout << "Using this as GWorld. Updated offset to 0x" << std::hex << offsetGWorld << std::dec << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Get the name of an object from the GNames array
    std::string GetNameFromIndex(int32_t index)
    {
        if (!gNamesValid)
            return "GNames not valid";

        if (index < 0 || index > 500000) // Sanity check
            return "Invalid index";

        // In a real implementation, you would navigate the GNames array structure
        // This is a simplified version
        uintptr_t nameEntryAddress = gNamesValue + (index * sizeof(uintptr_t));
        uintptr_t nameEntry = 0;
        
        if (!memory.Read(nameEntryAddress, &nameEntry, sizeof(nameEntry)))
            return "ReadError";

        if (nameEntry == 0)
            return "Null";

        // Read the actual name string
        char name[256] = { 0 };
        if (!memory.Read(nameEntry + 0x10, name, sizeof(name))) // Offset to the actual string data
            return "StringReadError";

        return std::string(name);
    }

    // Get an object from the GObjects array
    UObject GetObject(int32_t index)
    {
        UObject obj = { 0 };
        
        if (!gObjectsValid)
            return obj;

        // Calculate the address of the object in the GObjects array
        uintptr_t objectAddress = gObjectsValue + (index * sizeof(uintptr_t));
        uintptr_t objectPtr = 0;
        
        if (!memory.Read(objectAddress, &objectPtr, sizeof(objectPtr)))
            return obj;
        
        if (objectPtr == 0)
            return obj;
        
        // Read the object data
        memory.Read(objectPtr, &obj, sizeof(obj));
        return obj;
    }

    // Get the GWorld object
    uintptr_t GetGWorld()
    {
        return gWorldValid ? gWorldValue : 0;
    }

    // Get the PersistentLevel from GWorld
    uintptr_t GetPersistentLevel()
    {
        if (!gWorldValid)
            return 0;
            
        uintptr_t persistentLevel = 0;
        if (!memory.Read(gWorldValue + UWorld::OFFSET_PERSISTENT_LEVEL, &persistentLevel, sizeof(persistentLevel)))
            return 0;
            
        return persistentLevel;
    }

    // Get the GameState from GWorld
    uintptr_t GetGameState()
    {
        if (!gWorldValid)
            return 0;
            
        uintptr_t gameState = 0;
        if (!memory.Read(gWorldValue + UWorld::OFFSET_GAME_STATE, &gameState, sizeof(gameState)))
            return 0;
            
        return gameState;
    }

    // Get the OwningGameInstance from GWorld
    uintptr_t GetGameInstance()
    {
        if (!gWorldValid)
            return 0;
            
        uintptr_t gameInstance = 0;
        if (!memory.Read(gWorldValue + UWorld::OFFSET_OWNING_GAME_INSTANCE, &gameInstance, sizeof(gameInstance)))
            return 0;
            
        return gameInstance;
    }

    // Get the Actors array from PersistentLevel
    std::vector<uintptr_t> GetActors()
    {
        std::vector<uintptr_t> actors;
        
        uintptr_t persistentLevel = GetPersistentLevel();
        if (persistentLevel == 0)
            return actors;
            
        // Read the Actors TArray
        FTArrayData actorsArray = { 0 };
        
        if (!memory.Read(persistentLevel + ULevel::OFFSET_ACTORS, &actorsArray, sizeof(actorsArray)))
            return actors;
            
        if (actorsArray.Count <= 0 || actorsArray.Count > 10000) // Sanity check
            return actors;
            
        // Read all actor pointers
        actors.resize(actorsArray.Count);
        memory.Read(actorsArray.Data, actors.data(), actorsArray.Count * sizeof(uintptr_t));
        
        return actors;
    }

    // Get the PlayerArray from GameState
    std::vector<uintptr_t> GetPlayers()
    {
        std::vector<uintptr_t> players;
        
        uintptr_t gameState = GetGameState();
        if (gameState == 0)
            return players;
            
        // Read the PlayerArray TArray
        FTArrayData playerArray = { 0 };
        
        if (!memory.Read(gameState + AGameStateBase::OFFSET_PLAYER_ARRAY, &playerArray, sizeof(playerArray)))
            return players;
            
        if (playerArray.Count <= 0 || playerArray.Count > 100) // Sanity check
            return players;
            
        // Read all player pointers
        players.resize(playerArray.Count);
        memory.Read(playerArray.Data, players.data(), playerArray.Count * sizeof(uintptr_t));
        
        return players;
    }

    // Get the LocalPlayers array from GameInstance
    std::vector<uintptr_t> GetLocalPlayers()
    {
        std::vector<uintptr_t> localPlayers;
        
        uintptr_t gameInstance = GetGameInstance();
        if (gameInstance == 0)
            return localPlayers;
            
        // Read the LocalPlayers TArray
        FTArrayData localPlayersArray = { 0 };
        
        if (!memory.Read(gameInstance + UGameInstance::OFFSET_LOCAL_PLAYERS, &localPlayersArray, sizeof(localPlayersArray)))
            return localPlayers;
            
        if (localPlayersArray.Count <= 0 || localPlayersArray.Count > 4) // Sanity check
            return localPlayers;
            
        // Read all local player pointers
        localPlayers.resize(localPlayersArray.Count);
        memory.Read(localPlayersArray.Data, localPlayers.data(), localPlayersArray.Count * sizeof(uintptr_t));
        
        return localPlayers;
    }

    // Get the player name from a player state
    std::string GetPlayerName(uintptr_t playerState)
    {
        if (playerState == 0)
            return "";
            
        // Read the player name (this is a FString in UE4)
        FTArrayData playerName = { 0 };
        
        if (!memory.Read(playerState + APlayerState::OFFSET_PLAYER_NAME, &playerName, sizeof(playerName)))
            return "";
            
        if (playerName.Count <= 0 || playerName.Count > 64) // Sanity check
            return "";
            
        // Read the actual string data
        std::vector<wchar_t> nameBuffer(playerName.Count);
        if (!memory.Read(playerName.Data, nameBuffer.data(), playerName.Count * sizeof(wchar_t)))
            return "";
            
        // Convert wide string to regular string
        std::string name;
        for (const auto& wc : nameBuffer) {
            if (wc <= 127) {
                name += static_cast<char>(wc);
            }
        }
        return name;
    }

    // Get the world position of an actor
    FVector GetActorPosition(uintptr_t actor)
    {
        FVector position;
        
        if (actor == 0)
            return position;
            
        // Get the root component
        uintptr_t rootComponent = 0;
        if (!memory.Read(actor + AActor::OFFSET_ROOT_COMPONENT, &rootComponent, sizeof(rootComponent)))
            return position;
            
        if (rootComponent == 0)
            return position;
            
        // Read the ComponentToWorld transform
        FTransform componentToWorld;
        if (!memory.Read(rootComponent + USceneComponent::OFFSET_COMPONENT_TO_WORLD, &componentToWorld, sizeof(componentToWorld)))
            return position;
            
        // Return the translation part of the transform
        position.X = componentToWorld.Translation.X;
        position.Y = componentToWorld.Translation.Y;
        position.Z = componentToWorld.Translation.Z;
        
        return position;
    }

    // Get the world position of an actor using RelativeLocation
    FVector GetActorRelativePosition(uintptr_t actor)
    {
        FVector position;
        
        if (actor == 0)
            return position;
            
        // Get the root component
        uintptr_t rootComponent = 0;
        if (!memory.Read(actor + AActor::OFFSET_ROOT_COMPONENT, &rootComponent, sizeof(rootComponent)))
            return position;
            
        if (rootComponent == 0)
            return position;
            
        // Read the RelativeLocation directly
        if (!memory.Read(rootComponent + 0x11C, &position, sizeof(position)))
            return FVector(); // Return zero vector if read fails
            
        return position;
    }

    // Check if an actor is moving
    bool IsActorMoving(uintptr_t actor, float* speedOut = nullptr)
    {
        if (actor == 0)
            return false;
        
        // Get the root component
        uintptr_t rootComponent = 0;
        if (!memory.Read(actor + AActor::OFFSET_ROOT_COMPONENT, &rootComponent, sizeof(rootComponent)))
            return false;
        
        if (rootComponent == 0)
            return false;
        
        // Read the ComponentVelocity
        FVector velocity;
        if (!memory.Read(rootComponent + 0x140, &velocity, sizeof(velocity)))
            return false;
        
        // Calculate speed (magnitude of velocity)
        float speedSquared = velocity.X * velocity.X + velocity.Y * velocity.Y + velocity.Z * velocity.Z;
        
        // If speedOut is provided, store the speed
        if (speedOut != nullptr)
            *speedOut = std::sqrt(speedSquared);
        
        // Consider the actor moving if speed is above threshold
        return speedSquared > 1.0f; // Adjust threshold as needed
    }

    // Check if the engine data is valid
    bool IsValid() const
    {
        return gObjectsValid && gNamesValid && gWorldValid;
    }

    // Dump some basic information about the Unreal Engine state
    void DumpInfo()
    {
        std::cout << "Base Address: 0x" << std::hex << baseAddress << std::dec << std::endl;
        std::cout << "GObjects Address: 0x" << std::hex << gObjectsAddress << std::dec << " (Valid: " << (gObjectsValid ? "Yes" : "No") << ")" << std::endl;
        std::cout << "GNames Address: 0x" << std::hex << gNamesAddress << std::dec << " (Valid: " << (gNamesValid ? "Yes" : "No") << ")" << std::endl;
        std::cout << "GWorld Address: 0x" << std::hex << gWorldAddress << std::dec << " (Valid: " << (gWorldValid ? "Yes" : "No") << ")" << std::endl;
        
        if (gObjectsValid)
            std::cout << "GObjects Value: 0x" << std::hex << gObjectsValue << std::dec << std::endl;
        
        if (gNamesValid)
            std::cout << "GNames Value: 0x" << std::hex << gNamesValue << std::dec << std::endl;
        
        if (gWorldValid)
        {
            std::cout << "GWorld Value: 0x" << std::hex << gWorldValue << std::dec << std::endl;
            
            // Get and display PersistentLevel
            uintptr_t persistentLevel = GetPersistentLevel();
            std::cout << "PersistentLevel: 0x" << std::hex << persistentLevel << std::dec << std::endl;
            
            // Get and display GameState
            uintptr_t gameState = GetGameState();
            std::cout << "GameState: 0x" << std::hex << gameState << std::dec << std::endl;
            
            // Get and display GameInstance
            uintptr_t gameInstance = GetGameInstance();
            std::cout << "GameInstance: 0x" << std::hex << gameInstance << std::dec << std::endl;
        }
    }

    // Dump memory around an address to help with debugging
    void DumpMemory(uintptr_t address, int size = 0x100)
    {
        const int bytesPerRow = 16;
        std::vector<uint8_t> buffer(size);
        
        if (!memory.Read(address, buffer.data(), size))
        {
            std::cerr << "Failed to read memory at 0x" << std::hex << address << std::dec << std::endl;
            return;
        }
        
        std::cout << "Memory dump at 0x" << std::hex << address << ":" << std::endl;
        
        for (int i = 0; i < size; i += bytesPerRow)
        {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << (address + i) << ": ";
            
            // Print hex values
            for (int j = 0; j < bytesPerRow; j++)
            {
                if (i + j < size)
                    std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i + j]) << " ";
                else
                    std::cout << "   ";
            }
            
            std::cout << " | ";
            
            // Print ASCII representation
            for (int j = 0; j < bytesPerRow; j++)
            {
                if (i + j < size)
                {
                    char c = buffer[i + j];
                    std::cout << (c >= 32 && c <= 126 ? c : '.');
                }
                else
                {
                    std::cout << " ";
                }
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::dec; // Reset to decimal
    }

    // Analyze an actor in detail
    void AnalyzeActor(uintptr_t actor)
    {
        if (actor == 0)
        {
            std::cout << "Invalid actor address" << std::endl;
            return;
        }
        
        std::cout << "Analyzing Actor at 0x" << std::hex << actor << std::dec << std::endl;
        
        // Get the root component
        uintptr_t rootComponent = 0;
        if (!memory.Read(actor + AActor::OFFSET_ROOT_COMPONENT, &rootComponent, sizeof(rootComponent)))
        {
            std::cout << "Failed to read RootComponent" << std::endl;
            return;
        }
        
        if (rootComponent == 0)
        {
            std::cout << "Actor has no RootComponent" << std::endl;
            return;
        }
        
        std::cout << "RootComponent: 0x" << std::hex << rootComponent << std::dec << std::endl;
        
        // Read PhysicsVolume
        uintptr_t physicsVolume = 0;
        if (memory.Read(rootComponent + 0xB8, &physicsVolume, sizeof(physicsVolume)))
        {
            std::cout << "PhysicsVolume: 0x" << std::hex << physicsVolume << std::dec << std::endl;
        }
        
        // Read AttachParent
        uintptr_t attachParent = 0;
        if (memory.Read(rootComponent + 0xC0, &attachParent, sizeof(attachParent)))
        {
            std::cout << "AttachParent: 0x" << std::hex << attachParent << std::dec << std::endl;
        }
        
        // Read AttachSocketName
        FName attachSocketName;
        if (memory.Read(rootComponent + 0xC8, &attachSocketName, sizeof(attachSocketName)))
        {
            std::cout << "AttachSocketName Index: " << attachSocketName.ComparisonIndex << std::endl;
        }
        
        // Read AttachChildren
        FTArrayData attachChildren;
        if (memory.Read(rootComponent + 0xD0, &attachChildren, sizeof(attachChildren)))
        {
            std::cout << "AttachChildren Count: " << attachChildren.Count << std::endl;
        }
        
        // Read RelativeLocation
        FVector relativeLocation;
        if (memory.Read(rootComponent + 0x11C, &relativeLocation, sizeof(relativeLocation)))
        {
            std::cout << "RelativeLocation: X=" << relativeLocation.X 
                      << ", Y=" << relativeLocation.Y 
                      << ", Z=" << relativeLocation.Z << std::endl;
        }
        
        // Read RelativeRotation
        struct { float Pitch, Yaw, Roll; } relativeRotation;
        if (memory.Read(rootComponent + 0x128, &relativeRotation, sizeof(relativeRotation)))
        {
            std::cout << "RelativeRotation: Pitch=" << relativeRotation.Pitch 
                      << ", Yaw=" << relativeRotation.Yaw 
                      << ", Roll=" << relativeRotation.Roll << std::endl;
        }
        
        // Read RelativeScale3D
        FVector relativeScale3D;
        if (memory.Read(rootComponent + 0x134, &relativeScale3D, sizeof(relativeScale3D)))
        {
            std::cout << "RelativeScale3D: X=" << relativeScale3D.X 
                      << ", Y=" << relativeScale3D.Y 
                      << ", Z=" << relativeScale3D.Z << std::endl;
        }
        
        // Read ComponentVelocity
        FVector componentVelocity;
        if (memory.Read(rootComponent + 0x140, &componentVelocity, sizeof(componentVelocity)))
        {
            std::cout << "ComponentVelocity: X=" << componentVelocity.X 
                      << ", Y=" << componentVelocity.Y 
                      << ", Z=" << componentVelocity.Z << std::endl;
            
            float speed = std::sqrt(componentVelocity.X * componentVelocity.X + 
                                   componentVelocity.Y * componentVelocity.Y + 
                                   componentVelocity.Z * componentVelocity.Z);
            std::cout << "Speed: " << speed << std::endl;
        }
        
        // Read ComponentToWorld
        FTransform componentToWorld;
        if (memory.Read(rootComponent + 0x210, &componentToWorld, sizeof(componentToWorld)))
        {
            std::cout << "ComponentToWorld Translation: X=" << componentToWorld.Translation.X 
                      << ", Y=" << componentToWorld.Translation.Y 
                      << ", Z=" << componentToWorld.Translation.Z << std::endl;
            
            std::cout << "ComponentToWorld Rotation: X=" << componentToWorld.Rotation.X 
                      << ", Y=" << componentToWorld.Rotation.Y 
                      << ", Z=" << componentToWorld.Rotation.Z 
                      << ", W=" << componentToWorld.Rotation.W << std::endl;
        }
    }
}; 