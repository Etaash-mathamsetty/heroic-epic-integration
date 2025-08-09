#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <stdio.h>

#define CP_UNIXCP     65010 /* Wine extension */

WCHAR* convert_to_win32(const WCHAR *unixW) {
    typedef WCHAR* (CDECL *wine_get_dos_file_name_PFN)(const char* path);

    HMODULE k32 = LoadLibraryW(L"kernel32");
    wine_get_dos_file_name_PFN pwine_get_dos_file_name =
        (wine_get_dos_file_name_PFN)GetProcAddress(k32, "wine_get_dos_file_name");

    if (!pwine_get_dos_file_name) return NULL;

    int r = WideCharToMultiByte(CP_UNIXCP, 0, unixW, -1, NULL, 0, NULL, NULL);

    if (!r) return NULL;

    char *unixA = malloc(r);

    if (!unixA) return NULL;

    r = WideCharToMultiByte(CP_UNIXCP, 0, unixW, -1, unixA, r, NULL, NULL);

    if (!r) return NULL;

    WCHAR *ret = pwine_get_dos_file_name(unixA);
    free(unixA);

    return ret;
}

int wmain(int argc, WCHAR **argv) {
    if (argc < 2) return -1;

    WCHAR* args;
    const WCHAR* exe = argv[1];
    WCHAR* temp = NULL;
    int len = 1;
    SHELLEXECUTEINFOW info = {0};

    for (int i = 2; i < argc; i++)
    {
        len += wcslen(argv[i]) + 1;
    }

    args = calloc(len, sizeof(*args));

    if (!args) return -1;

    for (int i = 2; i < argc; i++)
    {
        wcscat(args, L" ");
        wcscat(args, argv[i]);
    }

    /* handle a unix path */
    if (exe[0] == L'/' || (exe[0] == L'"' && exe[1] == L'/')) {
        temp = convert_to_win32(exe);
        if (!temp) return -1;
        exe = temp;
    }

    printf("Wrapper: Executing: %ls %ls\n", exe, args);

    /* guess and set working directory */
    {
        WCHAR *exe_cpy = calloc(wcslen(exe)+1, sizeof(*exe));
        wcscpy(exe_cpy, exe);
        WCHAR *last = wcsrchr(exe_cpy, '\\');
        if (last)
        {
            *last = L'\0';
            printf("Wrapper: Working dir: %ls\n", exe_cpy);
            SetCurrentDirectoryW(exe_cpy);
        }
        free(exe_cpy);
    }

    SetEnvironmentVariable("SteamAppId", NULL);

    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.nShow = SW_SHOWNORMAL;
    info.lpFile = exe;
    info.lpParameters = args;

    if (!ShellExecuteExW(&info)) return -1;
    free(args);

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (!info.hProcess) return -1;

    Sleep(500);

    printf("Wrapper: Waiting for process termination...\n");
    WaitForSingleObject(info.hProcess, INFINITE);
    printf("Wrapper: Process terminated!\n");

    CloseHandle(info.hProcess);

    if (temp) HeapFree(GetProcessHeap(), 0, temp);

    return 0;
}
