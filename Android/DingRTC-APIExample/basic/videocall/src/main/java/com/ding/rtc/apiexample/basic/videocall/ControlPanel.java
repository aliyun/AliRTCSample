package com.ding.rtc.apiexample.basic.videocall;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.fragment.app.Fragment;

import com.ding.rtc.basic.videocall.R;

public class ControlPanel extends Fragment {

    private OnControlPanelListener mListener;

    private ImageView mAudioView;
    private boolean mAudioMuted = false;

    public ControlPanel() {
        // Required empty public constructor
    }

    static ControlPanel newInstance() {
        return new ControlPanel();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.control_panel, container, false);
        v.getBackground().setAlpha(0);

        mAudioView = v.findViewById(R.id.btn_audio);
        mAudioView.setOnClickListener( v1 -> {
            mAudioMuted = !mAudioMuted;
            if (mAudioMuted) {
                mAudioView.setImageResource(R.drawable.btn_audio_mute);
                mListener.onMuteAudio();
            } else {
                mAudioView.setImageResource(R.drawable.btn_audio_unmute);
                mListener.onUnmuteAudio();
            }
        });
        if (mAudioMuted) {
            mAudioView.setImageResource(R.drawable.btn_audio_mute);
        } else {
            mAudioView.setImageResource(R.drawable.btn_audio_unmute);
        }
        v.findViewById(R.id.btn_switch).setOnClickListener( v1 -> {
            mListener.onSwitchCamera();
        });
        v.findViewById(R.id.btn_settings).setOnClickListener( v1 -> {
            mListener.onSettings();
        });
        v.findViewById(R.id.btn_end_call).setOnClickListener( v1 -> {
            mListener.onEndCall();
        });

        return v;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnControlPanelListener) {
            mListener = (OnControlPanelListener) context;
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p/>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnControlPanelListener {
        void onMuteAudio();
        void onUnmuteAudio();
        void onSwitchCamera();
        void onSettings();
        void onEndCall();
    }
}
