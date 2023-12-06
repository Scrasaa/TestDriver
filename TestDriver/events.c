#pragma warning (disable : 4047 4311)

#include "data.h"
#include "events.h"
#include "messages.h"

// Callback function invoked when an image (DLL or executable) is loaded into a process
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessID, PIMAGE_INFO ImageInfo)
{
    // Uncomment the line below if you want to debug and print the loaded image name
    // DebugMessage("ImageLoaded: %ls \n", FullImageName->Buffer);

    // Check if the loaded image is Garry's Mod Client.DLL
    if (wcsstr(FullImageName->Buffer, L"\\Counter-Strike Global Offensive\\game\\csgo\\bin\\win64\\client.dll"))
    {
        DebugMessage("CS2 Client.DLL found!\n");

        // Store the base address of Garry's Mod Client.DLL
        CS2ClientDLLAddy = ImageInfo->ImageBase;
        grabProcessId = (ULONG) ProcessID;

        // Print the ProcessID of the process in which the DLL is loaded
        DebugMessage("ProcessID found!\n");
    }

    // Indicate that the callback was successful
    return STATUS_SUCCESS;
}