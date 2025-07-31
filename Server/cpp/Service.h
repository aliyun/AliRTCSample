#ifndef SERVICE_H
#define SERVICE_H
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

class Service
{
public:
    static const int32_t PRIVILEGE_ENABLED = 1;
    static const int32_t PRIVILEGE_AUDIO_PUBLISH = 2;
    static const int32_t PRIVILEGE_VIDEO_PUBLISH = 4;
    static const int32_t PRIVILEGE_SCREEN_PUBLISH = 8;

    std::string ChannelId;
    std::string UserId;
    int32_t *Privilege;

    Service() : Privilege(nullptr) {}
    Service(const std::string &channelId, const std::string &userId)
        : ChannelId(channelId), UserId(userId), Privilege(nullptr)
    {
    }
    ~Service()
    {
        delete Privilege;
    }

    static Service CreateService(const std::string &channelId, const std::string &userId)
    {
        return Service(channelId, userId);
    }

    void Validate()
    {
        if (ChannelId.empty() || UserId.empty())
        {
            throw std::invalid_argument("illegal ChannelId or UserId");
        }
    }
    void AddAudioPublishPrivilege()
    {
        if (Privilege == nullptr)
        {
            Privilege = new int32_t(PRIVILEGE_ENABLED);
        }
        *Privilege |= PRIVILEGE_AUDIO_PUBLISH;
    }
    void AddVideoPublishPrivilege()
    {
        if (Privilege == nullptr)
        {
            Privilege = new int32_t(PRIVILEGE_ENABLED);
        }
        *Privilege |= PRIVILEGE_VIDEO_PUBLISH;
    }
    void AddScreenPublishPrivilege()
    {
        if (Privilege == nullptr)
        {
            Privilege = new int32_t(PRIVILEGE_ENABLED);
        }
        *Privilege |= PRIVILEGE_SCREEN_PUBLISH;
    }
    std::vector<uint8_t> Pack()
    {
        std::vector<uint8_t> buf;

        int32_t channelIdLength = htonl(static_cast<int32_t>(ChannelId.size()));
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&channelIdLength),
                   reinterpret_cast<uint8_t *>(&channelIdLength) + sizeof(channelIdLength));
        buf.insert(buf.end(), ChannelId.begin(), ChannelId.end());

        int32_t userIdLength = htonl(static_cast<int32_t>(UserId.size()));
        buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&userIdLength),
                   reinterpret_cast<uint8_t *>(&userIdLength) + sizeof(userIdLength));
        buf.insert(buf.end(), UserId.begin(), UserId.end());

        bool hasPrivilege = (Privilege != nullptr);
        buf.push_back(static_cast<uint8_t>(hasPrivilege));
        if (hasPrivilege)
        {
            int32_t privilegeValue = htonl(*Privilege);
            buf.insert(buf.end(), reinterpret_cast<uint8_t *>(&privilegeValue),
                       reinterpret_cast<uint8_t *>(&privilegeValue) + sizeof(privilegeValue));
        }

        return buf;
    }
};

#endif