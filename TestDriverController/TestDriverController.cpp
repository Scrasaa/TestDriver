#include <iostream>
#include "kernalinterface.hpp"
#include "offset.hpp"
// CBasePlayer + m_iHealth

// client + 0x016CF800
// or client dll + 0x16C2B18

int main()
{
    auto Driver = KernelInterface("\\\\.\\testdriver");
    
    ULONGLONG address = Driver.GetClientAddress();
    ULONG ProcessID = Driver.GetProcessID();

    std::cout << "ProcessID: " << ProcessID << std::endl;
    std::cout << "Addy: 0x" << std::hex << address << std::endl;

    uintptr_t LocalPlayerAddress = Driver.ReadVirtualMemory<uintptr_t>(ProcessID, address + 0x16CF800, sizeof(uintptr_t));
    std::cout << "LocalPlayer Address: 0x" << LocalPlayerAddress << std::endl;

    if (LocalPlayerAddress)
    {
        std::cout << "Local Player Addy valid!\n";
        int32_t iLocalHealth = Driver.ReadVirtualMemory<int32_t>(ProcessID, LocalPlayerAddress + 0x32C, sizeof(int32_t));
        std::cout << "Local Player Health: " << iLocalHealth << std::endl;
        Driver.WriteVirtualMemory(ProcessID, (LocalPlayerAddress + C_CSPlayerPawnBase::m_flFlashDuration), 0.f, sizeof(0.f));
    }

    while (true)
    {
        

        
    }

    return 0;
}