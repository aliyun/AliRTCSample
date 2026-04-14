package com.ding.rtc.demo;

import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.demo.ui.ToastCompat;

public class BaseActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setScreenOnFlag();

    }

    public void showToastL(String msg) {
        runOnUiThread(() -> {
            Toast toast = ToastCompat.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG);
            toast.show();
        });
    }

    public void showToast(String msg) {
        runOnUiThread(() -> {
            Toast toast = ToastCompat.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT);
            toast.show();
        });
    }

    private void setScreenOnFlag() {
        final int keepScreenOnFlag =
                WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                        WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED |
                        WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON |
                        WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
        Window w = getWindow();
        w.getAttributes().flags |= keepScreenOnFlag;
        w.addFlags(keepScreenOnFlag);
    }
}