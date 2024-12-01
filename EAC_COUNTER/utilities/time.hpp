#pragma once
#include <stdafx.h>

VOID SleepMilliseconds(LONG milliseconds)
{
    LARGE_INTEGER interval;

    // Convert milliseconds to 100-nanosecond intervals (negative for relative time)
    interval.QuadPart = -(LONGLONG)milliseconds * 10 * 1000;

    // Sleep for the specified duration
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}

VOID SleepMicroseconds(LONG microseconds)
{
    LARGE_INTEGER interval;

    // Convert microseconds to 100-nanosecond intervals (negative for relative time)
    interval.QuadPart = -(LONGLONG)microseconds * 10;

    // Sleep for the specified duration
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}