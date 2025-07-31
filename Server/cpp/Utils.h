#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <zlib.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

class Utils
{
public:
    static std::vector<uint8_t> Compress(const std::vector<uint8_t> &data)
    {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK)
        {
            throw std::invalid_argument("Compress failed");
        }

        zs.next_in = (Bytef *)data.data();
        zs.avail_in = data.size();

        int ret;
        char outbuffer[1500];
        std::vector<uint8_t> outvector;

        do
        {
            zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            ret = deflate(&zs, Z_FINISH);
            if (ret < 0)
            {
                deflateEnd(&zs);
                throw std::runtime_error("deflate failed");
            }

            outvector.insert(outvector.end(), outbuffer, outbuffer + (sizeof(outbuffer) - zs.avail_out));
        } while (ret != Z_STREAM_END);

        deflateEnd(&zs);
        return outvector;
    }

    static std::vector<uint8_t> Decompress(const std::vector<uint8_t> &data)
    {
        uLongf originalSize = data.size() * 4;
        std::vector<uint8_t> decompressedData(originalSize);

        if (uncompress(decompressedData.data(), &originalSize, data.data(), data.size()) != Z_OK)
        {
            throw std::invalid_argument("decompress failed");
        }

        decompressedData.resize(originalSize);
        return decompressedData;
    }

    static std::vector<uint8_t> Sign(const std::vector<uint8_t> &key, const std::vector<uint8_t> &data)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned int len = 0;

        HMAC(EVP_sha256(), key.data(), key.size(), data.data(), data.size(), hash, &len);
        return std::vector<uint8_t>(hash, hash + len);
    }

    static std::vector<uint8_t> GenerateSign(const std::string &secretKey, int32_t timestamp, int32_t salt)
    {
        uint8_t timestampBytes[4];
        uint8_t saltBytes[4];

        timestamp = htonl(timestamp);
        salt = htonl(salt);

        std::vector<uint8_t> secretKeyBytes(secretKey.begin(), secretKey.end());

        std::memcpy(timestampBytes, &timestamp, 4);
        std::vector<uint8_t> signingT = Sign(std::vector<uint8_t>(timestampBytes, timestampBytes + 4), secretKeyBytes);
        
        std::memcpy(saltBytes, &salt, 4);
        return Sign(std::vector<uint8_t>(saltBytes, saltBytes + 4), signingT);
    }

    static const char pad_char = '=';

    static bool is_base64(unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') ||
               (c == '+') ||
               (c == '/');
    }

    static std::string Base64Encode(const std::vector<uint8_t> &src)
    {
        if (src.empty())
        {
            return "";
        }

        size_t output_length = (src.size() / 3) * 4 + (src.size() % 3 ? 4 : 0);
        std::string dst(output_length, '\0');

        size_t di = 0, si = 0;
        size_t n = (src.size() / 3) * 3;

        while (si < n)
        {
            uint32_t val = (static_cast<uint32_t>(src[si]) << 16) |
                           (static_cast<uint32_t>(src[si + 1]) << 8) |
                           static_cast<uint32_t>(src[si + 2]);

            dst[di++] = base64_chars[(val >> 18) & 0x3F];
            dst[di++] = base64_chars[(val >> 12) & 0x3F];
            dst[di++] = base64_chars[(val >> 6) & 0x3F];
            dst[di++] = base64_chars[val & 0x3F];

            si += 3;
        }

        size_t remain = src.size() - si;
        if (remain > 0)
        {
            uint32_t val = (static_cast<uint32_t>(src[si]) << 16);
            if (remain == 2)
            {
                val |= (static_cast<uint32_t>(src[si + 1]) << 8);
            }

            dst[di++] = base64_chars[(val >> 18) & 0x3F];
            dst[di++] = base64_chars[(val >> 12) & 0x3F];

            if (remain == 2)
            {
                dst[di++] = base64_chars[(val >> 6) & 0x3F];
            }
            else
            {
                dst[di++] = pad_char;
            }

            dst[di++] = pad_char;
        }

        return dst;
    }

    static std::vector<uint8_t> Base64Decode(const std::string &src)
    {
        size_t in_len = src.size();
        size_t i = 0;
        size_t j = 0;
        size_t output_length = (in_len / 4) * 3;

        if (src[in_len - 1] == pad_char)
        {
            output_length--;
            if (src[in_len - 2] == pad_char)
            {
                output_length--;
            }
        }

        std::vector<uint8_t> dst(output_length);

        while (i < in_len)
        {
            unsigned char char_array_4[4], char_array_3[3];
            size_t idx = 0;

            for (size_t k = 0; k < 4 && i < in_len; k++)
            {
                if (src[i] == pad_char)
                {
                    char_array_4[k] = 0; // replace padding character with 0
                }
                else if (is_base64(src[i]))
                {
                    char_array_4[k] = static_cast<unsigned char>(src[i]);
                }
                else
                {
                    throw std::invalid_argument("Input is not valid Base64 encoded string!");
                }
                i++;
                idx++;
            }

            char_array_4[0] = static_cast<unsigned char>(strchr(base64_chars, char_array_4[0]) - base64_chars);
            char_array_4[1] = static_cast<unsigned char>(strchr(base64_chars, char_array_4[1]) - base64_chars);
            char_array_4[2] = static_cast<unsigned char>(strchr(base64_chars, char_array_4[2]) - base64_chars);
            char_array_4[3] = static_cast<unsigned char>(strchr(base64_chars, char_array_4[3]) - base64_chars);

            char_array_3[0] = (char_array_4[0] << 2) | (char_array_4[1] >> 4);
            char_array_3[1] = (char_array_4[1] << 4) | (char_array_4[2] >> 2);
            char_array_3[2] = (char_array_4[2] << 6) | char_array_4[3];

            // Only write the bytes if they are valid (based on padding)
            for (size_t k = 0; k < idx - 1; k++)
            {
                if (j < output_length)
                {
                    dst[j++] = char_array_3[k];
                }
            }
        }

        return dst;
    }

    static size_t NextMultiple(size_t length, size_t base)
    {
        size_t power = base;
        while (power < length)
        {
            power *= 2;
        }
        return power;
    }
};

#endif