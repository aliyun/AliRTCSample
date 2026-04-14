package com.ding.rtc.demo.rtm;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.ding.rtc.demo.R;
import com.ding.rtc.demo.utils.LimitedSizeLinkedList;

import java.util.List;

public class RtmChatAdapter extends BaseAdapter {
    private final String selfUid;
    private final LayoutInflater inflater;
    private List<RtmMessage> msgList = new LimitedSizeLinkedList<>(700);

    public RtmChatAdapter(Context context, String selfUid) {
        this.selfUid = selfUid;
        inflater = LayoutInflater.from(context);
    }

    public void setMsgList(List<RtmMessage> msgList) {
        this.msgList = msgList;
    }

    @Override
    public int getCount() {
        return msgList.size();
    }

    @Override
    public RtmMessage getItem(int position) {
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
            convertView = inflater.inflate(R.layout.rtm_chat_item_layout, parent, false);
            holder = new MsgHolder();
            holder.leftAvatarTv = convertView.findViewById(R.id.left_avatar);
            holder.leftMsgTv = convertView.findViewById(R.id.left_text);
            holder.rightAvatarTv = convertView.findViewById(R.id.right_avatar);
            holder.rightMsgTv = convertView.findViewById(R.id.right_text);
            convertView.setTag(holder);
        } else {
            holder = (MsgHolder) convertView.getTag();
        }
        RtmMessage msg = getItem(position);
        String msgUid = msg.getUid();
        if (selfUid.equals(msgUid)) {
            holder.leftAvatarTv.setVisibility(View.GONE);
            holder.leftMsgTv.setVisibility(View.GONE);

            holder.rightAvatarTv.setVisibility(View.VISIBLE);
            holder.rightAvatarTv.setText(msgUid);
            holder.rightMsgTv.setVisibility(View.VISIBLE);
            holder.rightMsgTv.setText(msg.getStringData());
        } else {
            holder.leftAvatarTv.setVisibility(View.VISIBLE);
            holder.leftAvatarTv.setText(msgUid);
            holder.leftMsgTv.setVisibility(View.VISIBLE);
            holder.leftMsgTv.setText(msg.getStringData());

            holder.rightAvatarTv.setVisibility(View.GONE);
            holder.rightMsgTv.setVisibility(View.GONE);
        }
        return convertView;
    }

    private static class MsgHolder {
        TextView leftAvatarTv;
        TextView leftMsgTv;
        TextView rightAvatarTv;
        TextView rightMsgTv;
    }

}
