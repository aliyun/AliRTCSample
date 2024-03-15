package com.company.apptoken.util;

import java.io.ByteArrayOutputStream;
import java.util.zip.Deflater;
import java.util.zip.Inflater;

public class CompressUtils {
    public static byte[] compress(byte[] data) {
        byte[] output;
        final Deflater deflater = new Deflater();
        final ByteArrayOutputStream bos = new ByteArrayOutputStream(data.length);

        try {
            deflater.reset();
            deflater.setInput(data);
            deflater.finish();

            byte[] buf = new byte[data.length];
            while (!deflater.finished()) {
                int i = deflater.deflate(buf);
                bos.write(buf, 0, i);
            }
            output = bos.toByteArray();
        } catch (Throwable e) {
            output = data;
        } finally {
            deflater.end();
        }

        return output;
    }

    public static byte[] decompress(byte[] data) {
        final Inflater inflater = new Inflater();
        final ByteArrayOutputStream bos = new ByteArrayOutputStream(data.length);

        try {
            inflater.setInput(data);
            byte[] buf = new byte[data.length];
            while (!inflater.finished()) {
                int i = inflater.inflate(buf);
                bos.write(buf, 0, i);
            }
        } catch (Exception e) {
        } finally {
            inflater.end();
        }

        return bos.toByteArray();
    }
}
