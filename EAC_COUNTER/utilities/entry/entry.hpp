#pragma once
#include <stdafx.h>

VOID wait_10_seconds(PVOID StartContext) {
    UNREFERENCED_PARAMETER(StartContext);

    LARGE_INTEGER startTime, currentTime;

    // Get the current system time
    KeQuerySystemTime(&startTime);

    // Calculate the target time (10 seconds from now)
    LARGE_INTEGER targetTime;
    targetTime.QuadPart = startTime.QuadPart + 5 * 10000000; // 10 seconds in 100ns intervals (1 second = 10000000 ns)
    int i = 0;
    // Wait until 10 seconds have passed
    do {
        KeQuerySystemTime(&currentTime);
        BYTE buffer[128];
        SIZE_T bytes_read = 0;
        ULONG64 finalsize = 128;
        process::read_physical((PVOID)process::base_address, buffer, finalsize, &bytes_read); // ignore it is trying to read virtual in a physical read func
        //DbgPrintEx(0, 0, "Base Address: 0x%p\n", process::base_address);
        
        //if (NT_SUCCESS(status)) {
        //    DbgPrintEx(0, 0, "Read %zu bytes successfully from physical address 0x%p\n", bytes_read, buffer);
        //}
        //else {
        //    DbgPrintEx(0, 0, "Failed to read from physical address 0x%p, status: 0x%X\n", status);
        //}
        i++;

    } while (currentTime.QuadPart < targetTime.QuadPart);

    DbgPrint("10 seconds have passed.\n");
}

namespace entry {
	NTSTATUS main() {
        process::attach_to_process(L"explorer.exe");
        process::base_address = process::find_image();
        DbgPrintEx(0, 0, "Target: %ws\n", process::full_process_name);
        //DbgPrintEx(0, 0, "PID: %lu\n", (PVOID)process::pid);
        DbgPrintEx(0, 0, "Base Address: %p\n", (PVOID)process::base_address);

        HANDLE threadHandle = NULL;
        CLIENT_ID clientId;
        OBJECT_ATTRIBUTES objectAttributes;

        // Initialize the object attributes
        InitializeObjectAttributes(&objectAttributes, NULL, 0, NULL, NULL);

        // Create the system thread
        PsCreateSystemThread(
            &threadHandle,                       // Thread handle
            THREAD_ALL_ACCESS,                    // Desired access (you can change this)
            &objectAttributes,                    // Object attributes
            NULL,                                 // Process handle (NULL for current process)
            &clientId,                            // Client ID (optional)
            wait_10_seconds,                      // Start routine
            NULL                                  // Start context (optional)
        );

        return STATUS_SUCCESS;
	}
}