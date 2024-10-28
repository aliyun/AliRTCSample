using System.Security.Cryptography;
using System.Text;

namespace AppToken.Util
{
    public static class SignatureUtils
    {
        public static byte[] GenerateSign(string key, int timestamp, int salt)
        {
            byte[] timestampBytes = BigEndianUtils.GetBytesBigEndian(timestamp);
            using (HMACSHA256 macTs = new HMACSHA256(timestampBytes))
            {
                byte[] signing = macTs.ComputeHash(Encoding.UTF8.GetBytes(key));

                byte[] saltBytes = BigEndianUtils.GetBytesBigEndian(salt);
                using (HMACSHA256 macSalt = new HMACSHA256(saltBytes))
                {
                    return macSalt.ComputeHash(signing);
                }
            }
        }

        public static byte[] Sign(byte[] key, byte[] data)
        {
            using (HMACSHA256 mac = new HMACSHA256(key))
            {
                return mac.ComputeHash(data);
            }
        }
    }
}