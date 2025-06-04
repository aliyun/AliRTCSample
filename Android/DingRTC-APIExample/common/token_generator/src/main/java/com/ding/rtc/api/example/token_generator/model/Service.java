package com.ding.rtc.api.example.token_generator.model;

import com.ding.rtc.api.example.token_generator.constant.AppTokenConstants;
import com.ding.rtc.api.example.token_generator.enums.PrivilegeEnum;
import io.netty.buffer.ByteBuf;
import lombok.Getter;
import lombok.NonNull;

import java.util.Objects;

@Getter
public class Service {
    @NonNull
    private final String channelId;
    @NonNull
    private final String userId;
    private Integer privilege;

    public Service(@NonNull String channelId, @NonNull String userId) {
        this.channelId = channelId;
        this.userId = userId;
    }

    public static Service createServiceOnlyWithUserId(@NonNull String userId) {
        return new Service(AppTokenConstants.WILDCARD_CHARACTERS, userId);
    }

    public static Service createServiceOnlyWithChannelId(@NonNull String channelId) {
        return new Service(channelId, AppTokenConstants.WILDCARD_CHARACTERS);
    }

    public void addAudioPublishPrivilege() {
        if (Objects.isNull(this.privilege)) {
            this.privilege = 0;
            this.privilege = this.privilege | PrivilegeEnum.ENABLE_PRIVILEGE.getPrivilege();
        }
        this.privilege = this.privilege | PrivilegeEnum.ENABLE_AUDIO_PUBLISH.getPrivilege();
    }

    public void addVideoPublishPrivilege() {
        if (Objects.isNull(this.privilege)) {
            this.privilege = 0;
            this.privilege = this.privilege | PrivilegeEnum.ENABLE_PRIVILEGE.getPrivilege();
        }
        this.privilege = this.privilege | PrivilegeEnum.ENABLE_VIDEO_PUBLISH.getPrivilege();
    }

    public void addScreenPublishPrivilege() {
        if (Objects.isNull(this.privilege)) {
            this.privilege = 0;
            this.privilege = this.privilege | PrivilegeEnum.ENABLE_PRIVILEGE.getPrivilege();
        }
        this.privilege = this.privilege | PrivilegeEnum.ENABLE_SCREEN_PUBLISH.getPrivilege();
    }


    public void pack(@NonNull ByteBuf buf) {
        final byte[] channelId = this.channelId.getBytes();
        buf.writeInt(channelId.length);
        buf.writeBytes(channelId);
        final byte[] userId = this.userId.getBytes();
        buf.writeInt(userId.length);
        buf.writeBytes(userId);
        final boolean hasPrivilege = Objects.nonNull(this.privilege);
        buf.writeBoolean(hasPrivilege);
        if (hasPrivilege) {
            buf.writeInt(this.privilege);
        }
    }

    public static Service unpack(@NonNull ByteBuf buf) {
        final byte[] byteChannelId = new byte[buf.readInt()];
        for (int i = 0; i < byteChannelId.length; i++) {
            byteChannelId[i] = buf.readByte();
        }
        final String channelId = new String(byteChannelId);

        final byte[] byteUserId = new byte[buf.readInt()];
        for (int i = 0; i < byteUserId.length; i++) {
            byteUserId[i] = buf.readByte();
        }
        final String userId = new String(byteUserId);

        final boolean hasPrivilege = buf.readBoolean();
        Integer privilege = null;
        if (hasPrivilege) {
            privilege = buf.readInt();
        }
        return new Service(channelId, userId, privilege);
    }

    private Service(@NonNull String channelId, @NonNull String userId, Integer privilege) {
        this.channelId = channelId;
        this.userId = userId;
        this.privilege = privilege;
    }
}
