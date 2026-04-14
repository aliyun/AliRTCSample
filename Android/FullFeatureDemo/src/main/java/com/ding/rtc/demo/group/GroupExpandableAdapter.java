package com.ding.rtc.demo.group;

import static com.ding.rtc.DingRtcEngine.HALL_ID;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.DingRtcEngine.DingRtcAudioGroupMember;
import com.ding.rtc.demo.R;

import java.util.ArrayList;
import java.util.List;

public class GroupExpandableAdapter extends BaseExpandableListAdapter {

    private final LayoutInflater inflater;
    private List<RtcGroup> groupList = new ArrayList<>();
    private GroupListListener listener;

    public GroupExpandableAdapter(Context context) {
        inflater = LayoutInflater.from(context);
    }

    public void setGroupListListener(GroupListListener listener) {
        this.listener = listener;
    }

    public void setData(List<RtcGroup> groupList) {
        this.groupList = groupList;
    }

    @Override
    public int getGroupCount() {
        return groupList.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        if (groupPosition >= groupList.size()) {
            return 0;
        }
        RtcGroup groupData = groupList.get(groupPosition);
        return groupData.memberList.size();
    }

    @Override
    public RtcGroup getGroup(int groupPosition) {
        if (groupPosition < 0 || groupPosition > groupList.size()) {
            return null;
        }
        return groupList.get(groupPosition);
    }

    @Override
    public DingRtcAudioGroupMember getChild(int groupPosition, int childPosition) {
        RtcGroup group = getGroup(groupPosition);
        if (group == null) {
            return null;
        }
        if (childPosition < 0 || childPosition > group.memberList.size()) {
            return null;
        }
        return group.memberList.get(childPosition);
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
        GroupHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.group_item_layout, parent, false);
            holder = new GroupHolder();
            holder.arrowImg = convertView.findViewById(R.id.arrow_img);
            holder.groupDes = convertView.findViewById(R.id.group_des);
            holder.dismissImg = convertView.findViewById(R.id.dismiss);
            holder.joinImg = convertView.findViewById(R.id.join);
            holder.leaveImg = convertView.findViewById(R.id.leave);
            holder.subscribeImg = convertView.findViewById(R.id.subscribe);
            holder.mixImg = convertView.findViewById(R.id.mix);
            convertView.setTag(holder);
        } else {
            holder = (GroupHolder) convertView.getTag();
        }
        RtcGroup group = getGroup(groupPosition);
        String groupId = group.groupId;
        holder.arrowImg.setImageResource(isExpanded ? R.drawable.arrow_up : R.drawable.arrow_down);
        holder.joinImg.setImageResource(R.drawable.join_group);
        holder.leaveImg.setImageResource(R.drawable.leave_group);
        holder.dismissImg.setImageResource(R.drawable.dismiss_group);
        holder.subscribeImg.setImageResource(R.drawable.subscribe_group);
        holder.mixImg.setImageResource(R.drawable.mix_group);
        holder.groupDes.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (listener != null) {
                    listener.onSetGroupName(groupId);
                }
            }
        });
        if (HALL_ID.equals(groupId)) {
            holder.groupDes.setText("大厅(" + getChildrenCount(groupPosition) + ")");
            holder.joinImg.setVisibility(View.GONE);
            holder.leaveImg.setVisibility(View.GONE);
            holder.dismissImg.setVisibility(View.GONE);
            holder.subscribeImg.setVisibility(View.VISIBLE);
            holder.subscribeImg.setOnClickListener(v -> {
                if (listener != null) {
                    listener.onSubscribeGroup(groupId);
                }
            });
            holder.mixImg.setVisibility(View.VISIBLE);
            holder.mixImg.setOnClickListener(v -> {
                if (listener != null) {
                    listener.onMixGroup(groupId);
                }
            });
        } else {
            String groupDes = groupId;
            if (!TextUtils.isEmpty(group.groupName)) {
                groupDes += "(" + group.groupName + ")";
            }
            holder.groupDes.setText(groupDes + "(" + getChildrenCount(groupPosition) + ")");
            if (group.isJoined()) {
                holder.joinImg.setVisibility(View.GONE);
                holder.leaveImg.setVisibility(View.VISIBLE);
                holder.leaveImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onLeaveGroup(groupId);
                    }
                });
                holder.dismissImg.setVisibility(View.VISIBLE);
                holder.dismissImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onDismissGroup(groupId);
                    }
                });
                holder.subscribeImg.setVisibility(View.VISIBLE);
                holder.subscribeImg.setImageResource(R.drawable.subscribe_group);
                holder.subscribeImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onSubscribeGroup(groupId);
                    }
                });
                holder.mixImg.setVisibility(View.VISIBLE);
                holder.mixImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onMixGroup(groupId);
                    }
                });
            } else {
                holder.joinImg.setVisibility(View.VISIBLE);
                holder.joinImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onJoinGroup(groupId);
                    }
                });
                holder.leaveImg.setVisibility(View.GONE);
                holder.dismissImg.setVisibility(View.VISIBLE);
                holder.dismissImg.setOnClickListener(v -> {
                    if (listener != null) {
                        listener.onDismissGroup(groupId);
                    }
                });
                holder.subscribeImg.setVisibility(View.GONE);
                holder.mixImg.setVisibility(View.GONE);
            }
        }
        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        MemberHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.member_item_layout, parent, false);
            holder = new MemberHolder();
            holder.memberDes = convertView.findViewById(R.id.member_des);
            convertView.setTag(holder);
        } else {
            holder = (MemberHolder) convertView.getTag();

        }
        DingRtcAudioGroupMember member = getChild(groupPosition, childPosition);
        holder.memberDes.setText("User: " + member.uid + "(usrData:" + member.usrData + ")");
        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

    public static class GroupHolder {
        ImageView arrowImg;
        TextView groupDes;
        ImageView joinImg;
        ImageView dismissImg;
        ImageView leaveImg;
        ImageView subscribeImg;
        ImageView mixImg;
    }

    public static class MemberHolder {
        TextView memberDes;
    }

    public interface GroupListListener {
        void onJoinGroup(String groupId);
        void onDismissGroup(String groupId);
        void onLeaveGroup(String groupId);
        void onSubscribeGroup(String groupId);
        void onMixGroup(String groupId);
        void onSetGroupName(String groupId);
    }
}
