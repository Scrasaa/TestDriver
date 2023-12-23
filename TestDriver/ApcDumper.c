#include <ntifs.h>
#include <ntddk.h>

#include "messages.h"
#include "ApcSignatures.h"

// Function pointers set
void ResolveKernelRoutines()
{
    UNICODE_STRING InitializeApcRoutineName = RTL_CONSTANT_STRING(L"KeInitializeApc");
    KeInitializeApc = (KeInitializeApcFunc)MmGetSystemRoutineAddress(&InitializeApcRoutineName);

    UNICODE_STRING InsertQueueApcRoutineName = RTL_CONSTANT_STRING(L"KeInsertQueueApc");
    KeInsertQueueApc = (KeInsertQueueApcFunc)MmGetSystemRoutineAddress(&InsertQueueApcRoutineName);

    if (!KeInitializeApc || !KeInsertQueueApc)
        DbgBreakPoint();

    DebugMessage("[+] KeInsertQueueApc at 0x%p, KeInitializeApc at 0x%p\n", KeInsertQueueApc, KeInitializeApc);
}


void TestApcRoutine(PVOID SystemArgument1, PVOID SystemArgumenet2, PVOID SystemArgument3, PCONTEXT ContexRecord)
{
    DebugMessage("[+] TestApcRoutine Successfully called!\n");
    UNREFERENCED_PARAMETER(ContexRecord);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgumenet2);
    UNREFERENCED_PARAMETER(SystemArgument3);
}

inline void DumpCurrentThreadApcs()
{
    DebugMessage("[+] Dumping current thread Apcs...\n");
    KThread* CurrentThread = (KThread*)(PsGetCurrentThread());
    PKAPC_STATE ApcState = &CurrentThread->ApcState;

    KeAcquireSpinLockAtDpcLevel(&CurrentThread->ThreadLock);

    // dump kernelmode apc's in the current thread
    for (LIST_ENTRY* CurrentApcListEntry = ApcState->ApcListHead[KernelMode].Flink; CurrentApcListEntry != &ApcState->ApcListHead[KernelMode]; CurrentApcListEntry = CurrentApcListEntry->Flink) {
        KAPC* CurrentApc = (KAPC*)((unsigned __int8*)CurrentApcListEntry - offsetof(KAPC, ApcListEntry));

        // KAPC->Reserved[2] == ApcRoutine
        DebugMessage(("[+] KernelMode Apc: %p\n", CurrentApc->Reserved[2]));
    }

    // dump usermode apc's in the current thread
    for (LIST_ENTRY* CurrentApcListEntry = ApcState->ApcListHead[UserMode].Flink; CurrentApcListEntry != &ApcState->ApcListHead[UserMode]; CurrentApcListEntry = CurrentApcListEntry->Flink) {
        KAPC* CurrentApc = (KAPC*)((unsigned __int8*)CurrentApcListEntry - offsetof(KAPC, ApcListEntry));

        // KAPC->Reserved[2] == ApcRoutine
        DebugMessage(("[+] UserMode Apc: %p\n", CurrentApc->Reserved[2]));
    }

    KeReleaseSpinLockFromDpcLevel(&CurrentThread->ThreadLock);
}
