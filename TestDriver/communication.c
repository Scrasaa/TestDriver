#pragma warning (disable : 4022)

#include "communication.h"
#include "messages.h"
#include "data.h"
#include "memory.h"

// Function called when a create operation is requested
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	// Indicate that DeviceObject parameter is not used (avoids compiler warning)
	UNREFERENCED_PARAMETER(DeviceObject);

	// Set the success status and no additional information for the IRP
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Complete the IRP and signal that the create operation is successful
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CreateCall was called, connection established!\n");

	return STATUS_SUCCESS;
}

// Function called when a close operation is requested
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	// Set the success status and no additional information for the IRP
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Complete the IRP and signal that the close operation is successful
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CloseCall was called, connection terminated!\n");

	// Return the status of the operation (success)
	return STATUS_SUCCESS;
}

// Function called for handling IOCTL (Input/Output Control) requests
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    // Initialize status and byte count variables
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    ULONG ByteIO = 0;

    // Get the current stack location in the IRP
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

    // Get the control code from the IOCTL request
    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    // Check the control code
    if (ControlCode == IO_GET_CLIENTADDRESS)
    {
        // Handle the specific IOCTL request (IO_GET_CLIENADDRESS)
        PULONGLONG Output = (PULONGLONG)Irp->AssociatedIrp.SystemBuffer;
        // Copy the address of Garry's Mod Client.DLL to the output buffer
        *Output = CS2ClientDLLAddy;

        DebugMessage("ClientAddress requested!\n");

        // Set the success status and byte count
        Status = STATUS_SUCCESS;
        ByteIO = sizeof(*Output);
    } 
    else if (ControlCode == IO_REQUEST_PROCESSID)
    {
        PULONG Output = (PULONG)Irp->AssociatedIrp.SystemBuffer;

        *Output = grabProcessId;

        DebugMessage("ProcessID requested!\n");

        // Set the success status and byte count
        Status = STATUS_SUCCESS;
        ByteIO = sizeof(*Output);
    }
    else if (ControlCode == IO_READ_REQUEST)
    {
        PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;

        PEPROCESS Process;

        if (NT_SUCCESS(PsLookupProcessByProcessId(ReadInput->ProcessId, &Process)))
        {
            KernelReadVirtualMemory(Process, ReadInput->Address, ReadInput->pBuffer, ReadInput->SIZE);
            Status = STATUS_SUCCESS;
            ByteIO = sizeof(KERNEL_READ_REQUEST);
        }

    }
    else if (ControlCode == IO_WRITE_REQUEST)
    {
        PKERNEL_WRITE_REQUEST WriteInput = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;

        PEPROCESS Process;

        if (NT_SUCCESS(PsLookupProcessByProcessId(WriteInput->ProcessId, &Process)))
        {
            KernelWriteVirtualMemory(Process, WriteInput->pBuffer, WriteInput->Address, WriteInput->SIZE);
            Status = STATUS_SUCCESS;
            ByteIO = sizeof(KERNEL_READ_REQUEST);
        }
    }
    else
    {
        // For unknown control codes, set byte count to 0
        ByteIO = 0;
    }

    // Set the status and byte count fields of the IRP
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = ByteIO;

    // Complete the IRP
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Status;
}