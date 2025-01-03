using Ionic.Zlib;

namespace AppToken.Util
{
    public static class CompressUtils
    {

        public static byte[] Compress(byte[] input)
        {
            using (var memoryStream = new MemoryStream())
            {
                using (var deflateStream = new ZlibStream(memoryStream, CompressionMode.Compress, CompressionLevel.Default, true))
                {
                    deflateStream.Write(input, 0, input.Length);
                }

                return memoryStream.ToArray();
            }

        }

        public static byte[] Decompress(byte[] input)
        {
            using (var inputStream = new MemoryStream(input))
            {
                using (var deflateStream = new ZlibStream(inputStream, CompressionMode.Decompress, true))
                {
                    using var outputStream = new MemoryStream();
                    deflateStream.CopyTo(outputStream);

                    return outputStream.ToArray();
                }

            }

        }
    }
}