#ifndef APP_TOKEN_H
#define APP_TOKEN_H
#include <vector>
#include <string>

#include "AppTokenOptions.h"
#include "Utils.h"
#include "Service.h"

class AppToken
{
public:
    std::string AppId;
    std::string AppKey;
    int32_t Salt;
    int32_t IssueTimestamp;
    int32_t Timestamp;
    Service *TokenService;
    AppTokenOptions *TokenOptions;

    AppToken(const std::string &appId, const std::string &appKey, const std::int32_t timestamp)
        : AppId(appId), AppKey(appKey), Timestamp(timestamp)
    {
        Salt = static_cast<int32_t>(rand());
        IssueTimestamp = static_cast<int32_t>(std::time(0));
        TokenOptions = nullptr;
        TokenService = nullptr;
    }

    static AppToken CreateAppToken(const std::string &appId, const std::string &appKey, const std::int32_t timestamp)
    {
        return AppToken(appId, appKey, timestamp);
    }

    ~AppToken()
    {   
    }

    void SetService(Service *service)
    {
        TokenService = service;
    }
    void SetOptions(AppTokenOptions *tokenOptions)
    {
        TokenOptions = tokenOptions;
    }

    std::vector<uint8_t> buildSignBody()
    {
        std::vector<uint8_t> buf;

        // AppId
        uint32_t appIdLen = htonl(AppId.length());
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&appIdLen), reinterpret_cast<uint8_t *>(&appIdLen) + sizeof(appIdLen));
        buf.insert(buf.end(), AppId.begin(), AppId.end());

        // IssueTimestamp
        uint32_t issueTimestamp = htonl(IssueTimestamp);
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&issueTimestamp), reinterpret_cast<uint8_t *>(&issueTimestamp) + sizeof(issueTimestamp));

        // Salt
        uint32_t salt = htonl(Salt);
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&salt), reinterpret_cast<uint8_t *>(&salt) + sizeof(salt));

        // Timestamp
        uint32_t timestamp = htonl(Timestamp);
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&timestamp), reinterpret_cast<uint8_t *>(&timestamp) + sizeof(timestamp));

        // Service
        std::vector<uint8_t> serviceData = TokenService->Pack();
        buf.insert(buf.end(), serviceData.begin(), serviceData.end());

        if (TokenOptions == nullptr)
        {
            SetOptions(new AppTokenOptions());
        }

        // Options
        std::vector<uint8_t> optionsData = TokenOptions->Pack();
        buf.insert(buf.end(), optionsData.begin(), optionsData.end());

        std::vector<uint8_t> result(Utils::NextMultiple(buf.size(), 256), 0);
        std::copy(buf.begin(), buf.end(), result.begin());

        return result;
    }

    std::string Build()
    {
        if (AppKey.empty())
        {
            throw std::runtime_error("illegal AppKey");
        }
        if (TokenService == nullptr)
        {
            throw std::runtime_error("illegal Service");
        }

        TokenService->Validate();

        std::vector<uint8_t> generatedSignature = Utils::GenerateSign(AppKey, IssueTimestamp, Salt);

        std::vector<uint8_t> buf = buildSignBody();

        if (buf.empty())
        {
            throw std::runtime_error("build sign body failed");
        }

        std::vector<uint8_t> sign = Utils::Sign(generatedSignature, buf);

        std::vector<uint8_t> tokenBuf;
        // signLength
        int32_t signLength = htonl(static_cast<int32_t>(sign.size()));
        tokenBuf.insert(tokenBuf.end(), reinterpret_cast<uint8_t *>(&signLength),
                        reinterpret_cast<uint8_t *>(&signLength) + sizeof(signLength));

        // signBody
        tokenBuf.insert(tokenBuf.end(), sign.begin(), sign.end());

        // buf
        tokenBuf.insert(tokenBuf.end(), buf.begin(), buf.end());

        std::vector<uint8_t> finalTokenBuf(Utils::NextMultiple(tokenBuf.size(), 256), 0);
        std::copy(tokenBuf.begin(), tokenBuf.end(), finalTokenBuf.begin());

        std::vector<uint8_t> tokenCompress = Utils::Compress(finalTokenBuf);
        return "000" + Utils::Base64Encode(tokenCompress);
    }

};

#endif