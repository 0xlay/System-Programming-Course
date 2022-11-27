export module FileAPI;

import <vector>;
import <algorithm>;
import <fstream>;
import <string>;


export std::string ReadFile(std::string const& filePath)
{
    std::ifstream fileStream(filePath, std::ios::binary | std::fstream::in);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Can't open file " + filePath);
    }
    std::vector<char> buf(std::istreambuf_iterator<char>(fileStream), {});
    fileStream.close();
    return std::string(buf.begin(), buf.end());
}


export void WriteFile(std::string const& filePath, std::string const& buf)
{
    std::basic_ofstream<char> fileStream(filePath, std::ios::binary);
    fileStream.write(&buf[0], buf.size());
    fileStream.close();
}


export void AppendToFile(std::string const& filePath, std::string const& buf)
{
    std::basic_ofstream<char> fileStream(filePath, std::ios::binary | std::ios::app);
    fileStream.write(&buf[0], buf.size());
    fileStream.close();
}
