package com.ding.rtc.demo.whiteboard;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineWhiteboard;
import com.ding.rtc.DingRtcEngineWhiteboardEventListener;
import com.ding.rtc.DingRtcWhiteboardDocInfo;
import com.ding.rtc.DingRtcWhiteboardView;
import com.ding.rtc.api.DingRtcWhiteBoardTypes;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.MainActivity;
import com.ding.rtc.demo.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class WhiteboardActivity extends BaseActivity {
    public static final String TAG = WhiteboardActivity.class.getSimpleName();
    public final int[] colors = {Color.RED, Color.GREEN, Color.BLUE, Color.YELLOW, Color.CYAN, Color.MAGENTA};
    private static final int PICK_IMAGE_REQUEST = 1;
    private static final int REQUEST_WRITE_EXTERNAL_STORAGE = 111;
    private static DingRtcEngine mRtcEngine;
    DingRtcEngineWhiteboard mWhiteboard;
    DingRtcWhiteboardView mWbView;
    Handler mHandler;

    TextView mCurPageView;
    TextView mTotPageView;
    View mMorePane;
    View mDrawPane;
    View mClearPane;
    View mVisionPane;
    View mDocPane;
    View mImagePane;
    View mTextPane;

    View mSelectBtn;
    View mDrawBtn;
    View mReduBtn;
    View mUndoBtn;
    View mMoreBtn;

    Spinner mToolLineWidthSpin;
    Button mToolSelectColor;
    Button mToolSelectFillColor;

    Button mVisionShareBtn;
    Button mVisionFollowBtn;
    View mVisionSyncBtn;
    View mVisionResetBtn;

    Spinner mDocListSpin;

    TextView mImagePathTV;
    CheckBox mImageAsBackgrondCB;
    TextView mImageStatusTV;
    Button mFitModeBtn;
    Button mCropFillModeBtn;
    Button mCenterCropModeBtn;
    Button mStretchFillModeBtn;

    Spinner mTextSizeSpin;

    Spinner mRoleSpin;

    Spinner mSnapshotModeSpin;

    boolean isMorePaneVisible = false;
    boolean isDrawPaneVisible = false;
    boolean isClearPaneVisible = false;
    boolean isVisionPaneVisible = false;
    boolean isDocPaneVisible = false;
    boolean isImagePaneVisible = false;
    boolean isTextPaneVisible = false;

    boolean isStartVisionShare = true;
    boolean isStartVisionFollow = true;

    String mVisionShareStatus = "Vision free.";
    String mVisionFollowStatus = "Not follow.";

    List<DingRtcWhiteboardDocInfo> mDocInfoList = new ArrayList<>();
    List<String> mDocNameList = new ArrayList<>();
    ArrayAdapter mDocListAdapter;

    List<Integer> mLieWidthList = new ArrayList<>();
    ArrayAdapter mLineWidthAdapter;

    List<Integer> mTextSizeList = new ArrayList<>();
    ArrayAdapter mTextSizeAdapter;

    List<String> mRoleList = Arrays.asList("Admin", "Attendee", "Viewer");
    ArrayAdapter mRoleAdapter;

    List<String> mSnapshotModeList = Arrays.asList("View", "All");
    ArrayAdapter mSnapshotModeAdapter;

    private int mFontStyle = 0;
    private int mSnapshotMode = 0;

    private Uri mImageUri;
    private String mImagePath = "";

    public static void launch(Activity context, DingRtcEngine rtcEngine) {
        Intent intent = new Intent();
        intent.setClass(context, WhiteboardActivity.class);
        mRtcEngine = rtcEngine;
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_whiteboard);

        mMorePane = findViewById(R.id.pane_more);
        mDrawPane = findViewById(R.id.pane_draw);
        mClearPane = findViewById(R.id.pane_clear);
        mVisionPane = findViewById(R.id.pane_vision);
        mImagePane = findViewById(R.id.pane_image);
        mTextPane = findViewById(R.id.pane_text);
        mDocPane = findViewById(R.id.pane_doc);

        mWbView = findViewById(R.id.whiteboard_view);
        mCurPageView = findViewById(R.id.tv_page_current);
        mTotPageView = findViewById(R.id.tv_page_total);
        mSelectBtn = findViewById(R.id.btn_tool_select);
        mDrawBtn = findViewById(R.id.btn_tool_draw);
        mReduBtn = findViewById(R.id.btn_redo);
        mUndoBtn = findViewById(R.id.btn_undo);
        mMoreBtn = findViewById(R.id.btn_more);

        mToolLineWidthSpin = findViewById(R.id.spinner_tool_line_width);
        mToolSelectColor = findViewById(R.id.btn_tool_select_color);
        mToolSelectFillColor = findViewById(R.id.btn_tool_select_fill_color);

        mVisionShareBtn = findViewById(R.id.btn_vision_share);
        mVisionFollowBtn = findViewById(R.id.btn_vision_follow);
        mVisionSyncBtn = findViewById(R.id.btn_vision_sync);
        mVisionResetBtn = findViewById(R.id.btn_vision_reset);

        mDocNameList.add("doc0");
        mDocListSpin = findViewById(R.id.spinner_doc_list);
        mDocListAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, mDocNameList);
        mDocListAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mDocListSpin.setAdapter(mDocListAdapter);
        mDocListSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                if (i < mDocInfoList.size()) {
                    DingRtcWhiteboardDocInfo docInfo = mDocInfoList.get(i);
                    mWhiteboard.switchDoc(docInfo.getDocId());
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        for (int i = 1; i <= 20; i++) {
            mLieWidthList.add(i);
        }
        mToolLineWidthSpin = findViewById(R.id.spinner_tool_line_width);
        mLineWidthAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, mLieWidthList);
        mLineWidthAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mToolLineWidthSpin.setAdapter(mLineWidthAdapter);
        mToolLineWidthSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                Integer value = (Integer) adapterView.getItemAtPosition(i);
                mWhiteboard.setLineWidth(value);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mToolLineWidthSpin.setSelection(9);

        for (int i = 1; i <= 20; i++) {
            mTextSizeList.add(i);
        }
        mTextSizeSpin = findViewById(R.id.spinner_text_size);
        mTextSizeAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, mLieWidthList);
        mTextSizeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mTextSizeSpin.setAdapter(mTextSizeAdapter);
        mTextSizeSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                Integer value = (Integer) adapterView.getItemAtPosition(i);
                mWhiteboard.setFontSize(value);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mTextSizeSpin.setSelection(19);

        mImagePathTV = findViewById(R.id.tv_image_path);
        mImageAsBackgrondCB = findViewById(R.id.chk_image_background);
        mImageStatusTV = findViewById(R.id.tv_image_status);
        mFitModeBtn = findViewById(R.id.btn_image_fit);
        mCropFillModeBtn = findViewById(R.id.btn_image_crop_fill);
        mCenterCropModeBtn = findViewById(R.id.btn_image_center_crop);
        mStretchFillModeBtn = findViewById(R.id.btn_image_stretch_fill);

        mRoleSpin = findViewById(R.id.spinner_role_set);
        mRoleAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, mRoleList);
        mRoleAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mRoleSpin.setAdapter(mRoleAdapter);
        mRoleSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                mWhiteboard.setRole(DingRtcWhiteBoardTypes.DingRtcWBRoleType.fromValue(i));
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mRoleSpin.setSelection(1);

        mSnapshotModeSpin = findViewById(R.id.spinner_snapshot_mode);
        mSnapshotModeAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, mSnapshotModeList);
        mSnapshotModeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mSnapshotModeSpin.setAdapter(mSnapshotModeAdapter);
        mSnapshotModeSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                mSnapshotMode = i;
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mSnapshotModeSpin.setSelection(1);

        mHandler = new Handler();
        initWhiteboard();
    }

    public void unSelectAllBtn() {
        if (mSelectBtn.isSelected()) {
            mSelectBtn.setSelected(false);
        }
        if (mDrawBtn.isSelected()) {
            mDrawBtn.setSelected(false);
        }
        if (mMoreBtn.isSelected()) {
            mMoreBtn.setSelected(false);
        }
        if (mFitModeBtn.isSelected()) {
            mFitModeBtn.setSelected(false);
        }
        if (mCropFillModeBtn.isSelected()) {
            mCropFillModeBtn.setSelected(false);
        }
        if (mCenterCropModeBtn.isSelected()) {
            mCenterCropModeBtn.setSelected(false);
        }
        if (mStretchFillModeBtn.isSelected()) {
            mStretchFillModeBtn.setSelected(false);
        }
    }


    private void initWhiteboard() {
        mWhiteboard = mRtcEngine.getWhiteBoardManager().createWhiteboard("default", new DingRtcWhiteBoardTypes.DingRtcWBConfig(1280, 720, DingRtcWhiteBoardTypes.DingRtcWBMode.BASIC));
        mWhiteboard.setEventListener(new DingRtcEngineWhiteboardEventListener() {

            @Override
            public void onJoinResult(int result) {
                Log.d(TAG, "[DEMO] [Whiteboard] onJoinResult : " + result);
            }

            @Override
            public void onLeaveResult(int result) {
                Log.d(TAG, "[DEMO] [Whiteboard] onLeaveResult : " + result);
            }

            @Override
            public void onSnapshotComplete(int result, String filename) {
                Log.d(TAG, "[DEMO] [Whiteboard] onSnapshotComplete : " + result + ", filename " + filename);
            }

            @Override
            public void onUndoStatus(boolean undo) {
                Log.d(TAG, "[DEMO] [Whiteboard] onUndoStatus : " + undo);
                runOnUiThread(() -> mUndoBtn.setEnabled(undo));
            }

            @Override
            public void onRedoStatus(boolean redo) {
                Log.d(TAG, "[DEMO] [Whiteboard] onRedoStatus : " + redo);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mReduBtn.setEnabled(redo);
                    }
                });
            }

            @Override
            public void onZoomScaleChanged(float scale) {
                Log.d(TAG, "[DEMO] [Whiteboard] onZoomScaleChanged : " + scale);
            }

            @Override
            public void onDrawEvent(DingRtcWhiteBoardTypes.DingRtcWBDrawEvent event) {
                Log.d(TAG, "[DEMO] [Whiteboard] onDrawEvent : " + event.getValue());
            }

            @Override
            public void onContentUpdate(DingRtcWhiteBoardTypes.DingRtcWBContentUpdateType type) {
                Log.d(TAG, "[DEMO] [Whiteboard] onContentUpdate : " + type.getValue());
            }

            @Override
            public void onPageNumberChanged(int curPage, int totalPages) {
                Log.d(TAG, "[DEMO] [Whiteboard] onPageNumberChanged : curPage " + curPage + ", totalPages " + totalPages);
                updatePageInfo();
            }

            @Override
            public void onCreateDoc(int result, String docId) {
                Log.d(TAG, "[DEMO] [Whiteboard] [Document] onCreateDoc : " + result + ", docId " + docId);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateDocListAdapter();
                        WhiteboardActivity.this.showToast("onCreateDoc : " + result + ", docId " + docId + ".");
                    }
                });

            }

            @Override
            public void onDeleteDoc(int result, String docId) {
                Log.d(TAG, "[DEMO] [Whiteboard] [Document]  onDeleteDoc : " + result + ", docId " + docId);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateDocListAdapter();
                    }
                });

            }

            @Override
            public void onSwitchDoc(int result, String docId) {
                Log.d(TAG, "[DEMO] [Whiteboard] [Document]  onSwitchDoc : " + result + "");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        WhiteboardActivity.this.showToast("onSwitchDoc : " + result + ".");
                        DingRtcWhiteboardDocInfo docInfo;
                        int sel = 0;
                        for (int i = 0; i < mDocInfoList.size(); i++) {
                            docInfo = mDocInfoList.get(i);
                            Log.d(TAG, "onSwitchDoc " + docInfo.getDocId() + "," + i);
                            if (docId.equals(docInfo.getDocId())) {
                                sel = i;
                                break;
                            }
                        }
                        mDocListSpin.setSelection(sel);
                    }
                });
            }

            @Override
            public void onSaveDocComplete(int result, String docId, String outputDir) {
                Log.d(TAG, "[DEMO] [Whiteboard] onSaveDocComplete : " + result);
            }

            @Override
            public void onSaveDocProgress(String docId, int current, int total) {
                Log.d(TAG, "[DEMO] [Whiteboard] onSaveDocProgress : " + docId);
            }

            @Override
            public void onReceiveMessage(String userId, String msg, int size) {
                Log.d(TAG, "[DEMO] [Whiteboard] onReceiveMessage : " + userId);
            }

            @Override
            public void onVisionShareStarted(String userId) {
                Log.d(TAG, "[DEMO] [Whiteboard] onVisionShareStarted : " + userId);
                mVisionShareStatus = "Vision share start by " + userId;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateVisionStatus();
                        mVisionFollowBtn.setText(R.string.vision_start_follow);
                        mVisionFollowBtn.setEnabled(true);
                    }
                });

            }

            @Override
            public void onVisionShareStopped(String userId) {
                Log.d(TAG, "[DEMO] [Whiteboard] onVisionShareStopped : " + userId);
                mVisionShareStatus = "Vision free";
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateVisionStatus();
                        mVisionFollowBtn.setText(R.string.vision_start_follow);
                        mVisionFollowBtn.setEnabled(false);
                    }
                });

            }

            @Override
            public void onSaveDocThumbnailComplete(int result, String docId, String outputDir) {
                Log.d(TAG, "[DEMO] [Whiteboard] onSaveDocThumbnailComplete : " + result);
            }

            @Override
            public void onSaveDocThumbnailProgress(String docId, int current, int total) {
                Log.d(TAG, "[DEMO] [Whiteboard] onSaveDocThumbnailProgress : " + docId);
            }
        });

        updateDocListAdapter();

        mWhiteboard.setRole(DingRtcWhiteBoardTypes.DingRtcWBRoleType.ADMIN);
        mWhiteboard.setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH);
        mWhiteboard.join();
        mWhiteboard.setLineWidth(5);
        mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(1f, 0f, 0f, 1f));
        mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(.5f, .5f, .5f, 0.1f));
