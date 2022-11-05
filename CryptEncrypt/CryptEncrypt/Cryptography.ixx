module;
#pragma warning (disable : 4996)

export module Cryptography;

import <string>;
import <vector>;
import <stdexcept>;
import <fstream>;

import <openssl/evp.h>;
import <openssl/aes.h>;
import <openssl/sha.h>;


export class Crypto
{
public:
    Crypto(std::string_view password)
    {
        constexpr std::uint32_t kCount = 1;

        m_key.resize(EVP_MAX_KEY_LENGTH);
        m_iv.resize(EVP_MAX_IV_LENGTH);

        m_ctx = EVP_CIPHER_CTX_new();
        if (!m_ctx)
        {
            throw std::runtime_error("EVP_CIPHER_CTX_new failed");
        }

        const EVP_MD* dgst = EVP_get_digestbyname("md5");
        if (!dgst)
        {
            throw std::runtime_error("no such digest");
        }
        
        bool isSuccess = true;
        isSuccess = EVP_BytesToKey(
            EVP_aes_128_cbc(), EVP_md5(),
            nullptr,
            reinterpret_cast<const std::uint8_t*>(password.data()), password.size(),
            kCount, 
            reinterpret_cast<std::uint8_t*>(m_key.data()), reinterpret_cast<std::uint8_t*>(m_iv.data())
        );
        if (!isSuccess)
        {
            throw std::runtime_error("EVP_BytesToKey failed");
        }
    }


    Crypto(const Crypto& rhs)
    {
        m_ctx = EVP_CIPHER_CTX_new();
        if (!m_ctx)
        {
            throw std::runtime_error("EVP_CIPHER_CTX_new failed");
        }
        m_key = rhs.m_key;
        m_iv  = rhs.m_iv;
    }


    Crypto(Crypto&& rhs) noexcept
        : m_ctx(std::move(rhs.m_ctx)), m_key(std::move(rhs.m_key)), m_iv(std::move(rhs.m_iv))
    { }


    ~Crypto()
    {
        if (m_ctx)
        {
            EVP_CIPHER_CTX_free(m_ctx);
        }
    }


    Crypto& operator=(const Crypto& rhs)
    {
        if (this != &rhs)
        {
            m_ctx = EVP_CIPHER_CTX_new();
            if (!m_ctx)
            {
                throw std::runtime_error("EVP_CIPHER_CTX_new failed");
            }
            m_key = rhs.m_key;
            m_iv = rhs.m_iv;
        }
        return *this;
    }


    Crypto& operator=(Crypto&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_ctx, rhs.m_ctx);
            std::swap(m_key, rhs.m_key);
            std::swap(m_iv, rhs.m_iv);
        }
        return *this;
    }


    [[nodiscard]] std::string encrypt(std::string_view plainText)
    {
        bool isSuccess = true;

        //
        // Init
        //

        isSuccess = EVP_EncryptInit_ex(
            m_ctx, EVP_aes_128_cbc(), nullptr, 
            reinterpret_cast<std::uint8_t*>(m_key.data()), reinterpret_cast<std::uint8_t*>(m_iv.data())
        );
        if (!isSuccess)
        {
            throw std::runtime_error("EncryptInit error");
        }

        //
        // Encrypt
        //

        std::string cipherTextBuf(plainText.size() + AES_BLOCK_SIZE, 0);
        int cipherTextSize = 0;
        isSuccess = EVP_EncryptUpdate(
            m_ctx,
            reinterpret_cast<std::uint8_t*>(cipherTextBuf.data()), &cipherTextSize,
            reinterpret_cast<const std::uint8_t*>(plainText.data()), plainText.size()
        );
        if (!isSuccess)
        {
            throw std::runtime_error("Encrypt error");
        }

        //
        // Final
        //

        int lastPartLen = 0;
        isSuccess = EVP_EncryptFinal_ex(m_ctx, reinterpret_cast<std::uint8_t*>(cipherTextBuf.data()) + cipherTextSize, &lastPartLen);
        if (!isSuccess)
        {
            throw std::runtime_error("EncryptFinal error");
        }

        cipherTextBuf.resize(cipherTextSize + lastPartLen);
        return cipherTextBuf;
    }


    [[nodiscard]] std::string decrypt(std::string_view cipherText)
    {
        bool isSuccess = true;

        //
        // Init
        //

        isSuccess = EVP_DecryptInit_ex(
            m_ctx, EVP_aes_128_cbc(), nullptr, 
            reinterpret_cast<std::uint8_t*>(m_key.data()), reinterpret_cast<std::uint8_t*>(m_iv.data())
        );
        if (!isSuccess)
        {
            throw std::runtime_error("DecryptInit error");
        }

        //
        // Decrypt
        //

        std::string plainTextBuf(cipherText.size(), 0);
        int plainTextSize = 0;
        isSuccess = EVP_DecryptUpdate(
            m_ctx,
            reinterpret_cast<std::uint8_t*>(plainTextBuf.data()), &plainTextSize,
            reinterpret_cast<const std::uint8_t*>(cipherText.data()), cipherText.size()
        );
        if (!isSuccess)
        {
            throw std::runtime_error("Decrypt error");
        }

        //
        // Final
        //

        int lastPartLen = 0;
        isSuccess = EVP_DecryptFinal_ex(m_ctx, reinterpret_cast<std::uint8_t*>(plainTextBuf.data()) + plainTextSize, &lastPartLen);
        if (!isSuccess)
        {
            throw std::runtime_error("DecryptFinal error");
        }

        plainTextBuf.resize(plainTextSize + lastPartLen);
        return plainTextBuf;
    }

private:
    EVP_CIPHER_CTX* m_ctx = nullptr;
    std::string     m_key;
    std::string     m_iv;
};


export std::string CalculateHash(std::string const& data)
{
    std::string result(SHA256_DIGEST_LENGTH, '\0');

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(reinterpret_cast<std::uint8_t*>(result.data()), &sha256);

    return result;
}


export std::string FetchHash(std::string& data)
{
    std::string result;
    std::copy(data.begin() + data.size() - SHA256_DIGEST_LENGTH, data.end(), std::back_inserter(result));
    data.resize(data.size() - result.size());
    return result;
}
