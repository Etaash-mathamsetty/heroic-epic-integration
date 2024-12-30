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

    char *unixA = (char*)malloc(r);
    r = WideCharToMultiByte(CP_UNIXCP, 0, unixW, -1, unixA, r, NULL, NULL);

    if (!r) return NULL;

    WCHAR *ret = pwine_get_dos_file_name(unixA);
    free(unixA);

    return ret;
}

int wmain(int argc, WCHAR **argv) {
    if(argc < 2) return -1;

    WCHAR* args;
    const WCHAR* exe = argv[1];
    int len = 1;

    for(int i = 2; i < argc; i++)
    {
        len += wcslen(argv[i]) + 1;
    }

    args = calloc(len, sizeof(*args));

    if (!args) return -1;

    for(int i = 2; i < argc; i++)
    {
        wcscat(args, L" ");
        wcscat(args, argv[i]);
    }

    /* handle a unix path */
    if (exe[0] == L'/' || (exe[0] == L'"' && exe[1] == L'/')) {
        WCHAR* temp = convert_to_win32(exe);
        if (!temp) return -1;
        exe = temp;
        HeapFree(GetProcessHeap(), 0, temp);
    }

    printf("executing: %ls %ls\n", exe, args);

    ShellExecuteW(NULL, NULL, exe, args, NULL, SW_SHOWNORMAL);
    free(args);

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    HANDLE process = INVALID_HANDLE_VALUE;

    if(hProcessSnap == INVALID_HANDLE_VALUE) return -1;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if(Process32FirstW(hProcessSnap, &pe32))
    {
        do
        {
            if(wcsstr(exe, pe32.szExeFile))
            {
                process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
                break;
            }

        } while(Process32NextW(hProcessSnap, &pe32));
    }

    if(process == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hProcessSnap);
        return -1;
    }

    DWORD process_exit_code = STILL_ACTIVE;

    //waiting for process termination
    while(process_exit_code == STILL_ACTIVE)
    {
        Sleep(1000);

        if(!GetExitCodeProcess(process, &process_exit_code)) break;
    }

    CloseHandle(process);
    CloseHandle(hProcessSnap);

    return 0;
}
