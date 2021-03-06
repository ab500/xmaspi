#include "PatternRunner.h"

#include <stdexcept>
#include <iostream>

#include "CallbackDefinitions.h"
#include "SocketServer.h"

void PatternRunner::RegisterCallbacks(
    std::map<
        unsigned int,
        std::function<void(const SocketCommand&)>>& commandMap)
{
    if (commandMap.count(CallbackDefinitions::PatternRunner::ReadSettings) > 0 ||
        commandMap.count(CallbackDefinitions::PatternRunner::WriteSettings) > 0 ||
        commandMap.count(CallbackDefinitions::PatternRunner::ResetDevice) > 0)
    {
       throw std::runtime_error("Callback already registered."); 
    }

    commandMap[CallbackDefinitions::PatternRunner::ReadSettings] =
        std::bind(&PatternRunner::ReadSettingsCallback,
            this, std::placeholders::_1);
    commandMap[CallbackDefinitions::PatternRunner::WriteSettings] =
        std::bind(&PatternRunner::WriteSettingsCallback,
            this, std::placeholders::_1);
    commandMap[CallbackDefinitions::PatternRunner::ResetDevice] =
        std::bind(&PatternRunner::ResetDeviceCallback,
            this, std::placeholders::_1);
}

void PatternRunner::UnregisterCallbacks(
    std::map<
        unsigned int,
        std::function<void(const SocketCommand&)>>& commandMap)
{
    commandMap.erase(CallbackDefinitions::PatternRunner::ReadSettings);
    commandMap.erase(CallbackDefinitions::PatternRunner::WriteSettings);
    commandMap.erase(CallbackDefinitions::PatternRunner::ResetDevice);
}

void PatternRunner::ReadSettingsCallback(const SocketCommand& command)
{
    std::cout << "ReadSettings command received." << std::endl;
}

void PatternRunner::WriteSettingsCallback(const SocketCommand& command)
{
    std::cout << "WriteSettings command received." << std::endl;
}

void PatternRunner::ResetDeviceCallback(const SocketCommand& command)
{
    std::cout << "ResetDevice command received." << std::endl;
}

void PatternRunner::Start()
{
    // Ensure device is initialized.
    // Start tick thread to submit new
    // LightBoards to it.
}

void PatternRunner::Stop()
{
    // Stop/Join the running tick thread.
}
