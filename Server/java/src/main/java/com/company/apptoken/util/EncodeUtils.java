package com.company.apptoken.util;

import org.apache.commons.codec.binary.Base64;

public class EncodeUtils {
    public static String base64Encode(byte[] data) {
        final byte[] encodedBytes = Base64.encodeBase64(data);
        return new String(encodedBytes);
    }

    public static byte[] base64Decode(String data) {
        return Base64.decodeBase64(data.getBytes());
    }
}
