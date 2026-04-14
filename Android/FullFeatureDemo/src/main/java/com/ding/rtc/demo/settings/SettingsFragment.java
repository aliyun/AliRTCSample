package com.ding.rtc.demo.settings;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.SwitchPreference;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.ding.rtc.demo.R;


public class SettingsFragment extends PreferenceFragment implements Preference.OnPreferenceChangeListener, SharedPreferences.OnSharedPreferenceChangeListener {

    static int position;
    private Dialog faceBeautyDialog;
    private boolean hasBindFaceBeautyDefaultValue = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        PreferenceManager.setDefaultValues(getActivity().getBaseContext(), R.xml.setting_pref, false);
        addPreferencesFromResource(R.xml.setting_pref);

        bindPreferenceSummaryToValue(findPreference(getString(R.string.render_grid_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_render_crop_mode_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_publish_resolution_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_publish_fps_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_screen_share_publish_resolution_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_screen_share_publish_fps_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.demo_subscribe_resolution_key)));

        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_profile_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_denoise_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_scenario_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_observer_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_playout_volume)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.audio_recording_volume)));

        bindPreferenceSummaryToValue(findPreference(getString(R.string.video_observer_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.video_observer_format_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.video_encoder_orientation_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.video_camera_orientation_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.video_max_bitrate_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.screen_max_bitrate_key)));

        bindPreferenceSummaryToValue(findPreference(getString(R.string.title_face_beauty_key)));

        bindPreferenceSummaryToValue(findPreference(getString(R.string.title_camera_rotation_key)));
        bindPreferenceSummaryToValue(findPreference(getString(R.string.title_virtual_background_key)));

        PreferenceManager.getDefaultSharedPreferences(getActivity()).registerOnSharedPreferenceChangeListener(this);
    }

    private void showFaceBeautyDialog() {
        if (faceBeautyDialog == null) {
            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
            View rootView = LayoutInflater.from(getActivity()).inflate(R.layout.face_beauty_dialog, null);
            TextView skinBuffingText = rootView.findViewById(R.id.skin_buffing_text);
            float defaultBuffingValue = preferences.getFloat("skin_buffing", 0.5f);
            skinBuffingText.setText("磨皮(" + defaultBuffingValue + ")" );
            SeekBar skinBuffingSeekbar = rootView.findViewById(R.id.skin_buffing_seekbar);
            skinBuffingSeekbar.setProgress((int) (defaultBuffingValue * 100));
            skinBuffingSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    float value = progress * 1.0f / 100;
                    skinBuffingText.setText("磨皮(" + value + ")" );
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });

            TextView skinSharpenText = rootView.findViewById(R.id.skin_sharpen_text);
            float defaultSharpeValue = preferences.getFloat("skin_sharpen", 0.5f);
            skinSharpenText.setText("锐化(" + defaultSharpeValue + ")");
            SeekBar skinSharpenSeekbar = rootView.findViewById(R.id.skin_sharpen_seekbar);
            skinSharpenSeekbar.setProgress((int) (defaultSharpeValue * 100));
            skinSharpenSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    float value = progress * 1.0f / 100;
                    skinSharpenText.setText("锐化(" + value + ")");

                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });

            TextView skinWhiteningText = rootView.findViewById(R.id.skin_whitening_text);
            float defaultWhitingValue = preferences.getFloat("skin_whitening", 0.5f);
            skinWhiteningText.setText("美白(" + defaultWhitingValue + ")");
            SeekBar skinWhiteningSeekbar = rootView.findViewById(R.id.skin_whitening_seekbar);
            skinWhiteningSeekbar.setProgress((int) (defaultWhitingValue * 100));
            skinWhiteningSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    float value = progress * 1.0f / 100;
                    skinWhiteningText.setText("美白(" + value + ")");
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });

            TextView skinRosyText = rootView.findViewById(R.id.skin_rosy_text);
            float defaultRosyValue = preferences.getFloat("skin_rosy", 0.0f);
            skinRosyText.setText("红润(" + defaultRosyValue + ")");
            SeekBar skinRosySeekbar = rootView.findViewById(R.id.skin_rosy_seekbar);
            skinRosySeekbar.setProgress((int) (defaultRosyValue * 100));
            skinRosySeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    float value = progress * 1.0f / 100;
                    skinRosyText.setText("红润(" + value + ")");
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });

            GridView filterGridView = rootView.findViewById(R.id.filter_imgs);
            String lastFilterPath = PreferenceManager.getDefaultSharedPreferences(getActivity()).getString("filter_path", "");
            FacebeautyFilterAdapter adapter = new FacebeautyFilterAdapter(getActivity(), lastFilterPath);
            filterGridView.setAdapter(adapter);

            faceBeautyDialog = new AlertDialog.Builder(getActivity())
                    .setView(rootView)
                    .create();
            faceBeautyDialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
                @Override
                public void onDismiss(DialogInterface dialog) {
                    String filterPath = adapter.getSelectedFilterPath();
                    if (TextUtils.isEmpty(filterPath)) {
                        Toast.makeText(SettingsFragment.this.getActivity(), "Filter 文件路径为空", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(SettingsFragment.this.getActivity(), "Filter 文件路径" + filterPath, Toast.LENGTH_SHORT).show();
                    }
                    SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(getActivity()).edit();
                    editor.putFloat("skin_buffing", skinBuffingSeekbar.getProgress() * 1.0f / 100)
                            .putFloat("skin_sharpen", skinSharpenSeekbar.getProgress() * 1.0f / 100)
                            .putFloat("skin_whitening", skinWhiteningSeekbar.getProgress() * 1.0f / 100)
                            .putFloat("skin_rosy", skinRosySeekbar.getProgress() * 1.0f / 100)
                            .putString("filter_path", filterPath)
                            .apply();
                }
            });
        }
        faceBeautyDialog.show();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        PreferenceManager.getDefaultSharedPreferences(getActivity()).unregisterOnSharedPreferenceChangeListener(this);
    }


    private Preference bindPreferenceSummaryToValue(Preference preference) {
        return bindPreferenceSummaryToValue(preference, null);
    }

    private Preference bindPreferenceSummaryToValue(Preference preference, Preference.OnPreferenceChangeListener listener) {
        Preference.OnPreferenceChangeListener delegate = listener == null ? this : (preference1, newValue) -> {
            SettingsFragment.this.onPreferenceChange(preference1, newValue);
            listener.onPreferenceChange(preference1, newValue);
            return true;
        };
        preference.setOnPreferenceChangeListener(delegate);
        bindPreferenceDefValue(preference);
        return preference;
    }

    private void bindPreferenceDefValue(Preference preference) {
        preference.getOnPreferenceChangeListener().onPreferenceChange(preference,
                PreferenceManager
                        .getDefaultSharedPreferences(preference.getContext())
                        .getString(preference.getKey(), ""));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        String stringValue = newValue.toString();

        if (preference instanceof ListPreference) {
            ListPreference listPreference = (ListPreference) preference;
            int prefIndex = listPreference.findIndexOfValue(stringValue);
            if (prefIndex >= 0) {
                preference.setSummary(listPreference.getEntries()[prefIndex]);
            }
            Log.d("SettingsFragment", "onPreferenceChange: " + preference.getSummary() + ", " + newValue);

            if (preference.getKey().endsWith(getString(R.string.title_face_beauty_key))) {
                if (hasBindFaceBeautyDefaultValue) {
                    if ("1".equals(stringValue) || "2".equals(stringValue)) {
                        showFaceBeautyDialog();
                    }
                } else {
                    hasBindFaceBeautyDefaultValue = true;
                }
            }

        } else if (preference instanceof SwitchPreference) {
            //ignore
        } else {
            preference.setSummary(stringValue);
        }
        return true;
    }


    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ListView lst = getView().findViewById(android.R.id.list);
        if (lst != null && position > 0) {
            lst.setSelection(position);
        }
    }

    @Override
    public void onDestroyView() {
        ListView lst = getView().findViewById(android.R.id.list);
        if (lst != null) {
            position = lst.getFirstVisiblePosition();
        }
        super.onDestroyView();
    }


    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
    }

}
