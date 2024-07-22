#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Windows.h>

/*
Note: 1. Obviously this program only runs on Windows.
      2. Didn't use system() because it's slow and not all Windows terminals support some syntax Example: (Old CMD: 1.exe), (New CMD: ./1.exe).
      3. Use fwrite() because it's faster (Maybe not very necessary but who cares).
      4. You can use programs from the Environment Variables. Examples: (gcc, g++, py, nslookup, ping ...)
*/

LPWSTR one_wchar_full_argv(INT argc, LPSTR argv[]);
bool executeProgram(LPWSTR full_argv);

INT main(INT argc, LPSTR argv[])
{   
    LPWSTR full_argv = NULL;
    DOUBLE results = 0;

    LARGE_INTEGER frequency;
    LARGE_INTEGER startTimer;
    LARGE_INTEGER endTimer;

    if (argc < 2)
    {
        fwrite("No parameters were entered.\n", sizeof(char), 28, stdout);
        return 1;
    }

    full_argv = one_wchar_full_argv(argc, argv);

    // Apply Timer.
    QueryPerformanceFrequency(&frequency);

    // Start Timer.
    QueryPerformanceCounter(&startTimer);

    if (executeProgram(full_argv) == FALSE)
    {
        // If failed to open file. Exit.
        fwrite("Couldn't find file.\n", sizeof(char), 20, stdout);
        return 1;
    }

    // End Timer.
    QueryPerformanceCounter(&endTimer);

    // Calculate the time that was taken, And print it.
    results = (DOUBLE)(endTimer.QuadPart - startTimer.QuadPart) / frequency.QuadPart;
    printf("\nSeconds: %.15f\n", results);

    free(full_argv);
    return 0;
}

/*
Input: argc, argv.
Output: Allocated string (wchar_t).
Action: Convert all the argvs to one string.
*/
LPWSTR one_wchar_full_argv(INT argc, LPSTR argv[])
{
    INT index = 0;
    INT jndex = 0;
    INT kndex = 0;
    INT argv_size = 0;
    LPWSTR buffer = NULL;

    for (index = 1; index < argc; index += 1)
    {
        argv_size += strlen(argv[index]) + 3;
    }

    buffer = (LPWSTR)malloc(sizeof(WCHAR) * argv_size);
    if (buffer == NULL)
    {
        fwrite("Failed to allocate memory.\n"
            "try running the program again.\n",
            sizeof(char), 58, stdout);
        exit(1);
    }

    for (index = 1; index < argc; index += 1)
    {
        buffer[kndex++] = L'\"';
        for (jndex = 0; argv[index][jndex] != L'\0'; jndex += 1)
        {
            buffer[kndex++] = (WCHAR)argv[index][jndex];
        }

        buffer[kndex++] = L'\"';
        buffer[kndex++] = L' ';
    }

    buffer[kndex - 1] = L'\0';

    return buffer;
}

/*
Input: argv.
Output: Bool?
Action: Open file, and return results.
*/
bool executeProgram(LPWSTR full_argv)
{
    // Needed Structure that I have no idea what they do. 
    PROCESS_INFORMATION procInfo;
    ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof(STARTUPINFOW));
    startupInfo.cb = sizeof(STARTUPINFOW);

    // Open file.
    if (CreateProcessW(
        NULL,                   // Application name
        full_argv,              // Command line
        NULL,                   // Process security attributes
        NULL,                   // Primary thread security attributes
        false,                  // Handles are not inherited
        0,                      // Creation flags
        NULL,                   // Environment
        NULL,                   // Current directory
        &startupInfo,           // Startup info
        &procInfo               // Process information
    )) 
    {
        // Wait for the program to close.
        WaitForSingleObject(procInfo.hProcess, INFINITE);

        // Close the processes of the program.
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    }

    // If opening file failed.
    else 
    {
        return false;
    }

    // If opening file succeeded.
    return true;
}