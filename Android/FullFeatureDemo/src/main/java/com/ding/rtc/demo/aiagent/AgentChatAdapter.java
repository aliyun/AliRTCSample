package com.ding.rtc.demo.aiagent;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.demo.R;
import com.ding.rtc.demo.utils.LimitedSizeLinkedList;

import java.util.List;

public class AgentChatAdapter extends BaseAdapter {
    private final LayoutInflater inflater;
    private final List<AgentMsg> msgList = new LimitedSizeLinkedList<>(500);

    public AgentChatAdapter(Context context) {
        inflater = LayoutInflater.from(context);
    }

    public void addMsg(AgentMsg msg) {
        if (!msgList.isEmpty()) {
            AgentMsg lastMsg = msgList.get(msgList.size() - 1);
            if (lastMsg.isAgent()) {
                if (msg.isAgent() && !lastMsg.isEnd()) {
                    msgList.remove(lastMsg);
                }
            } else {
                if (!msg.isAgent() && !lastMsg.isEnd()) {
                    msgList.remove(lastMsg);
                }
            }
        }
        msgList.add(msg);
        notifyDataSetChanged();
    }

    public void clearAllMsg() {
        msgList.clear();
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return msgList.size();
    }

    @Override
    public AgentMsg getItem(int position) {
        return msgList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        MsgHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.agent_chat_item, parent, false);
            holder = new MsgHolder();
            holder.agentAvatar = convertView.findViewById(R.id.agent_avatar);
            holder.agentMsgTv = convertView.findViewById(R.id.agent_text);
            holder.uerAvatar = convertView.findViewById(R.id.user_avatar);
            holder.uerMsgTv = convertView.findViewById(R.id.user_text);
            convertView.setTag(holder);
        } else {
            holder = (MsgHolder) convertView.getTag();
        }
        AgentMsg msg = getItem(position);

        if (msg.isAgent()) {
            holder.agentAvatar.setVisibility(View.VISIBLE);
            holder.agentMsgTv.setVisibility(View.VISIBLE);
            holder.uerAvatar.setVisibility(View.GONE);
            holder.uerMsgTv.setVisibility(View.GONE);

            holder.agentMsgTv.setText(msg.getMsg());
        } else {
            holder.agentAvatar.setVisibility(View.GONE);
            holder.agentMsgTv.setVisibility(View.GONE);
            holder.uerAvatar.setVisibility(View.VISIBLE);
            holder.uerMsgTv.setVisibility(View.VISIBLE);

            holder.uerMsgTv.setText(msg.getMsg());
        }
        return convertView;
    }

    private static class MsgHolder {
        ImageView agentAvatar;
        TextView agentMsgTv;
        ImageView uerAvatar;
        TextView uerMsgTv;
    }

}
