package com.company.apptoken.model;

import io.netty.buffer.ByteBuf;
import lombok.Getter;
import lombok.NonNull;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

@Getter
public class AppTokenOptions {
    private Map<String, String> engineOptions;

    public void addEngineOptions(@NonNull Map<String, String> engineOptions) {
        this.engineOptions = engineOptions;
    }

    public void pack(@NonNull ByteBuf buf) {
        final boolean hasEngineOptions = Objects.nonNull(this.engineOptions);
        buf.writeBoolean(hasEngineOptions);
        if (!hasEngineOptions) {
            return;
        }
        buf.writeInt(this.engineOptions.size());
        for (Map.Entry<String, String> entry : this.engineOptions.entrySet()) {
            if (Objects.isNull(entry) || Objects.isNull(entry.getKey()) || Objects.isNull(entry.getValue())) {
                throw new IllegalArgumentException("illegal engineOptions entry");
            }
            buf.writeInt(entry.getKey().length());
            buf.writeBytes(entry.getKey().getBytes());
            buf.writeInt(entry.getValue().length());
            buf.writeBytes(entry.getValue().getBytes());
        }
    }

    public static AppTokenOptions unpack(@NonNull ByteBuf buf) {
        final boolean hasEngineOptions = buf.readBoolean();
        if (!hasEngineOptions) {
            return new AppTokenOptions();
        }

        final Map<String, String> engineOptions = new HashMap<>();
        final int size = buf.readInt();
        for (int i = 0; i < size; i++) {
            final byte[] key = new byte[buf.readInt()];
            final byte[] value = new byte[buf.readInt()];
            engineOptions.put(new String(key), new String(value));
        }
        return new AppTokenOptions(engineOptions);
    }

    private AppTokenOptions(Map<String, String> engineOptions) {
        this.engineOptions = engineOptions;
    }

    public AppTokenOptions() {
        this.engineOptions = new HashMap<>();
    }
}
