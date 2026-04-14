package com.ding.rtc.demo.settings;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.demo.R;

import java.io.File;

public class FacebeautyFilterAdapter extends BaseAdapter {
    private final Context context;
    private final LayoutInflater inflater;
    private static final int[] IMG_RESOURCES = new int[]{R.drawable.heibai};

    private static final String[] IMG_DES = new String[]{"黑白"};

    private static final String[] FILTER_FILE_NAMES = new String[]{"heibai_res.png"};

    private int selectedPos = -1;

    FacebeautyFilterAdapter(Context context, String lastFilterPath) {
        this.context = context;
        inflater = LayoutInflater.from(context);
        for (int i = 0; i < FILTER_FILE_NAMES.length; i++) {
            if (lastFilterPath.contains(FILTER_FILE_NAMES[i])) {
                selectedPos = i;
                break;
            }
        }
    }

    String getSelectedFilterPath() {
        if (selectedPos == -1 || selectedPos >= FILTER_FILE_NAMES.length) {
            return "";
        }
        String fileName = FILTER_FILE_NAMES[selectedPos];
        File file = new File(context.getFilesDir().getAbsolutePath(), fileName);
        if (!file.exists() || !file.isFile()) {
            return "";
        }
        return file.getAbsolutePath();
    }

    @Override
    public int getCount() {
        return IMG_RESOURCES.length;
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        FilterHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.facebeauty_filter_item_layout, parent, false);
            holder = new FilterHolder();
            holder.img = convertView.findViewById(R.id.filter_img);
            holder.des = convertView.findViewById(R.id.filter_des);
            holder.checkBox = convertView.findViewById(R.id.filter_img_checkbox);
            convertView.setTag(holder);
        } else {
            holder = (FilterHolder) convertView.getTag();
        }
        holder.img.setImageResource(IMG_RESOURCES[position]);
        holder.des.setText(IMG_DES[position]);
        holder.checkBox.setChecked(selectedPos == position);
        holder.checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    selectedPos = position;
                } else {
                    if (selectedPos == position) {
                        selectedPos = -1;
                    }
                }
                notifyDataSetChanged();
            }
        });
        return convertView;
    }

    private static class FilterHolder {
        ImageView img;
        TextView des;
        CheckBox checkBox;
    }

}
