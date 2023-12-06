#pragma warning (disable : 4100 4047 4024)

#include "testdriver.h"
#include "messages.h"
#include "events.h"
#include "data.h"
#include "communication.h"

// Driver Entry Point
// This function is called when the driver is loaded.
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    // Avoid compiler warning for unused parameter
    UNREFERENCED_PARAMETER(pRegistryPath);

    // Set the driver unload routine
    pDriverObject->DriverUnload = UnloadDriver;

    // Print a debug message to the kernel debugger
    DbgPrintEx(0, 0, "Entry Message: Debug Test!\n");

    DebugMessage("Driver Entry!\n");

    // Register a callback for image load notifications
    PsSetLoadImageNotifyRoutine(ImageLoadCallback);

    // Initialize Unicode strings for device and symbolic link names
    RtlInitUnicodeString(&dev, L"\\Device\\testdriver");
    RtlInitUnicodeString(&dos, L"\\DosDevices\\testdriver");

    // Create a device object and a symbolic link
    IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
    IoCreateSymbolicLink(&dos, &dev);

    // Set the major function handlers for create, close, and device control operations
    pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

    // Set flags for direct I/O and mark device as initialized
    pDeviceObject->Flags |= DO_DIRECT_IO;
    pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}

// Driver Unload Routine
// This function is called when the driver is being unloaded.
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
    // Avoid compiler warning for unused parameter
    UNREFERENCED_PARAMETER(pDriverObject);

    DebugMessage("Driver Unloaded!\n");

    // Unregister the image load callback routine
    PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);

    // Delete the symbolic link created during DriverEntry
    IoDeleteSymbolicLink(&dos);

    // Delete the device object created during DriverEntry
    IoDeleteDevice(pDriverObject->DeviceObject);

    // Return success status
    return STATUS_SUCCESS;
}
