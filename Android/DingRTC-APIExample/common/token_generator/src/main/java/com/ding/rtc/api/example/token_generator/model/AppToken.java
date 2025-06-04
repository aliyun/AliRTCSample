package com.ding.rtc.api.example.token_generator.model;

import com.ding.rtc.api.example.token_generator.constant.AppTokenConstants;
import com.ding.rtc.api.example.token_generator.constant.SecurityConstants;
import com.ding.rtc.api.example.token_generator.util.CompressUtils;
import com.ding.rtc.api.example.token_generator.util.EncodeUtils;
import com.ding.rtc.api.example.token_generator.util.SignatureUtils;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import lombok.Getter;
import lombok.NonNull;
import org.apache.commons.lang3.StringUtils;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.security.SecureRandom;
import java.util.Map;
import java.util.Objects;
import java.util.TreeMap;

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

    public static void main(String[] args) throws Throwable {
        // find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
        String appId = "replace_your_appId";
        // find appKey in your RTC console
        String appKey = "replace_your_appKey";
        // Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
        int expiredTs = (int) (System.currentTimeMillis() / 1000) + 12 * 60 * 60;
        String channelId = "replace_your_channelId";
        String userId = "replace_your_userId";

        final AppToken appToken = new AppToken(appId, appKey, expiredTs);

        // By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
        // Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
        // Example0: full privileges as default
        final Service service = new Service(channelId, userId);
        appToken.addService(service);
        final String appTokenStr = appToken.buildTokenString();
        System.out.println(appTokenStr);

//         Example1: only allow audio
//        final Service service = new Service(channelId, userId);
//        service.addAudioPublishPrivilege();
//        appToken.addService(service);
//        final String appTokenStr = appToken.buildTokenString();
//        System.out.println(appTokenStr);

//         Example2: only allow audio and video
//        final Service service = new Service(channelId, userId);
//        service.addAudioPublishPrivilege();
//        service.addVideoPublishPrivilege();
//        appToken.addService(service);
//        final String appTokenStr = appToken.buildTokenString();
//        System.out.println(appTokenStr);

        // By default, no more additional options are set. You can set more options individually as shown in the example below.
        // Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
        //Example1: add more options
//        final Service service = new Service(channelId, userId);
//        appToken.addService(service);
//        final AppTokenOptions appTokenOptions = new AppTokenOptions();
//        Map<String, String> engineOptions = new TreeMap<>();
//        // set duration per channel to 86400s as example, please set by yourself
//        engineOptions.put("duration_per_channel","86400");
//        // set delay close per channel to 60s  as example, please set by yourself
//        engineOptions.put("delay_close_per_channel","60");
//        appTokenOptions.addEngineOptions(engineOptions);
//        appToken.addOptions(appTokenOptions);
//        final String appTokenStr = appToken.buildTokenString();
//        System.out.println(appTokenStr);
    }
}
