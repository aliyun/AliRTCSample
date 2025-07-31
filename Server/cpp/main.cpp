#include <iostream>
#include <string>

#include "AppTokenOptions.h"
#include "Service.h"
#include "AppToken.h"

int main()
{
    // find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
    std::string appId = "replace_your_appId";
    // find appKey in your RTC console
    std::string appKey = "replace_your_appKey";
    // Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
    int32_t timestamp = static_cast<int32_t>(std::time(0)) + 12 * 60 * 60;

    std::string channelId = "replace_your_channelId";
    std::string userId = "replace_your_userId";

    AppToken appToken = AppToken(appId, appKey, timestamp);

    // By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
    // Please check (https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
    // Example0: full privileges as default
    Service service = Service::CreateService(channelId, userId);
    appToken.SetService(&service);
    std::cout << appToken.Build() << std::endl;

    // Example1: only allow audio
    // Service service = Service::CreateService(channelId, userId);
    // service.AddAudioPublishPrivilege();
    // appToken.SetService(&service);
    // std::cout << appToken.Build() << std::endl;

    // Example2: only allow audio and video
    // Service service = Service::CreateService(channelId, userId);
    // service.AddAudioPublishPrivilege();
    // service.AddVideoPublishPrivilege();
    // appToken.SetService(&service);
    // std::cout << appToken.Build() << std::endl;

    // By default, no more additional options are set. You can set more options individually as shown in the example below.
    // Please check (https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
    // std::map<std::string, std::string> tokenOptions;
    // tokenOptions["duration_per_channel"] = "86400";
    // tokenOptions["delay_close_per_channel"] = "60";
    // AppTokenOptions appTokenOptions;
    // appTokenOptions.SetEngineOptions(tokenOptions);
    // appToken.SetOptions(&appTokenOptions);
    // std::cout << appToken.Build() << std::endl;

    return 0;
}