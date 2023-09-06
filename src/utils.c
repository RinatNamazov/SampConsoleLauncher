/*****************************************************************************
 *
 *  PROJECT:        SampConsoleLauncher
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           utils.c
 *  DESCRIPTION:    Utils
 *  COPYRIGHT:      (c) 2023 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "utils.h"

#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>

void get_dir_path(const char* full_path, char* dir_path) {
    // Remove the name of the executable file.
    char drive[_MAX_DRIVE], dir[_MAX_DIR];
    _splitpath(full_path, drive, dir, NULL, NULL);
    _makepath(dir_path, drive, dir, NULL, NULL);
}

// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
void print_last_error(char* name) {
    // Retrieve the system error message for the last-error code.
    DWORD  dw = GetLastError();
    LPVOID display_buf;
    LPVOID msg_buf;

    if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                           | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msg_buf, 0,
                       NULL)) {
        fprintf(stderr,
                "%s failed with error %d: *format error message failed with "
                "error %d*\n",
                name, dw, GetLastError());
        return;
    }

    // Print the error message.
    display_buf = LocalAlloc(
        LMEM_ZEROINIT, (lstrlen((LPCTSTR)msg_buf) + lstrlen((LPCTSTR)name) + 40) * sizeof(TCHAR));
    if (display_buf != NULL) {
        StringCchPrintf((LPTSTR)display_buf, LocalSize(display_buf) / sizeof(TCHAR),
                        "%s failed with error %d: %s\n", name, dw, (TCHAR*)msg_buf);
        fprintf(stderr, display_buf);
        LocalFree(display_buf);
    } else {
        fprintf(stderr, "%s failed with error %d\n", name, dw);
    }

    LocalFree(msg_buf);
}
