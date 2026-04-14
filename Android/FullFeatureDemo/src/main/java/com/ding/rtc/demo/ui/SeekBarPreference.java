package com.ding.rtc.demo.ui;

import android.content.Context;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import com.ding.rtc.demo.R;

public class SeekBarPreference extends Preference implements SeekBar.OnSeekBarChangeListener {

    private static final int DEFAULT_VALUE = 0;

    private final int mMin = -100;
    private final int mMax = 100;
    private int mValue;
    private SeekBar mSeekBar;
    private TextView mValueText;

    public SeekBarPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected View onCreateView(ViewGroup parent) {
        super.onCreateView(parent);
        LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        return inflater.inflate(R.layout.preference_seekbar, parent, false);
    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        mSeekBar = view.findViewById(R.id.seekBar);
        mValueText = view.findViewById(R.id.valueText);

        mSeekBar.setMax(mMax - mMin);
        mSeekBar.setProgress(mValue - mMin);
        mSeekBar.setOnSeekBarChangeListener(this);
        updateValueText();
    }

    @Override
    protected Object onGetDefaultValue(TypedArray a, int index) {
        return a.getInt(index, DEFAULT_VALUE);
    }

    @Override
    protected void onSetInitialValue(boolean restorePersistedValue, Object defaultValue) {
        if (restorePersistedValue) {
            mValue = getPersistedInt(DEFAULT_VALUE);
        } else {
            mValue = (Integer) defaultValue;
            persistInt(mValue);
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        mValue = progress + mMin;
        updateValueText();
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        persistInt(mValue);
        notifyChanged();
    }

    private void updateValueText() {
        if (mValueText != null) {
            mValueText.setText(mValue + "%");
        }
    }
}