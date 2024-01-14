#include <iostream>
#include <thread>
#include <chrono>
#include "kernalinterface.hpp"
#include "offset.hpp"
// 016C8F58 localplayer 

// Causes the local player's team to be spoofed leading to have the nametags for the "enemy" team
void ToggleTeam(KernelInterface& Driver, ULONG ProcessID, ULONGLONG address)
{
    constexpr int TEAM_2 = 2;
    constexpr int TEAM_3 = 3;
    constexpr int TIMEOUT_LIMIT = 64;
    constexpr auto SLEEP_DURATION = std::chrono::milliseconds(50);

    int32_t iSpoofNum = TEAM_3;

    std::cout << "[IMPORTANT] Attention, press the RIGHT Arrow to toggle between teams!\n"
        << "A '.' indicates that 'm_iTeamNum' has not yet been changed, so we're continuously sending WPM until it is.\n"
        << "Remember, the game will behave as if you are on the opposite team when 'spoofed'\n"
        << "meaning all VGUI elements will react accordingly\nIf not already enabled, I recommend putting '+cl_show_team_equipment' in your console.\n\n\nOutput:";

    while (true)
    {
        uintptr_t LocalPlayerAddress = Driver.ReadVirtualMemory<uintptr_t>(ProcessID, address + 0x1810F48, sizeof(uintptr_t)); // localplayer controller is something different lol

        if ((GetAsyncKeyState(VK_RIGHT) & 0x8000)) {
            iSpoofNum = (iSpoofNum == TEAM_2) ? TEAM_3 : TEAM_2;
            std::cout << "\n[!] changing m_iTeamNum -> "
                << Driver.ReadVirtualMemory<int32_t>(ProcessID, LocalPlayerAddress + 0x3BF, sizeof(int32_t))
                << " -> " << iSpoofNum << "\n";
            Beep(1000, 100);
        }

        int iTimeOut = 0;
        while (Driver.ReadVirtualMemory<int32_t>(ProcessID, LocalPlayerAddress + 0x3BF, sizeof(int32_t)) != iSpoofNum && iTimeOut < TIMEOUT_LIMIT) {
            Driver.WriteVirtualMemory<int32_t>(ProcessID, LocalPlayerAddress + 0x3BF, iSpoofNum, sizeof(int32_t));
            std::cout << ".";
            iTimeOut++;
            std::this_thread::sleep_for(SLEEP_DURATION);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Reduce CPU usage
    }
}

int main()
{
    auto Driver = KernelInterface("\\\\.\\testdriver");
    
    ULONGLONG address = Driver.GetClientAddress();
    ULONG ProcessID = Driver.GetProcessID();

    std::cout << "ProcessID: " << ProcessID << std::endl;
    std::cout << "Addy: 0x" << std::hex << address << std::endl;

    uintptr_t LocalPlayerAddress = Driver.ReadVirtualMemory<uintptr_t>(ProcessID, address + 0x016D5C80, sizeof(uintptr_t));
    std::cout << "LocalPlayer Address: 0x" << LocalPlayerAddress << std::endl;

    while (true)
    {
        if (LocalPlayerAddress)
        {
            int32_t iLocalHealth = Driver.ReadVirtualMemory<int32_t>(ProcessID, LocalPlayerAddress + 0x32C, sizeof(int32_t));
            std::cout << "Local Player Health: " << std::dec << iLocalHealth << std::endl;
        }
        Sleep(100);
    }

    return 0;
}