#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include <Windows.h>
#include <Psapi.h>

#include "Handle.hpp"

#undef max
#undef min

static constexpr DWORD kMaxPids = 100'000; // Warning: Max pids equal (DWORD - 1), but it's 17GB.
static constexpr DWORD kMaxNameSize = MAX_PATH;
SmartHandle<HANDLE> OpenProcessByName(std::wstring_view processName);


int wmain(int argc, wchar_t** argv)
{
    if (argc < 2)
    {
        std::cerr << "You must call: ProcessTerminate.exe your_app_name.exe" << std::endl;
        return -1;
    }

    SmartHandle process = OpenProcessByName(argv[1]);
    if (!process)
    {
        return -1;
    }

    if (TerminateProcess(process, 0))
    {
        std::cout << "Success..." << std::endl;
    }
    else
    {
        std::cerr << "Fail..." << std::endl;
    }

    return 0;
}


SmartHandle<HANDLE> OpenProcessByName(std::wstring_view processName)
{
    SmartHandle<HANDLE> process = nullptr;
    DWORD bytes = 0;

    std::vector<DWORD> pids;
    std::wstring name;

    try
    {
        pids.resize(kMaxPids);
        name.resize(kMaxNameSize);
    }
    catch (const std::bad_alloc&)
    {
        return process;
    }

    if (EnumProcesses(pids.data(), static_cast<DWORD>(pids.capacity()) * sizeof(DWORD), &bytes))
    {
        pids.resize(bytes / sizeof(DWORD));
        for (auto&& pid : pids)
        {
            SmartHandle handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pid);
            if (!handle)
            {
                continue;
            }

            HMODULE mod;
            if (EnumProcessModulesEx(handle, &mod, sizeof(mod), &bytes, LIST_MODULES_ALL))
            {
                GetModuleBaseNameW(handle, mod, name.data(), static_cast<DWORD>(name.size()));
                if (_wcsicmp(processName.data(),name.data()) == 0)
                {
                    process = std::move(handle);
                    break;
                }
            }
        }
    }

    return process;
}
