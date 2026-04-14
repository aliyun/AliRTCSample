package com.ding.rtc.demo.utils;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MD5Util {

    /**
     * 计算字符串的 MD5 值
     *
     * @param input 需要加密的字符串
     * @return MD5 字符串（32位小写）
     */
    public static String md5(String input) {
        try {
            // 获取 MD5 算法的 MessageDigest 对象
            MessageDigest md = MessageDigest.getInstance("MD5");

            // 将输入字符串更新到摘要中
            md.update(input.getBytes());

            // 获得密文（byte数组）
            byte[] digest = md.digest();

            // 将 byte 数组转换为 16 进制字符串
            BigInteger bigInt = new BigInteger(1, digest);
            String hashText = bigInt.toString(16);

            // 补齐 32 位（有些哈希值可能以 0 开头，会被省略）
            while (hashText.length() < 32) {
                hashText = "0" + hashText;
            }

            return hashText;
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("MD5 algorithm not found", e);
        }
    }
}
