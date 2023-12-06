#pragma once

#pragma warning ( disable : 6001 4244)

#include "communication.hpp"

class KernelInterface
{
public:
	HANDLE hDriver;
    // Constructor for the KernelInterface class.
    // Initializes the class with a handle to the driver obtained by opening a file.
    KernelInterface(LPCSTR RegistryPath)
    {
        // Open a handle to the driver using CreateFileA.
        // The handle is stored in the hDriver member variable.
        hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    }

    // Function to retrieve the client address from the kernel driver.
    // Returns 0 if the handle to the driver is invalid or if the IOCTL request fails.
    ULONGLONG GetClientAddress()
    {
        // Check if the handle to the driver is invalid.
        if (hDriver == INVALID_HANDLE_VALUE)
            return 0;

        // Variables to store the client address, bytes transferred, and the result of the IOCTL request.
        ULONGLONG Address;
        DWORD Bytes;

        // Issue an IOCTL request to the driver to get the client address.
        // The result is stored in the 'Address' variable.
        if (DeviceIoControl(hDriver, IO_GET_CLIENTADDRESS, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
        {
            // Return the client address if the IOCTL request is successful.
            return Address;
        }

        return 0;
    }

    ULONG GetProcessID()
    {
        if (hDriver == INVALID_HANDLE_VALUE)
            return 0;

        ULONG ProcessId;
        DWORD Bytes;

        if (DeviceIoControl(hDriver, IO_REQUEST_PROCESSID, &ProcessId, sizeof(ProcessId), &ProcessId, sizeof(ProcessId), &Bytes, NULL))
        {
            // Return the client address if the IOCTL request is successful.
            return ProcessId;
        }

        return 0;
    }

    template <typename type>
    type ReadVirtualMemory(ULONG ProcessId, ULONG ReadAddress, SIZE_T Size)
    {
        type Buffer;

        KERNEL_READ_REQUEST ReadRequest;

        ReadRequest.ProcessId = ProcessId;
        ReadRequest.Address = ReadAddress;
        ReadRequest.pBuffer = &Buffer;
        ReadRequest.SIZE = Size;

        if (DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
        {
            return Buffer;
        }

        return Buffer;
    }

    template <typename type>
    bool WriteVirtualMemory(ULONG ProcessId, ULONG WriteAddress, type WriteValue, SIZE_T Size)
    {
        if (hDriver == INVALID_HANDLE_VALUE)
            return false;

        DWORD Bytes;

        KERNEL_WRITE_REQUEST WriteRequest;

        WriteRequest.ProcessId = ProcessId;
        WriteRequest.Address = WriteAddress;
        WriteRequest.pBuffer = &WriteValue;
        WriteRequest.SIZE = Size;

        if (DeviceIoControl(hDriver, IO_WRITE_REQUEST, &WriteRequest, sizeof(WriteRequest), 0, 0, &Bytes, NULL))
        {
            return true;
        }

        return false;
    }

};