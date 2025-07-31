#ifndef APP_TOKEN_OPTIONS_H
#define APP_TOKEN_OPTIONS_H
#include <map>
#include <string>
#include <vector>
class AppTokenOptions
{
public:
    std::map<std::string, std::string> EngineOptions;

    AppTokenOptions()
    {
    }

    void SetEngineOptions(const std::map<std::string, std::string> &engineOptions)
    {
        EngineOptions = engineOptions;
    }

    std::vector<uint8_t> Pack()
    {
        std::vector<uint8_t> buf;

        bool hasEngineOptions = !EngineOptions.empty();
        buf.push_back(static_cast<uint8_t>(1));

        int32_t size = htonl(static_cast<int32_t>(EngineOptions.size()));
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&size), reinterpret_cast<uint8_t *>(&size) + sizeof(size));

        if (hasEngineOptions)
        {
            std::map<std::string, std::string>::iterator it;
            for (it = EngineOptions.begin(); it != EngineOptions.end(); ++it)
            {
                std::string key = it->first;
                std::string value = it->second;

                if (key.empty() || value.empty())
                {
                    throw std::invalid_argument("Illegal engineOptions entry");
                }

                // Key
                int32_t keyLength = htonl(static_cast<int32_t>(key.size()));
                buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&keyLength), reinterpret_cast<uint8_t *>(&keyLength) + sizeof(keyLength));
                buf.insert(buf.end(), key.begin(), key.end());

                // Value
                int32_t valueLength = htonl(static_cast<int32_t>(value.size()));
                buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&valueLength), reinterpret_cast<uint8_t *>(&valueLength) + sizeof(valueLength));
                buf.insert(buf.end(), value.begin(), value.end());
            }
        }
        return buf;
    }
};

#endif