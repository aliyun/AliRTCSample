package com.ding.rtc.demo.rtm;

import android.text.TextUtils;

import java.util.ArrayList;
import java.util.List;

public class RtmSessionMgr {

    private static final String TAG = "[RtmSession]";

    private final List<RtmSessionWrapper> sessionList = new ArrayList<>();

    private static class Holder {
        private static final RtmSessionMgr INSTANCE = new RtmSessionMgr();
    }

    public static RtmSessionMgr getInstance() {
        return Holder.INSTANCE;
    }

    private RtmSessionMgr() {
    }

    public RtmSessionWrapper getSession(String sessionId) {
        if (TextUtils.isEmpty(sessionId)) {
            return null;
        }
        for (RtmSessionWrapper session : sessionList) {
            if (sessionId.equals(session.getSessionId())) {
                return session;
            }
        }
        return null;
    }

    public List<RtmSessionWrapper> getSessionList() {
        return sessionList;
    }

    public int getSessionCount() {
        return sessionList.size();
    }

    public void addSession(RtmSessionWrapper session) {
        boolean alreadyContained = false;
        for (RtmSessionWrapper sessionWrapper : sessionList) {
            if (session.getSessionId().equals(sessionWrapper.getSessionId())) {
                alreadyContained = true;
                break;
            }
        }
        if (!alreadyContained) {
            sessionList.add(session);
        }
    }

    public void onJoinSession(String sessionId) {
        boolean exist = false;
        for (RtmSessionWrapper session : sessionList) {
            if (session.getSessionId().equals(sessionId)) {
                exist = true;
                session.setJoined(true);
                break;
            }
        }
        if (!exist) {
            RtmSession session = new RtmSession();
            session.sessionId = sessionId;
            session.joined = true;
            sessionList.add(new RtmSessionWrapper(session));
        }
    }

    public void onLeaveSession(String sessionId) {
        for (RtmSessionWrapper session : sessionList) {
            if (session.getSessionId().equals(sessionId)) {
                session.setJoined(false);
                session.removeAllUsers();
                break;
            }
        }
    }

    public void removeSession(String sessionId) {
        for (RtmSessionWrapper session : sessionList) {
            if (session.getSessionId().equals(sessionId)) {
                sessionList.remove(session);
                break;
            }
        }
    }

    public void addSessionUser(String sessionId, RtmUserWrapper user) {
        RtmSessionWrapper session = getSession(sessionId);
        if (session != null) {
            session.addUser(user);
        }
    }

    public void removeSessionUser(String sessionId, String uid) {
        RtmSessionWrapper session = getSession(sessionId);
        if (session != null) {
            session.removeUser(uid);
        }
    }

    public void clearSession() {
        sessionList.clear();
    }

}
