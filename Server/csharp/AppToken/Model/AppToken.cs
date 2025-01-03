using System.Text;
using AppToken.Util;

namespace AppToken.Model
{
    public class AppToken
    {
        private readonly string appId;
        private readonly string appKey;
        private int issueTimestamp;
        private int salt;
        private int timestamp;
        private Service service;
        private AppTokenOptions options;
        private byte[] signature;

        private const string VERSION_0 = "000";
        private const int VERSION_LENGTH = 3;

        public AppToken(string appId, string appKey, int timestamp)
        {
            this.appId = appId ?? throw new ArgumentNullException("missing appId");
            this.appKey = appKey ?? throw new ArgumentNullException("missing appKey");
            this.timestamp = timestamp;
            this.issueTimestamp = (int)(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000);
            this.salt = new Random().Next();
        }

        private AppToken(string appId, int issueTimestamp, int salt, int timestamp, Service service, AppTokenOptions options, byte[] signature)
        {
            this.appId = appId;
            this.timestamp = timestamp;
            this.issueTimestamp = issueTimestamp;
            this.salt = salt;
            this.service = service;
            this.options = options;
            this.signature = signature;
        }

        public void SetService(Service service)
        {
            this.service = service ?? throw new ArgumentNullException("missing service");
        }

        public void SetOptions(AppTokenOptions options)
        {
            this.options = options;
        }

        public string Build()
        {

            using var buf = new MemoryStream();
            using (var writer = new BinaryWriter(buf, Encoding.UTF8, true))
            {
                var appIdBytes = Encoding.UTF8.GetBytes(this.appId);
                writer.Write(BigEndianUtils.GetBytesBigEndian(appIdBytes.Length));
                writer.Write(appIdBytes);
                writer.Write(BigEndianUtils.GetBytesBigEndian(this.issueTimestamp));
                writer.Write(BigEndianUtils.GetBytesBigEndian(this.salt));
                writer.Write(BigEndianUtils.GetBytesBigEndian(this.timestamp));

                this.service.Pack(buf);

                this.options = this.options ?? new AppTokenOptions();
                this.options.Pack(buf);
            }

            using var tokenBuf = new MemoryStream();
            using (var writer = new BinaryWriter(tokenBuf, Encoding.UTF8, true))
            {

                byte[] signKey = SignatureUtils.GenerateSign(this.appKey, this.issueTimestamp, this.salt);
                byte[] tokenBody = BytesUtils.GetFixedLengthByteArray(buf);
                byte[] signature = SignatureUtils.Sign(signKey, tokenBody);

                writer.Write(BigEndianUtils.GetBytesBigEndian(signature.Length));
                writer.Write(signature);
                writer.Write(tokenBody);
            }

            return VERSION_0 + EncodeUtils.Base64Encode(CompressUtils.Compress(BytesUtils.GetFixedLengthByteArray(tokenBuf)));
        }

        public static AppToken Parse(string token)
        {
            token = token ?? throw new ArgumentNullException("empty appToken");
            if (!token.StartsWith(VERSION_0))
            {
                throw new ArgumentNullException("unsupported version");
            }

            byte[] data = CompressUtils.Decompress(EncodeUtils.Base64Decode(token.Substring(VERSION_LENGTH)));
            using var buf = new MemoryStream(data);
            using (var reader = new BinaryReader(buf, Encoding.UTF8, true))
            {
                int signLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                byte[] byteSignature = reader.ReadBytes(signLength);

                int appIdLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                string appId = Encoding.UTF8.GetString(reader.ReadBytes(appIdLength));

                int issueTimestamp = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                int salt = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                int timestamp = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));

                Service service = Service.Unpack(buf);
                AppTokenOptions options = AppTokenOptions.Unpack(buf);

                return new AppToken(appId, issueTimestamp, salt, timestamp, service, options, byteSignature);
            }
        }
    }
}