package com.ding.rtc.apiexample.basic.videocall;

import android.content.Context;
import android.content.Intent;
import android.app.Activity;
import android.os.Bundle;
import android.media.projection.MediaProjectionManager;

// ScreenCapturePermissionActivity.java
public class ScreenCapturePermissionActivity extends Activity {
    private static final int SYSTEM_CAPTURE_REQUEST = 3000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MediaProjectionManager projectionManager =
                (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);

        if (projectionManager != null) {
            startActivityForResult(
                    projectionManager.createScreenCaptureIntent(),
                    SYSTEM_CAPTURE_REQUEST
            );
        } else {
            setResult(RESULT_CANCELED);
            finish();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == SYSTEM_CAPTURE_REQUEST) {
            setResult(resultCode, data);
            finish();
        }
    }
}

