package com.ding.rtc.demo.rtm;

import static com.ding.rtc.demo.aiagent.AiAgent.AGENT_SESSION_ID;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.demo.R;

import java.util.ArrayList;
import java.util.List;

public class RtmExpandableAdapter extends BaseExpandableListAdapter {

    private final LayoutInflater inflater;
    private List<RtmSessionWrapper> sessionList = new ArrayList<>();
    private SessionListListener listener;
    private final String selfUid;

    public RtmExpandableAdapter(Context context, String selfUid) {
        inflater = LayoutInflater.from(context);
        this.selfUid = selfUid;
    }

    public void setSessionListListener(SessionListListener listener) {
        this.listener = listener;
    }

    public void setData(List<RtmSessionWrapper> sessionList) {
        this.sessionList = sessionList;
    }

    @Override
    public int getGroupCount() {
        return sessionList.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        if (groupPosition >= sessionList.size()) {
            return 0;
        }
        RtmSessionWrapper session = sessionList.get(groupPosition);
        return session.getRtmUsers().size();
    }

    @Override
    public RtmSessionWrapper getGroup(int groupPosition) {
        if (groupPosition < 0 || groupPosition > sessionList.size()) {
            return null;
        }
        return sessionList.get(groupPosition);
    }

    @Override
    public RtmUserWrapper getChild(int groupPosition, int childPosition) {
        RtmSessionWrapper session = getGroup(groupPosition);
        if (session == null) {
            return null;
        }
        if (childPosition < 0 || childPosition > session.getRtmUsers().size()) {
            return null;
        }
        return session.getRtmUsers().get(childPosition);
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        SessionHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.rtm_session_item_layout, parent, false);
            holder = new SessionHolder();
            holder.arrowImg = convertView.findViewById(R.id.arrow_img);
            holder.sessionDes = convertView.findViewById(R.id.session_des);
            holder.closeImg = convertView.findViewById(R.id.close);
            holder.joinImg = convertView.findViewById(R.id.join);
            holder.leaveImg = convertView.findViewById(R.id.leave);
            holder.chatImg = convertView.findViewById(R.id.chat);
            convertView.setTag(holder);
        } else {
            holder = (SessionHolder) convertView.getTag();
        }
        RtmSessionWrapper session = getGroup(groupPosition);
        String sessionId = session.getSessionId();
        holder.arrowImg.setImageResource(isExpanded ? R.drawable.arrow_up : R.drawable.arrow_down);
        holder.sessionDes.setText("SessionId: " + sessionId);
        if (session.isJoined()) {
            holder.joinImg.setVisibility(View.GONE);
            holder.leaveImg.setVisibility(View.VISIBLE);
            holder.closeImg.setVisibility(View.VISIBLE);
            holder.chatImg.setVisibility(View.VISIBLE);
        } else {
            holder.joinImg.setVisibility(View.VISIBLE);
            holder.leaveImg.setVisibility(View.GONE);
            holder.closeImg.setVisibility(View.GONE);
            holder.chatImg.setVisibility(View.GONE);
        }
        holder.joinImg.setOnClickListener(v -> {
            if (listener != null) {
                listener.onJoinSession(sessionId);
            }
        });
        holder.leaveImg.setOnClickListener(v -> {
            if (listener != null) {
                listener.onLeaveSession(sessionId);
            }
        });
        holder.closeImg.setOnClickListener(v -> {
            if (listener != null) {
                listener.onCloseSession(sessionId);
            }
        });
        holder.chatImg.setOnClickListener(v -> {
            if (listener != null) {
                listener.onClickChat(sessionId, "");
            }
        });

        if (AGENT_SESSION_ID.equals(sessionId)) {
            holder.joinImg.setVisibility(View.GONE);
            holder.leaveImg.setVisibility(View.GONE);
            holder.closeImg.setVisibility(View.GONE);
            holder.chatImg.setVisibility(View.GONE);
        }
        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        UserHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.rtm_user_item_layout, parent, false);
            holder = new UserHolder();
            holder.userDes = convertView.findViewById(R.id.user_des);
            convertView.setTag(holder);
        } else {
            holder = (UserHolder) convertView.getTag();
        }
        RtmUserWrapper user = getChild(groupPosition, childPosition);
        if (selfUid.equals(user.uid())) {
            holder.userDes.setText("Uid: " + user.uid() + "(自己)");
        } else {
            holder.userDes.setText("Uid: " + user.uid());
            convertView.setOnClickListener(v -> {
                if (listener != null) {
                    RtmSessionWrapper session = getGroup(groupPosition);
                    listener.onClickChat(session.getSessionId(), user.uid());
                }
            });
        }
        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return false;
    }


    public static class SessionHolder {
        ImageView arrowImg;
        TextView sessionDes;
        ImageView joinImg;
        ImageView leaveImg;
        ImageView closeImg;
        ImageView chatImg;
    }

    public static class UserHolder {
        TextView userDes;
    }

    public interface SessionListListener {
        void onJoinSession(String sessionId);
        void onLeaveSession(String sessionId);
        void onCloseSession(String sessionId);
        void onClickChat(String sessionId, String uid);
    }
}
