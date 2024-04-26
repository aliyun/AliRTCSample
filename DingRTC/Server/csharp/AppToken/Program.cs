
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
            int timestamp = (int)(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000);

            Service service = new Service("channelId", "userId");
            service.AddAudioPublishPrivilege();

            AppTokenOptions options = new AppTokenOptions();

            AppToken.Model.AppToken appTokenBuilder = new AppToken.Model.AppToken(appId, appKey, timestamp);
            appTokenBuilder.SetOptions(options);
            appTokenBuilder.SetService(service);

            var token = appTokenBuilder.Build();

            Console.WriteLine(token);

            AppToken.Model.AppToken appToken = AppToken.Model.AppToken.Parse(token);

            Console.WriteLine(appToken.Validate(appKey));
        }
    }
}

