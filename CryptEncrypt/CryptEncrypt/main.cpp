import Cryptography;
import FileAPI;

#include <iostream>
#include <string>
#include <stdexcept>


void PrintHash(std::string_view hash);


int main()
{
    std::string pass = "pass";

    try
    {
        Crypto crypto(pass);

        std::string plainText = ReadFile("original_text.txt");
        std::string originalHash = CalculateHash(plainText);
        std::string cipherText = crypto.encrypt(plainText);
        WriteFile("cipher_text.txt", cipherText);
        AppendToFile("cipher_text.txt", originalHash);

        cipherText = ReadFile("cipher_text.txt");
        std::string hash = FetchHash(cipherText);
        std::string decryptedText = crypto.decrypt(cipherText);
        WriteFile("decrypted_text.txt", decryptedText);
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << ex.what();
    }

    return 0;
}

