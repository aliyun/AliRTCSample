package com.ding.rtc.demo.ui;

import android.view.View;
import android.view.ViewGroup;
import android.widget.GridLayout;

import androidx.annotation.NonNull;
import androidx.viewpager.widget.PagerAdapter;

import java.util.ArrayList;
import java.util.List;

public class ViewPagerAdapter extends PagerAdapter {
    private final List<GridLayout> gridList;

    public ViewPagerAdapter() {
        gridList = new ArrayList<>();
    }

    public void add(List<GridLayout> datas) {
        if (!gridList.isEmpty()) {
            gridList.clear();
        }
        gridList.addAll(datas);
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return gridList.size();
    }

    @Override
    public int getItemPosition(@NonNull Object object) {
        return POSITION_NONE;
    }

    @Override
    public boolean isViewFromObject(@NonNull View view, @NonNull Object object) {
        return view == object;
    }

    @NonNull
    @Override
    public Object instantiateItem(ViewGroup container, int position) {
        container.addView(gridList.get(position));
        return gridList.get(position);
    }

    @Override
    public void destroyItem(ViewGroup container, int position, @NonNull Object object) {
        container.removeView((View) object);
    }
}
