package com.ding.rtc.demo.rtm;

import com.ding.rtc.DingRtmClient;
import com.ding.rtc.DingRtmEventListener;

import java.util.ArrayList;
import java.util.List;

public class RtmClientHolder {
    private static RtmClientHolder instance;
    private DingRtmClient client;
    private final List<DingRtmEventListener> listeners;

    private RtmClientHolder() {
        listeners = new ArrayList<>();
    }

    public static RtmClientHolder getInstance() {
        if (instance == null) {
            instance = new RtmClientHolder();
        }
        return instance;
    }

    public void addListener(DingRtmEventListener listener) {
        listeners.add(listener);
    }

    public void init(DingRtmClient client) {
        this.client = client;

        client.setListener(new DingRtmEventListener() {
            @Override
            public void onRtmServerStateChanged(DingRtmClient.DingRtmServerState state, int errorCode) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onRtmServerStateChanged(state, errorCode);
                }
            }

            @Override
            public void onJoinSessionResult(String sessionId, int result) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onJoinSessionResult(sessionId, result);
                }
            }

            @Override
            public void onLeaveSessionResult(String sessionId, int reason) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onLeaveSessionResult(sessionId, reason);
                }
            }

            @Override
            public void onCloseSessionResult(String sessionId, int result) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onCloseSessionResult(sessionId, result);
                }
            }

            @Override
            public void onRemovedFromSession(String sessionId, int reason) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onCloseSessionResult(sessionId, reason);
                }
            }

            @Override
            public void onSessionCreate(String sessionId) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionCreate(sessionId);
                }
            }

            @Override
            public void onSessionClose(String sessionId) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionClose(sessionId);
                }
            }

            @Override
            public void onSessionRemoteUserJoin(String sessionId, String uid) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionRemoteUserJoin(sessionId, uid);
                }
            }

            @Override
            public void onSessionRemoteUserLeave(String sessionId, String uid) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionRemoteUserLeave(sessionId, uid);
                }
            }

            @Override
            public void onMessage(String sessionId, String fromUid, boolean broadcast, byte[] data) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onMessage(sessionId, fromUid, broadcast, data);
                }
            }

            @Override
            public void onSessionPropertySettingResult(String sessionId, String key, int result) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionPropertySettingResult(sessionId, key, result);
                }
            }

            @Override
            public void onSessionPropertyUpdate(String sessionId, DingRtmClient.SessionPropertyData data) {
                for (DingRtmEventListener listener : listeners) {
                    listener.onSessionPropertyUpdate(sessionId, data);
                }
            }
        });
    }

    public DingRtmClient getRtmClient() {
        return client;
    }

    public void clear() {
        client.setListener(null);
        client = null;
        listeners.clear();
    }
}
