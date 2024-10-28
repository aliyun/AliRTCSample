namespace AppToken.Util
{
    public static class BytesUtils
    {
        public static int NextMultiple(long n, int baseValue)
        {
            if (baseValue <= 0 || n <= 0)
            {
                return 0;
            }

            int result = baseValue;
            while (result < n)
            {
                result *= 2;
            }

            return result;
        }

        public static byte[] GetFixedLengthByteArray(MemoryStream buf)
        {
            return GetFixedLengthByteArray(buf, NextMultiple(buf.Length, 256));
        }

        public static byte[] GetFixedLengthByteArray(MemoryStream buf, int fixedLength)
        {
            if (fixedLength < 0)
            {
                throw new ArgumentException("Fixed length must be a non-negative number.");
            }

            byte[] sourceArray = buf.ToArray();
            byte[] fixedLengthArray = new byte[fixedLength];
            Array.Copy(sourceArray, 0, fixedLengthArray, 0, Math.Min(sourceArray.Length, fixedLength));
            return fixedLengthArray;
        }
    }
}