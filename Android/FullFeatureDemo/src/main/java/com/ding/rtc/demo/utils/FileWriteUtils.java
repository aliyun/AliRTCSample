package com.ding.rtc.demo.utils;

import android.content.Context;
import android.util.Log;

import com.ding.rtc.DingRtcEngine;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;

public class FileWriteUtils {
    private final boolean debugFileStream;
    private FileOutputStream outputStream = null;

    public static void copyResToInternalStorage(Context context, int drawableResourceId, String fileName) {
        try {
            File dir = context.getExternalFilesDir("");
            if (dir != null && !dir.exists()) {
                dir.mkdirs();
            }
            File file = new File(dir, fileName);
            if (!file.exists()) {
                // 确保父目录存在
                if (file.getParentFile() != null && !file.getParentFile().exists()) {
                    file.getParentFile().mkdirs();
                }
                // 获取输入流
                InputStream in = context.getResources().openRawResource(drawableResourceId);
                // 定义输出路径和文件名
                OutputStream out = new FileOutputStream(file);

                byte[] buffer = new byte[1024];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }
                in.close();
                out.flush();
                out.close();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void checkAndCopyFileToSD(String fileName, Context context) {
        // copy segment model to android phone
        File file = new File(context.getFilesDir().getAbsolutePath(), fileName);
        String filePath = file.getPath();
        if (!file.exists() || !file.isFile()) {
            copyBigDataToSD(fileName, filePath, context);
        }
    }

    private static void copyBigDataToSD(String fileName, String strOutFileName, Context context) {
        try {
            InputStream myInput;
            OutputStream myOutput = new FileOutputStream(strOutFileName);
            myInput = context.getAssets().open(fileName);
            byte[] buffer = new byte[1024];
            int length = myInput.read(buffer);
            while (length > 0) {
                myOutput.write(buffer, 0, length);
                length = myInput.read(buffer);
            }

            myOutput.flush();
            myInput.close();
            myOutput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private static final Map<String, FileOutputStream> userFileStreams = new HashMap<>();

    public static void dumpRemoteUserAudio(Context context, String uid, DingRtcEngine.DingRtcAudioFrame frame) {
        if (context == null || uid == null || frame == null || frame.data == null) {
            return;
        }
        File file = new File(context.getExternalFilesDir("audio_dump") + "/"
                + uid + "_" + frame.samplesPerSec + "_" + frame.numChannels + ".pcm");
        if (!file.exists()) {
            try {
                file.createNewFile();
            } catch (Exception e) {

            }
        }
        FileOutputStream stream = userFileStreams.get(uid);
        if (stream == null) {
            try {
                stream = new FileOutputStream(file);
                userFileStreams.put(uid, stream);
            } catch (Exception e) {
            }
        }
        int totalBytes = frame.data.capacity();
        if (stream == null) {
            return;
        }

        try {
            byte[] buffer = new byte[totalBytes];
            frame.data.get(buffer);
            stream.write(buffer, 0, totalBytes);
            stream.flush();
            //stream.close();
        } catch (Exception e) {
            Log.e("FileWriteUtils", "dumpRemoteUserAudio err : " + e.getMessage());
        }
    }

    public static void destroy() {
        for (FileOutputStream stream : userFileStreams.values()) {
            if (stream != null) {
                try {
                    stream.close();
                } catch (Exception e) {

                }
            }
        }
        userFileStreams.clear();
    }

    public FileWriteUtils(boolean debug) {
        debugFileStream = debug;
    }

    public void open(String filePath) {
        if (debugFileStream) {
            try {
                outputStream = new FileOutputStream(filePath);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
        }
    }

    public void write(byte[] data) {
        if (debugFileStream) {
            if (outputStream != null) {
                try {
                    outputStream.write(data);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void write(ByteBuffer data) {
        byte[] tmp = new byte[data.remaining()];
        data.get(tmp, 0, tmp.length);
        data.rewind();
        if (debugFileStream) {
            if (outputStream != null) {
                try {
                    outputStream.write(tmp);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void close() {
        if (debugFileStream) {
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                outputStream = null;
            }
        }
    }
}
