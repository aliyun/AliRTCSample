package com.ding.rtc.demo.subtitle;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.ding.rtc.DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage;
import com.ding.rtc.demo.utils.LimitedSizeLinkedList;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class SubtitleTranslateAdapter extends BaseAdapter {

    private final LayoutInflater inflater;
    private List<String> data;

    public SubtitleTranslateAdapter(Context context) {
        inflater = LayoutInflater.from(context);
    }

    public void updateMessage(Map<String, Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>>> data) {
        this.data = new ArrayList<>(data.size());
        for (Map.Entry<String, Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>>> entry: data.entrySet()) {
            StringBuilder content = new StringBuilder();
            String uid = entry.getKey();
            content.append(uid).append("\n");
            Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>> lanSubtitleMsgs = entry.getValue();
            content.append("[原语言(不翻译)]");
            LimitedSizeLinkedList<DingRtcSubtitleMessage> sourceSubtitleMsgs = lanSubtitleMsgs.get("source");
            if (sourceSubtitleMsgs != null) {
                for (DingRtcSubtitleMessage msg : sourceSubtitleMsgs) {
                    content.append("{").append(msg.sentenceIndex).append("}").append(msg.subtitle);
                }
            }
            content.append("\n");
            for (Map.Entry<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>> lanSubtitleMsgEntry : lanSubtitleMsgs.entrySet()) {
                String lan = lanSubtitleMsgEntry.getKey();
                if (!"source".equals(lan)) {
                    content.append("[").append(SubtitleManager.getInstance().getCnTranslateLanguage(lan)).append("]");
                    LimitedSizeLinkedList<DingRtcSubtitleMessage> subtitleMsgs = lanSubtitleMsgEntry.getValue();
                    for (DingRtcSubtitleMessage msg : subtitleMsgs) {
                        content.append("{").append(msg.sentenceIndex).append("}").append(msg.subtitle);
                    }
                    content.append("\n");
                }
            }
            this.data.add(content.toString());
        }
    }

    @Override
    public int getCount() {
        return data != null ? data.size() : 0;
    }

    @Override
    public String getItem(int position) {
        if (data == null || data.size() <= position) {
            return "";
        }
        return data.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        SubtitleHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(android.R.layout.simple_list_item_1, parent, false);
            holder = new SubtitleHolder();
            holder.content = convertView.findViewById(android.R.id.text1);
            convertView.setTag(holder);
        } else {
            holder = (SubtitleHolder) convertView.getTag();
        }
        holder.content.setText(getItem(position));
        return convertView;
    }

    private static class SubtitleHolder {
        TextView content;
    }

}
