#include <iostream>
#include <cassert>
#include "SmartHandle.hpp"
#include "Gamble.h"


struct GambleInfo
{
    Gamble& gamble;
    std::string name;
    int predictionValue;
};


DWORD WINAPI AddPrediction(void* args)
{
    assert(args);
    if (args)
    {
        auto gamble = static_cast<GambleInfo*>(args);
        gamble->gamble.MakePrediction(gamble->name, gamble->predictionValue);
    }
    return 0;
}


int main()
{
    constexpr std::uint32_t          kPseudoWorkTime  = 2 * 60'000; // 2 min
    constexpr std::uint32_t          kWaitTime        = 1000;       // 1 sec

    bool                             next             = true;
    std::vector<SmartHandle<HANDLE>> handles;
    Gamble                           gamble;

    while (next)
    {
        GambleInfo gamble_info {
            .gamble = gamble
        };

        std::cout << "Enter your name: ";
        std::getline(std::cin, gamble_info.name);
        std::cout << "Enter prediction value: ";
        std::cin >> gamble_info.predictionValue;

        SmartHandle handle = ::CreateThread(nullptr, 0, &AddPrediction, &gamble_info, 0, nullptr);
        if (!handle)
        {
            std::cerr << "Failed to create thread! Error code: 0x" << std::hex << ::GetLastError() << std::endl;
            return -1;
        }

        std::cout << "Enter '1' if there are other players, otherwise '0': ";
        std::cin >> next;

        std::cin.ignore();
    }

    ::Sleep(kPseudoWorkTime);

    for (auto& handle : handles)
    {
        if (::WaitForSingleObject(handle, kWaitTime) != WAIT_OBJECT_0)
        {
            std::cerr << "Failed to wait object! Error code: 0x" << std::hex << ::GetLastError() << std::endl;
            return -1;
        }
    }

    std::cout << std::endl;
    gamble.showWinner();
    return 0;
}
