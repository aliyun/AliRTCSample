namespace AppToken.Util
{
    public static class EncodeUtils
    {
        public static String Base64Encode(byte[] data)
        {
            return Convert.ToBase64String(data);
        }

        public static byte[] Base64Decode(string data){
            return Convert.FromBase64String(data);
        }
    }
}