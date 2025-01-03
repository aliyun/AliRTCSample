using System.Text;
using AppToken.Util;

namespace AppToken.Model
{
    public class Service
    {
        private readonly string channelId;
        private readonly string userId;
        private int? privilege;

        public Service(string channelId, string userId)
        {
            this.channelId = channelId ?? throw new ArgumentNullException(nameof(channelId));
            this.userId = userId ?? throw new ArgumentNullException(nameof(userId));
        }

        private Service(string channelId, string userId, int? privilege)
        : this(channelId, userId)
        {
            this.privilege = privilege;
        }

        public static Service CreateServiceOnlyWithUserId(string userId)
        {
            return new Service("*", userId);
        }

        public static Service CreateServiceOnlyWithChannelId(string channelId)
        {
            return new Service(channelId, "*");
        }

        public void AddAudioPublishPrivilege()
        {
            if (!this.privilege.HasValue)
            {
                this.privilege = 0;
                this.privilege |= PrivilegeEnum.ENABLE_PRIVILEGE;
            }
            this.privilege |= PrivilegeEnum.ENABLE_AUDIO_PUBLISH;
        }

        public void AddVideoPublishPrivilege()
        {
            if (!this.privilege.HasValue)
            {
                this.privilege = 0;
                this.privilege |= PrivilegeEnum.ENABLE_PRIVILEGE;
            }
            this.privilege |= PrivilegeEnum.ENABLE_VIDEO_PUBLISH;
        }

        public void AddScreenPublishPrivilege()
        {
            if (!this.privilege.HasValue)
            {
                this.privilege = 0;
                this.privilege |= PrivilegeEnum.ENABLE_PRIVILEGE;
            }
            this.privilege |= PrivilegeEnum.ENABLE_SCREEN_PUBLISH;
        }

        public void Pack(MemoryStream buf)
        {
            using (var writer = new BinaryWriter(buf, Encoding.UTF8, true))
            {
                var channelIdBytes = Encoding.UTF8.GetBytes(this.channelId);
                writer.Write(BigEndianUtils.GetBytesBigEndian(channelIdBytes.Length));
                writer.Write(channelIdBytes);

                var userIdBytes = Encoding.UTF8.GetBytes(this.userId);
                writer.Write(BigEndianUtils.GetBytesBigEndian(userIdBytes.Length));
                writer.Write(userIdBytes);

                writer.Write(this.privilege.HasValue);

                if (this.privilege.HasValue)
                {
                    writer.Write(BigEndianUtils.GetBytesBigEndian(this.privilege.Value));
                }
            }
        }

        public static Service Unpack(MemoryStream buf)
        {
            using (var reader = new BinaryReader(buf, Encoding.UTF8, true))
            {
                var channelLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                var byteChannelId = reader.ReadBytes(channelLength);
                var channelId = Encoding.UTF8.GetString(byteChannelId);

                var userIdLength = BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4));
                var byteUserId = reader.ReadBytes(userIdLength);
                var userId = Encoding.UTF8.GetString(byteUserId);

                var hasPrivilege = reader.ReadBoolean();
                int? privilege = hasPrivilege ? BigEndianUtils.FromBytesBigEndianToInt(reader.ReadBytes(4)) : (int?)null;

                return new Service(channelId, userId, privilege);
            }
        }
    }

    public static class PrivilegeEnum
    {
        public const int ENABLE_PRIVILEGE = 1;
        public const int ENABLE_AUDIO_PUBLISH = 2;
        public const int ENABLE_VIDEO_PUBLISH = 4;
        public const int ENABLE_SCREEN_PUBLISH = 8;
    }
}