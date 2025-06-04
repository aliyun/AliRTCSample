package com.ding.rtc.api.example.token_generator.util;

import com.ding.rtc.api.example.token_generator.constant.SecurityConstants;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import lombok.NonNull;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

public class SignatureUtils {
    public static byte[] sign(@NonNull String secretKey, int timestamp, int salt) throws Throwable {
        final Mac mac = Mac.getInstance(SecurityConstants.ALGORITHM_HMAC_SHA256);

        final ByteBuf bufTs = Unpooled.buffer(SecurityConstants.INT_LENGTH);
        bufTs.writeInt(timestamp);
        mac.init(new SecretKeySpec(bufTs.array(), SecurityConstants.ALGORITHM_HMAC_SHA256));

        byte[] signing = mac.doFinal(secretKey.getBytes());

        final ByteBuf bufSalt = Unpooled.buffer(SecurityConstants.INT_LENGTH);
        bufSalt.writeInt(salt);
        mac.init(new SecretKeySpec(bufSalt.array(), SecurityConstants.ALGORITHM_HMAC_SHA256));

        return mac.doFinal(signing);
    }
}
