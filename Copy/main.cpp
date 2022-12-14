#include <iostream>
#include <vector>
#include "Handle.hpp"


int wmain(int argc, wchar_t** argv)
{
    int status = 0;

    do
    {
        if (argc < 3)
        {
            std::cerr << "Copy.exe source.txt target.txt" << std::endl;
            status = -1;
            break;
        }

        SmartHandle sourceFile = ::CreateFileW(argv[1], GENERIC_READ, FILE_SHARE_READ, nullptr,
                                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (sourceFile == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Failed to open the source file: 0x" << std::hex << ::GetLastError() << std::endl;
            status = -1;
            break;
        }

        SmartHandle targetFile = ::CreateFileW(argv[2], GENERIC_WRITE, 0, nullptr,
                                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (targetFile == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Failed to open the target file: 0x" << std::hex << ::GetLastError() << std::endl;
            status = -1;
            break;
        }

        LARGE_INTEGER li{};
        GetFileSizeEx(sourceFile, &li);

        try
        {
            std::vector<char> sourceBuf(li.QuadPart, 0);
            if (!ReadFile(sourceFile, sourceBuf.data(), sourceBuf.size(), nullptr, nullptr))
            {
                std::cerr << "Failed to read data from the source file: 0x" << std::hex << ::GetLastError() << std::endl;
                status = -1;
                break;
            }

            if (!WriteFile(targetFile, sourceBuf.data(), sourceBuf.size(), nullptr, nullptr))
            {
                std::cerr << "Failed to write data into the target file: 0x" << std::hex << ::GetLastError() << std::endl;
                status = -1;
                break;
            }
        }
        catch(const std::bad_alloc& ex)
        {
            std::cerr << ex.what() << std::endl;
            status = -1;
            break;
        }

        std::cout << "Success" << std::endl;

    } while (false);
    
    return status;
}
