const { ByteBuffer } = require('./util/bytes_utils');

WILDCARD_CHARACTERS = "*";
ENABLE_PRIVILEGE = 1;
ENABLE_AUDIO_PRIVILEGE = 2;
ENABLE_VIDEO_PRIVILEGE = 4;

class Service {


    constructor(channelId, userId, privilege = null) {
        this.channelId = channelId;
        this.userId = userId;
        this.privilege = privilege;
    }

    static createServiceOnlyWithUserId(userId) {
        return new this(WILDCARD_CHARACTERS, userId);
    }

    static createServiceOnlyWithChannelId(channelId) {
        return new this(channelId, WILDCARD_CHARACTERS);
    }

    addAudioPublishPrivilege() {
        this.privilege = this.privilege || 0 | ENABLE_PRIVILEGE;
        this.privilege |= this.ENABLE_AUDIO_PRIVILEGE;
    }

    addVideoPublishPrivilege() {
        this.privilege = this.privilege || 0 | ENABLE_PRIVILEGE;
        this.privilege |= ENABLE_VIDEO_PRIVILEGE;
    }

    pack() {
        let buf = new ByteBuffer(0)
        buf.writeString(this.channelId)
        buf.writeString(this.userId)

        let has_privilege = this.privilege !== null

        buf.writeBool(has_privilege)
        if (has_privilege) {
            buf.writeInt32(this.privilege)
        }

        return buf.getvalue(true)
    }

    static unpack(buffer) {
        let channel_id = buffer.readString();
        let user_id = buffer.readString();

        let has_privilege = buffer.readInt8() == 1
        let privilege = has_privilege ? buffer.readInt32() : null;

        return new Service(channel_id, user_id, privilege);
    }
}

module.exports.Service = Service
