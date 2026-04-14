package com.ding.rtc.demo.token.model;


import com.ding.rtc.demo.token.constant.AppTokenConstants;
import com.ding.rtc.demo.token.constant.SecurityConstants;
import com.ding.rtc.demo.token.util.CompressUtils;
import com.ding.rtc.demo.token.util.EncodeUtils;
import com.ding.rtc.demo.token.util.SignatureUtils;

import org.apache.commons.lang3.StringUtils;

import java.security.SecureRandom;
import java.util.Objects;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import lombok.Getter;
import lombok.NonNull;

@Getter
public class AppToken {
    @NonNull
    private final String appId;
    private String appKey;
    private final int issueTimestamp;
    private final int salt;
    private final int timestamp;
    private Service service;
    private AppTokenOptions options;
    private byte[] signature;

    public AppToken(@NonNull String appId, @NonNull String appKey, int timestamp) {
        this.appId = appId;
        this.appKey = appKey;
        this.timestamp = timestamp;
        this.issueTimestamp = (int) (System.currentTimeMillis() / 1000);
        this.salt = new SecureRandom().nextInt();
    }

    public void addService(@NonNull Service service) {
        this.service = service;
    }

    public void addOptions(@NonNull AppTokenOptions options) {
        this.options = options;
    }

    public String buildTokenString() throws Throwable {
        if (StringUtils.isBlank(this.appKey)) {
            throw new IllegalArgumentException("missing secretKey");
        }

        if (Objects.isNull(this.service)) {
            throw new IllegalArgumentException("missing service");
        }

        final byte[] signatureTemp = SignatureUtils.sign(this.appKey, this.issueTimestamp, this.salt);

        final ByteBuf buf = Unpooled.buffer();
        final byte[] appId = this.appId.getBytes();
        buf.writeInt(appId.length);
        buf.writeBytes(appId);
        buf.writeInt(this.issueTimestamp);
        buf.writeInt(this.salt);
        buf.writeInt(this.timestamp);

        this.service.pack(buf);
        if (Objects.isNull(this.options)) {
            this.options = new AppTokenOptions();
        }
        this.options.pack(buf);

        final Mac mac = Mac.getInstance(SecurityConstants.ALGORITHM_HMAC_SHA256);
        mac.init(new SecretKeySpec(signatureTemp, SecurityConstants.ALGORITHM_HMAC_SHA256));
        final byte[] signature = mac.doFinal(buf.array());

        final ByteBuf bufToken = Unpooled.buffer();
        bufToken.writeInt(signature.length);
        bufToken.writeBytes(signature);
        bufToken.writeBytes(buf.array());

        return AppTokenConstants.VERSION_0 + EncodeUtils.base64Encode(CompressUtils.compress(bufToken.array()));
    }
}