//        mWhiteboard.setScalingMode(DingRtcWhiteBoardTypes.DingRtcWBScalingMode.CenterCrop);
        mWhiteboard.setMinZoomScale(1f);
        mWhiteboard.setMaxZoomScale(4f);
//        mWhiteboard.setZoomScale(2f);
        mWhiteboard.open(mWbView);
        String stampId = "star";
        String stampPath = getApplicationContext().getExternalFilesDir("").getAbsolutePath() + File.separator + MainActivity.STAMP_FILE_NAME;
        Log.d(TAG, "File load stamp " + stampId);
        Log.d(TAG, "File load stamp " + stampPath);
        DingRtcWhiteBoardTypes.DingRtcWBStamp stamp = new DingRtcWhiteBoardTypes.DingRtcWBStamp(stampId, stampPath, false);
        mWhiteboard.addStamp(stamp);
        mWhiteboard.setBooleanOption(DingRtcWhiteBoardTypes.DingRtcWBOption.ENABLE_CURSORPOS_SYNC, true);
        mWhiteboard.setBooleanOption(DingRtcWhiteBoardTypes.DingRtcWBOption.ENABLE_SHOW_REMOTE_CURSOR, true);

        Log.d("MZWA", "model: " + Build.MODEL + ", brand: " + Build.BRAND);

        updatePageInfo();
    }

    private void updatePageInfo() {
        mHandler.post(() -> {
            if (mCurPageView != null) {
                mCurPageView.setText(String.valueOf(mWhiteboard.getCurrentPageNumber()));
            }
            if (mTotPageView != null) {
                mTotPageView.setText(String.valueOf(mWhiteboard.getTotalNumberOfPages()));
            }
        });
    }

    public void onClickMorePane(View view) {
        Log.i(TAG, "onClickMore");
        if (hasPaneVisible()) {
            hidePanes();
            ((Button) view).setText(R.string.title_more);
        } else {
            unSelectAllBtn();

            mMorePane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickMore visible");
            isMorePaneVisible = !isMorePaneVisible;
            ((Button) view).setText(R.string.title_back);
        }
    }

    public void onClickDrawPane(View view) {
        boolean visible = isDrawPaneVisible;
        Log.i(TAG, "onClickDrawPane");
        hidePanes();
        unSelectAllBtn();
        view.setSelected(true);
        if (!visible) {
            Color color = null;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                color = Color.valueOf(colors[0]);

                mToolSelectColor.setBackgroundColor(colors[0]);
                mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(color.red(), color.green(), color.blue(), color.alpha()));
                mDrawPane.setVisibility(View.VISIBLE);

                Color fillColor = Color.valueOf(colors[2]);
                mToolSelectFillColor.setBackgroundColor(colors[2]);
                mWhiteboard.setFillColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(fillColor.red(), fillColor.green(), fillColor.blue(), fillColor.alpha()));

                isDrawPaneVisible = true;
                Log.i(TAG, "onClickDrawPane visible");
            }
        }
    }

    public void onClickVisionPane(View view) {
        Log.i(TAG, "onClickVisionPane");
        unSelectAllBtn();
        hidePanes();
        if (!isVisionPaneVisible) {
            mVisionPane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickVisionPane visible");
            isVisionPaneVisible = !isVisionPaneVisible;
        }
    }

    public void onClickTextPane(View view) {
        Log.i(TAG, "onClickTextPane");
        mWhiteboard.setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType.fromValue(9));
        unSelectAllBtn();
        hidePanes();
        if (!isTextPaneVisible) {
            mTextPane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickTextPane visible");
            isTextPaneVisible = !isTextPaneVisible;
        }
    }

    public void onClickImagePane(View view) {
        Log.i(TAG, "onClickImagePane");
        unSelectAllBtn();
        hidePanes();
        if (!isImagePaneVisible) {
            mImagePane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickImagePane visible");
            isImagePaneVisible = !isImagePaneVisible;
        }
    }

    public void onClickDocPane(View view) {
        Log.i(TAG, "onClickDocPane");
        unSelectAllBtn();
        hidePanes();
        if (!isDocPaneVisible) {
            mDocPane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickDocPane visible");
            isDocPaneVisible = !isDocPaneVisible;
        }
    }

    public void onClickDocTool(View view) {
        switch (view.getId()) {
            case R.id.btn_doc_load_pdf:
                Log.i(TAG, "onClickDocTool load pdf start.");
                File pdfFile = new File(this.getExternalFilesDir(""), MainActivity.PDF_FILE_NAME);
                if (pdfFile.exists()) {
                    DingRtcWhiteBoardTypes.DingRtcWBDocContents contents = new DingRtcWhiteBoardTypes.DingRtcWBDocContents();
                    contents.name = pdfFile.getName();
                    contents.transDocId = "";
                    contents.type = DingRtcWhiteBoardTypes.DingRtcWBDocType.PDF;
                    contents.urls = new ArrayList<>();
                    contents.urls.add(pdfFile.getAbsolutePath());
                    this.showToast("pdf file path: " + pdfFile.getAbsolutePath());
                    mWhiteboard.createDoc(contents, true);
                } else {
                    this.showToast("pdf file not exists.");
                }
                Log.i(TAG, "onClickDocTool load pdf over.");
                break;
            case R.id.btn_doc_load_word:
                Log.i(TAG, "onClickDocTool load word start.");
                File wordFile = new File(this.getExternalFilesDir(""), MainActivity.WORD_FILE_NAME);
                if (wordFile.exists()) {
                    DingRtcWhiteBoardTypes.DingRtcWBDocContents contents = new DingRtcWhiteBoardTypes.DingRtcWBDocContents();
                    contents.name = wordFile.getName();
                    contents.transDocId = "";
                    contents.type = DingRtcWhiteBoardTypes.DingRtcWBDocType.PDF;
                    contents.urls = new ArrayList<>();
                    contents.urls.add(wordFile.getAbsolutePath());
                    this.showToast("word file path: " + wordFile.getAbsolutePath());
                    String docId = mWhiteboard.createDoc(contents, true);
                } else {
                    this.showToast("word file not exists.");
                }
                Log.i(TAG, "onClickDocTool load word over.");
                break;
            case R.id.btn_doc_load_html:
                this.showToast("html not support.");
                break;
            case R.id.btn_doc_delete:
                Log.i(TAG, "onClickDocTool delete doc start.");
                String docId = mWhiteboard.getCurrentDocID();
                String docName = mWhiteboard.getDocInfo(docId).getName();
                mWhiteboard.deleteDoc(docId);
                Log.i(TAG, "onClickDocTool delete doc over.");
                break;
            case R.id.btn_doc_save:
                try {
                    mWhiteboard.saveDocToImages(mWhiteboard.getCurrentDocID(), this.getExternalFilesDir("savefiles").getAbsolutePath());
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
        }
    }

    public void onClickVisionTool(View view) {
        switch (view.getId()) {
            case R.id.btn_vision_share:
                if (isStartVisionShare) {
                    mWhiteboard.startShareVision();

                    mVisionShareBtn.setText(R.string.vision_stop_share);
                } else {
                    mWhiteboard.stopShareVision();
                    mVisionShareBtn.setText(R.string.vision_start_share);

                }
                isStartVisionShare = !isStartVisionShare;
                break;
            case R.id.btn_vision_follow:
                if (isStartVisionFollow) {
                    mWhiteboard.startFollowVision();

                    mVisionFollowBtn.setText(R.string.vision_stop_follow);
                    mVisionFollowStatus = "Following.";
                } else {
                    mWhiteboard.stopFollowVision();
                    mVisionFollowBtn.setText(R.string.vision_start_follow);
                    mVisionFollowStatus = "Not follow.";
                }
                isStartVisionFollow = !isStartVisionFollow;
                updateVisionStatus();
                break;
            case R.id.btn_vision_sync:
//                mWhiteboard.syncVision();
                break;
            case R.id.btn_vision_reset:
//                mWhiteboard.resetVision();
                break;
        }
    }

    public void updateVisionStatus() {
        setTitle(mVisionShareStatus + mVisionFollowStatus);
    }

    public void updateDocListAdapter() {
        mDocInfoList.clear();
        List<String> docList = mWhiteboard.getDocList();
        for (String docId : docList) {
            DingRtcWhiteboardDocInfo info = mWhiteboard.getDocInfo(docId);
            mDocInfoList.add(info);
        }
        mDocNameList.clear();
        for (DingRtcWhiteboardDocInfo docInfo : mDocInfoList) {
            mDocNameList.add(docInfo.getName());
        }
        //mDocListAdapter.clear();
        //mDocListAdapter.addAll(mDocNameList);
        mDocListAdapter.notifyDataSetChanged();
    }

    public void onClickSelectTool(View view) {
        int value = 1;
        if (view.getTag() != null) {
            String tag = (String) view.getTag();
            value = Integer.parseInt(tag);
            Log.i(TAG, "onClickDrawPane value = " + value);
        }

        DingRtcWhiteBoardTypes.DingRtcWBToolType toolType = DingRtcWhiteBoardTypes.DingRtcWBToolType.fromValue(value);
        if (toolType == DingRtcWhiteBoardTypes.DingRtcWBToolType.STAMP) {
            mWhiteboard.setStamp("star");
        }
        mWhiteboard.setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType.fromValue(value));
        unSelectAllBtn();
        view.setSelected(true);
        Log.i(TAG, "onClickDrawPane tool type = " + DingRtcWhiteBoardTypes.DingRtcWBToolType.fromValue(value));

        hidePanes();
    }

    public void onClickScalingMode(View view) {
        int value = 1;
        if (view.getTag() != null) {
            String tag = (String) view.getTag();
            value = Integer.parseInt(tag);
            Log.i(TAG, "onClickScalingMode value = " + value);
        }

        mWhiteboard.setScalingMode(DingRtcWhiteBoardTypes.DingRtcWBScalingMode.fromValue(value));
        unSelectAllBtn();
        view.setSelected(true);
        Log.i(TAG, "onClickScalingMode scaling mode = " + DingRtcWhiteBoardTypes.DingRtcWBScalingMode.fromValue(value));

        hidePanes();
    }

    public void onClickSetFontStyle(View view) {
        int value = 1;
        if (view.getTag() != null) {
            String tag = (String) view.getTag();
            value = Integer.parseInt(tag);
            Log.i(TAG, "onClickScalingMode value = " + value);
        }
        Button btn = (Button) view;
        btn.setSelected(!btn.isSelected());
        if (btn.isSelected()) {
            mFontStyle += value;
        } else {
            mFontStyle -= value;
        }

        mWhiteboard.setFontStyle(DingRtcWhiteBoardTypes.DingRtcWBFontStyle.fromValue(mFontStyle));
        unSelectAllBtn();
        Log.i(TAG, "onClickSetFontStyle scaling mode = " + DingRtcWhiteBoardTypes.DingRtcWBFontStyle.fromValue(mFontStyle));
    }

    public void onClickImageButton(View view) {
        switch (view.getId()) {
            case R.id.btn_image_select:
                mImagePath = "";
                openImageFileChooser();
                break;
            case R.id.btn_image_set:
                if (mImagePath != null && !mImagePath.trim().equals("")) {
                    if (mImageAsBackgrondCB.isChecked()) {
                        // mImagePath = getApplicationContext().getExternalFilesDir("").getAbsolutePath() + File.separator + MainActivity.STAMP_FILE_NAME;
                        mWhiteboard.setBackgroundImage(mImagePath, 1);
                        mImageStatusTV.setText("set background image.");
                    } else {
                        mWhiteboard.addImageFile(mImagePath);
                        mImageStatusTV.setText("add image.");
                    }
                }
                break;
        }
    }

    boolean hasPaneVisible() {
        return isDrawPaneVisible || isClearPaneVisible || isVisionPaneVisible || isImagePaneVisible || isMorePaneVisible || isDocPaneVisible || isTextPaneVisible;
    }

    void hidePanes() {
        if (isDrawPaneVisible) {
            mDrawPane.setVisibility(View.GONE);
            isDrawPaneVisible = !isDrawPaneVisible;
        }
        if (isClearPaneVisible) {
            mClearPane.setVisibility(View.GONE);
            isClearPaneVisible = !isClearPaneVisible;
        }

        if (isVisionPaneVisible) {
            mVisionPane.setVisibility(View.GONE);
            isVisionPaneVisible = !isVisionPaneVisible;
        }

        if (isImagePaneVisible) {
            mImagePane.setVisibility(View.GONE);
            isImagePaneVisible = !isImagePaneVisible;
        }

        if (isTextPaneVisible) {
            mTextPane.setVisibility(View.GONE);
            isTextPaneVisible = !isTextPaneVisible;
        }

        if (isMorePaneVisible) {
            mMorePane.setVisibility(View.GONE);
            isMorePaneVisible = !isMorePaneVisible;
        }
        if (isDocPaneVisible) {
            mDocPane.setVisibility(View.GONE);
            isDocPaneVisible = !isDocPaneVisible;
        }
    }

    public void onClickPage(View view) {
        switch (view.getId()) {
            case R.id.btn_page_first:
                mWhiteboard.gotoPage(1);
                break;
            case R.id.btn_page_pre:
                mWhiteboard.prevPage();
                break;
            case R.id.btn_page_next:
                mWhiteboard.nextPage();
                break;
            case R.id.btn_page_last:
                mWhiteboard.gotoPage(mWhiteboard.getTotalNumberOfPages());
                break;
            case R.id.btn_page_create:
                mWhiteboard.addPage(true);
                break;
            case R.id.btn_page_delete:
                mWhiteboard.removePage(mWhiteboard.getCurrentPageNumber(), true);
                break;
        }
        hidePanes();
        updatePageInfo();
    }

    public void onClick(View view) {
        DingRtcWhiteBoardTypes.DingRtcWBClearParam param;
        if (mWhiteboard == null) {
            Log.e(TAG, "onClick whiteboard is null.");
            return;
        }
        switch (view.getId()) {
            case R.id.btn_undo:
                Log.d(TAG, "onClick undo.");
                mWhiteboard.undo();
                break;
            case R.id.btn_redo:
                Log.d(TAG, "onClick redo.");
                mWhiteboard.redo();
                break;
            case R.id.btn_clear_all:
                param = new DingRtcWhiteBoardTypes.DingRtcWBClearParam(true, DingRtcWhiteBoardTypes.DingRtcWBClearMode.ALL);
                mWhiteboard.clearContents(param);
                break;
            case R.id.btn_clear_other:
                param = new DingRtcWhiteBoardTypes.DingRtcWBClearParam(true, DingRtcWhiteBoardTypes.DingRtcWBClearMode.OTHERS);
                mWhiteboard.clearContents(param);
                break;
            case R.id.btn_clear_myself:
                param = new DingRtcWhiteBoardTypes.DingRtcWBClearParam(true, DingRtcWhiteBoardTypes.DingRtcWBClearMode.SELF);
                mWhiteboard.clearContents(param);
                break;
        }
    }

    public void onClickClearPane(View view) {
        Log.i(TAG, "onClickClearPane");
        unSelectAllBtn();
        view.setSelected(true);
        if (isClearPaneVisible) {
            mClearPane.setVisibility(View.GONE);
            Log.i(TAG, "onClickClearPane gone");
        } else {
            mClearPane.setVisibility(View.VISIBLE);
            Log.i(TAG, "onClickClearPane visible");
        }
        isClearPaneVisible = !isClearPaneVisible;
    }

    private void openImageFileChooser() {
        // 创建意图以打开文件选择器
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("image/*");  // 设置类型为所有文件
        intent.addCategory(Intent.CATEGORY_OPENABLE);

        try {
            startActivityForResult(
                    Intent.createChooser(intent, "选择文件"),
                    PICK_IMAGE_REQUEST);
        } catch (android.content.ActivityNotFoundException ex) {
            // 文件管理器不可用时处理异常
            mImagePathTV.setText("请安装文件管理器.");
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == PICK_IMAGE_REQUEST && resultCode == Activity.RESULT_OK) {
            if (data != null) {
                mImageUri = data.getData();
                mImagePath = copyImageToSdCard();
                Log.i(TAG, "pick image url " + mImagePath);
                mImagePathTV.setText(mImagePath);
            }
        }
    }

    private String copyImageToSdCard() {
        String imageFile = "";
        if (mImageUri == null) {
            return imageFile;
        }
        // 检查是否有写入存储的权限
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    REQUEST_WRITE_EXTERNAL_STORAGE);
            return imageFile;
        }

        try {
            InputStream in = getContentResolver().openInputStream(mImageUri);
            File sdCardDir = getApplicationContext().getExternalFilesDir("");
            File targetFile = new File(sdCardDir, "copied_image_" + System.currentTimeMillis() + ".jpg");
            OutputStream out = new FileOutputStream(targetFile);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            out.flush();
            out.close();

            imageFile = targetFile.getAbsolutePath();
            Toast.makeText(this, "图片已成功复制到" + targetFile.getAbsolutePath(), Toast.LENGTH_LONG).show();
        } catch (IOException e) {
            e.printStackTrace();
            Toast.makeText(this, "复制图片失败", Toast.LENGTH_SHORT).show();
        }
        return imageFile;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_WRITE_EXTERNAL_STORAGE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 用户同意了权限请求
                mImagePath = copyImageToSdCard();
                Log.i(TAG, "pick image url " + mImagePath);
                mImagePathTV.setText(mImagePath);
            } else {
                // 用户拒绝了权限请求
                Toast.makeText(WhiteboardActivity.this.getApplicationContext(), "Write External Storage is not permitted!!!", Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void onClickSelectColor(View view) {
        String[] colorNames = {"Red", "Green", "Blue", "Yellow", "Cyan", "Magenta"};

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Choose a color");
        builder.setItems(colorNames, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                // 设置选中的颜色
                Color color = null;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    color = Color.valueOf(colors[which]);

                    mToolSelectColor.setBackgroundColor(colors[which]);
                    mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(color.red(), color.green(), color.blue(), color.alpha()));
                } else {
                    mToolSelectColor.setBackgroundColor(colors[which]);
                    float r = Color.red(colors[which]) / 255.0f;
                    float g = Color.green(colors[which]) / 255.0f;
                    float b = Color.blue(colors[which]) / 255.0f;
                    float a = Color.alpha(colors[which]) / 255.0f;
                    mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(r, g, b, a));
                }
            }
        });
        builder.show();
    }

    public void onClickSelectFillColor(View view) {
        final int[] colors = {Color.RED, Color.GREEN, Color.BLUE, Color.YELLOW, Color.CYAN, Color.MAGENTA};
        String[] colorNames = {"Red", "Green", "Blue", "Yellow", "Cyan", "Magenta"};

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Choose a color");
        builder.setItems(colorNames, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                // 设置选中的颜色
                Color color = null;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    color = Color.valueOf(colors[which]);
                    mToolSelectFillColor.setBackgroundColor(colors[which]);
                    mWhiteboard.setFillColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(color.red(), color.green(), color.blue(), color.alpha()));
                } else {
                    mToolSelectColor.setBackgroundColor(colors[which]);
                    float r = Color.red(colors[which]) / 255.0f;
                    float g = Color.green(colors[which]) / 255.0f;
                    float b = Color.blue(colors[which]) / 255.0f;
                    float a = Color.alpha(colors[which]) / 255.0f;
                    mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(r, g, b, a));
                }
            }
        });
        builder.show();
    }

    public void onClickSnapshot(View view) {
        mWhiteboard.snapshot(DingRtcWhiteBoardTypes.DingRtcWBSnapshotMode.fromValue(mSnapshotMode), getExternalFilesDir("").getAbsolutePath());
    }

    @Override
    protected void onDestroy() {
        if (mWhiteboard != null) {
            mWhiteboard.destroy();
            mWhiteboard = null;
        }
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        if (mWhiteboard != null) {
            mWhiteboard.setEventListener(null);
            //mWhiteboard.stop();
            mWhiteboard.close();
            mWhiteboard.leave();
        }
        super.onBackPressed();
    }
}
