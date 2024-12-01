#pragma once
#include <stdafx.h>

NTSTATUS WriteToFile(const wchar_t* filePath, CHAR data[])
{
    UNICODE_STRING uniName;
    RtlInitUnicodeString(&uniName, filePath);

    OBJECT_ATTRIBUTES objAttr;

    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    HANDLE handle;
    IO_STATUS_BLOCK ioStatusBlock;

    NTSTATUS ntstatus = ZwCreateFile(&handle,
        GENERIC_WRITE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);

    if (NT_SUCCESS(ntstatus)) {
        DbgPrintEx(0, 0, "Success opening file for write.\n");

        size_t length = strlen(data); // strlen to get the length of the null-terminated string

        if (length > 0) ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, data, (ULONG)length, NULL, NULL);
    }
    ZwClose(handle);
    return ntstatus;
}

NTSTATUS print_pid() {
    // Convert ULONG PID to CHAR array
    CHAR pidString[20]; // Allocate a buffer large enough to hold the PID string
    RtlStringCbPrintfA(pidString, sizeof(pidString), "%lu", process::pid);

    if (pidString) {
        const wchar_t* filePath = L"\\??\\C:\\process_id.txt";
        return WriteToFile(filePath, pidString);
    }
    else return STATUS_UNSUCCESSFUL;
}