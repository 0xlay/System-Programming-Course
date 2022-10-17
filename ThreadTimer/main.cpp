#include <iostream>
#include <Windows.h>
#include <string>
#include "Handle.hpp"


static constexpr wchar_t kVariable[] = L"_THREAD_TIMER_";
static constexpr int     kPause = 5'000;


DWORD WINAPI tick(void* args)
{
    if (args)
    {
        SYSTEMTIME lt{};
        int time = *(static_cast<int*>(args));
        int pause = kPause;

        while(true)
        {
            GetLocalTime(&lt);
            std::cout << lt.wHour << "h " << lt.wMinute << "m " << lt.wSecond << "s" << std::endl;

            if (time < pause)
            {
                pause = time;
            }

            Sleep(pause);
            time -= pause;

            if (time <= 0)
            {
                break;
            }
        }
    }
    return 0;
}


int main()
{
    std::wstring envStr;
    try
    {
        envStr.resize(MAX_PATH, 0);
    }
    catch (const std::bad_alloc& ex)
    {
        std::cerr << ex.what() << std::endl;
        return -1;
    }

    if (std::uint32_t size = GetEnvironmentVariableW(kVariable, envStr.data(), envStr.size()); size > 0 && size <= envStr.size())
    {
        envStr.resize(size);
    }
    else
    {
        std::cerr << "Failed to get environment variable. ErrorCode: 0x" << std::hex << ::GetLastError() << std::endl;
        return -1;
    }

    int time = 0;
    try
    {
        // convert to int, and sec to ms
        time = std::stoi(envStr) * 1'000;
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return -1;
    }

    SmartHandle thread = ::CreateThread(nullptr, 0, tick, &time, 0, nullptr);
    if (!thread)
    {
        std::cerr << "Failed to create thread. ErroCode: 0x" << std::hex << ::GetLastError() << std::endl;
        return -1;
    }

    if(::WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0)
    {
        std::cout << "Success!" << std::endl;
    }
    else
    {
        std::cerr << "WaitForSingleObject was failed. Error code: 0x" << std::hex << ::GetLastError() << std::endl;
    }

    return 0;
}