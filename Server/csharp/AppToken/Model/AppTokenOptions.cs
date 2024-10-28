using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using AppToken.Util;

namespace AppToken.Model
{
    public class AppTokenOptions
    {
        private Dictionary<string, string> engineOptions;

        public AppTokenOptions()
        {
            engineOptions = new Dictionary<string, string>();
        }

        private AppTokenOptions(Dictionary<string, string> engineOptions)
        {
            this.engineOptions = engineOptions ?? throw new ArgumentNullException(nameof(engineOptions));
        }

        public void SetEngineOptions(Dictionary<string, string> engineOptions)
        {
            this.engineOptions = engineOptions ?? throw new ArgumentNullException(nameof(engineOptions));
        }

        public void Pack(MemoryStream buf)
        {
            using (var writer = new BinaryWriter(buf, Encoding.UTF8, true))
            {
                writer.Write(engineOptions != null); // Write bool for existence
                if (engineOptions == null)
                {
                    return;
                }

                writer.Write(BigEndianUtils.GetBytesBigEndian(engineOptions.Count));
                var sortedKeys = engineOptions.Keys.OrderBy(k => k).ToList();
                foreach (var key in sortedKeys)
                {
                    var value = engineOptions[key];
                    if (key == null || value == null)
                        throw new ArgumentException("Illegal engineOptions entry");

                    writer.Write(BigEndianUtils.GetBytesBigEndian(key.Length));
                    writer.Write(Encoding.UTF8.GetBytes(key));

                    writer.Write(BigEndianUtils.GetBytesBigEndian(value.Length));
                    writer.Write(Encoding.UTF8.GetBytes(value));
                }
            }
        }

        public static AppTokenOptions Unpack(MemoryStream buf)
        {
            var engineOptions = new Dictionary<string, string>();

            using (var reader = new BinaryReader(buf, Encoding.UTF8, true))
            {
                if (!reader.ReadBoolean())
                    return new AppTokenOptions();

                int size = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                for (int i = 0; i < size; i++)
                {
                    int keyLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                    string key = Encoding.UTF8.GetString(reader.ReadBytes(keyLength));

                    int valueLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                    string value = Encoding.UTF8.GetString(reader.ReadBytes(valueLength));

                    engineOptions.Add(key, value);
                }
            }

            return new AppTokenOptions(engineOptions);
        }


    }
}