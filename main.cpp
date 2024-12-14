#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>

#include <iostream>

int main(int argc, char **argv) {
    if(argc < 2) return -1;

    std::string args;
    std::string exe = argv[1];

    for(int i = 2; i < argc; i++)
    {
        args += " ";
        args += argv[i];
    }

    std::cout << "executing: " << exe << " " << args << std::endl;

    ShellExecuteA(NULL, NULL, exe.c_str(), args.c_str(), NULL, SW_SHOWNORMAL);

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    HANDLE process = INVALID_HANDLE_VALUE;

    if(hProcessSnap == INVALID_HANDLE_VALUE) return -1;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if(strstr(exe.c_str(), pe32.szExeFile))
            {
                process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
                break;
            }

        } while(Process32Next(hProcessSnap, &pe32));
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