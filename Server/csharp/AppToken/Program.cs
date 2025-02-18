
using AppToken.Model;
using AppToken.Util;

namespace Program
{
    public class Program
    {
        public static void Main()
        {
            // find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
            string appId = "replace_your_appId";
            // find appKey in your RTC console
            string appKey = "replace_your_appKey";
            // Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
            int expiredTs = (int)(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000) + 12 * 60 * 60;
            string channelId = "replace_your_channelId";
            string userId = "replace_your_userId";

            AppToken.Model.AppToken appTokenBuilder = new AppToken.Model.AppToken(appId, appKey, expiredTs);
           
            // By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
            // Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
            // Example0: full privileges as default
            Service service = new Service(channelId, userId);
            appTokenBuilder.SetService(service);
            var token = appTokenBuilder.Build();
            Console.WriteLine(token);

            // Example1: only allow audio
            // Service service = new Service(channelId, userId);
            // service.AddAudioPublishPrivilege();
            // appTokenBuilder.SetService(service);
            // var token = appTokenBuilder.Build();
            // Console.WriteLine(token);

            // Example2: only allow audio and video
            // Service service = new Service(channelId, userId);
            // service.AddAudioPublishPrivilege();
            // service.AddVideoPublishPrivilege();
            // appTokenBuilder.SetService(service);
            // var token = appTokenBuilder.Build();
            // Console.WriteLine(token);

            // By default, no more additional options are set. You can set more options individually as shown in the example below.
            // Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
            //Example1: add more options
            // Service service = new Service(channelId, userId);
            // appTokenBuilder.SetService(service);
            
            // AppTokenOptions appTokenOptions = new AppTokenOptions();
            // Dictionary<string, string> engineOptions = new Dictionary<string, string>();
            // engineOptions.Add("duration_per_channel", "86400");
            // engineOptions.Add("delay_close_per_channel", "60");
            // appTokenOptions.SetEngineOptions(engineOptions);
            // appTokenBuilder.SetOptions(appTokenOptions);

            // var token = appTokenBuilder.Build();
            // Console.WriteLine(token);
        }
    }
}

