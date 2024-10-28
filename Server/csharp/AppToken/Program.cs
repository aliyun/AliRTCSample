
using AppToken.Model;
using AppToken.Util;

namespace Program
{
    public class Program
    {
        public static void Main()
        {
            string appId = "appId";
            string appKey = "appKey";
            int expiredTs = (int)(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000) + 12 * 60 * 60; // expired after 12h

            Service service = new Service("channelId", "userId");
            service.AddAudioPublishPrivilege();

            AppTokenOptions options = new AppTokenOptions();

            AppToken.Model.AppToken appTokenBuilder = new AppToken.Model.AppToken(appId, appKey, expiredTs);
            appTokenBuilder.SetOptions(options);
            appTokenBuilder.SetService(service);

            var token = appTokenBuilder.Build();

            Console.WriteLine(token);

            AppToken.Model.AppToken appToken = AppToken.Model.AppToken.Parse(token);

            Console.WriteLine(appToken.Validate(appKey));
        }
    }
}

