package com.ding.rtc.demo.utils;

import android.app.Activity;
import android.graphics.Point;
import android.view.WindowManager;

public class DensityUtils {
    private static int mScreenWidth;
    private static int mScreenHeight;

    public static int getScreenWidth(Activity activity) {
        if (mScreenWidth == 0) {
            WindowManager manager = activity.getWindowManager();
            Point point = new Point();
            manager.getDefaultDisplay().getRealSize(point);
            mScreenWidth = point.x;
            mScreenHeight = point.y;
        }
        return mScreenWidth;
    }
}
