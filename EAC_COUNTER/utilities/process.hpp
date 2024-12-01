#pragma once
#include <stdafx.h>

#define ImageFileName 0x5A8 // EPROCESS::ImageFileName
#define ActiveThreads 0x5F0 // EPROCESS::ActiveThreads
#define ThreadListHead 0x5E0 // EPROCESS::ThreadListHead
#define ActiveProcessLinks 0x448 // EPROCESS::ActiveProcessLinks

template <typename str_type, typename str_type_2>
__forceinline bool crt_strcmp(str_type str, str_type_2 in_str, bool two)
{
    if (!str || !in_str)
        return false;

    wchar_t c1, c2;
#define to_lower(c_char) ((c_char >= 'A' && c_char <= 'Z') ? (c_char + 32) : c_char)

    do
    {
        c1 = *str++; c2 = *in_str++;
        c1 = to_lower(c1); c2 = to_lower(c2);

        if (!c1 && (two ? !c2 : 1))
            return true;

    } while (c1 == c2);

    return false;
}
extern "C" PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);

namespace process {
    const wchar_t* full_process_name;
    ULONG pid = 0;
    uintptr_t base_address;

    HANDLE get_process_id_by_name(const wchar_t* process_name)
    {
        CHAR image_name[15];
        PEPROCESS sys_process = PsInitialSystemProcess;
        PEPROCESS cur_entry = sys_process;

        do
        {
            RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + ImageFileName), sizeof(image_name));

            if (crt_strcmp(image_name, process_name, true))
            {
                DWORD active_threads;
                RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + ActiveThreads), sizeof(active_threads));

                if (active_threads)
                    return qtx_import(PsGetProcessId)(cur_entry);
            }

            PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+ActiveProcessLinks);
            cur_entry = (PEPROCESS)((uintptr_t)list->Flink - ActiveProcessLinks);

        } while (cur_entry != sys_process);

        return 0;
    }
    NTSTATUS attach_to_process(const wchar_t* process_name) {
        while (true) {
            process::full_process_name = process_name;
            process::pid = (ULONG)(ULONG_PTR)(get_process_id_by_name(process::full_process_name));
            if (process::pid == 0) SleepMilliseconds(5000);
            else break; 
        }
        return STATUS_SUCCESS;
    }
    ULONGLONG find_image() {
        PEPROCESS processs = NULL;
        if (process::pid == 0) return 0;
        PsLookupProcessByProcessId((HANDLE)process::pid, &processs);
        if (!processs) {
            return 0;
        }
        ULONGLONG baseimg = (ULONGLONG)PsGetProcessSectionBaseAddress(processs);
        if (!baseimg) {
            return 0;
        }
        ObDereferenceObject(processs);
        return baseimg;
    }
    NTSTATUS read_physical(PVOID target_address,
        PVOID buffer,
        SIZE_T size,
        SIZE_T* bytes_read) 
    {
        MM_COPY_ADDRESS to_read = { 0 };
        to_read.PhysicalAddress.QuadPart = (LONGLONG)target_address;
        return qtx_import(MmCopyMemory)(buffer, to_read, size, MM_COPY_MEMORY_PHYSICAL, bytes_read);
    }
}
