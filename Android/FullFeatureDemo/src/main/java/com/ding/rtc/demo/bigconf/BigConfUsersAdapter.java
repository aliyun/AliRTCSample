package com.ding.rtc.demo.bigconf;

import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsNoSubscribe;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsSubscribeIdle;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.demo.R;

import java.util.ArrayList;
import java.util.List;

public class BigConfUsersAdapter extends RecyclerView.Adapter<BigConfUsersAdapter.UserHolder> {
    private final Context context;
    private final LayoutInflater inflater;
    private final DingRtcEngine engine;
    private final int column;
    private final List<BigConfUser> users;
    private int renderWidth;
    private int renderHeight;

    public BigConfUsersAdapter(Context context, DingRtcEngine engine, int column) {
        this.context = context;
        inflater = LayoutInflater.from(context);
        this.engine = engine;
        this.column = column;

        users = new ArrayList<>();
    }

    public void setUsers(List<BigConfUser> users) {
        this.users.clear();
        this.users.addAll(users);
        notifyDataSetChanged();
    }

    public List<BigConfUser> getAllUsers() {
        return users;
    }

    public void addUser(BigConfUser userInfo) {
        int pos = users.size();
        users.add(userInfo);
        notifyItemInserted(pos);
    }

    public boolean removeUser(String uid) {
        if (TextUtils.isEmpty(uid)) {
            return false;
        }
        int pos = -1;
        for (int i = 0; i < users.size(); i++) {
            if (uid.equals(users.get(i).uid)) {
                pos = i;
                break;
            }
        }
        if (pos > -1) {
            users.remove(pos);
            notifyItemRemoved(pos);
            return true;
        }
        return false;
    }

    public void updateUser(BigConfUser user) {
        if (user == null || TextUtils.isEmpty(user.uid)) {
            return;
        }
        int pos = -1;
        for (int i = 0; i < users.size(); i++) {
            if (users.get(i).uid.equals(user.uid)) {
                pos = i;
                break;
            }
        }
        if (pos > -1) {
            users.set(pos, user);
            notifyItemChanged(pos);
        }
    }

    public BigConfUser getLastUser() {
        if (users.size() > 0) {
            return users.get(users.size() - 1);
        }
        return null;
    }

    public int getUserCount() {
        return users.size();
    }

    @NonNull
    @Override
    public UserHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        if (renderWidth == 0) {
            renderWidth = parent.getWidth() / column;
            renderHeight = parent.getHeight() / column;
        }
        View renderLayout = inflater.inflate(R.layout.bigconf_render_layout, parent, false);
        renderLayout.setLayoutParams(new ViewGroup.LayoutParams(renderWidth, renderHeight));
        return new UserHolder(renderLayout);
    }

    @Override
    public void onBindViewHolder(@NonNull UserHolder holder, int position) {
        BigConfUser user = users.get(position);
        if (!TextUtils.isEmpty(user.nickName)) {
            holder.nickNameTv.setText(user.nickName);
        } else {
            holder.nickNameTv.setText(user.uid);
        }
        if (user.isSelf) {
            DingRtcEngine.DingRtcVideoCanvas canvas = new DingRtcEngine.DingRtcVideoCanvas();
            SurfaceView cameraView = DingRtcEngine.createRenderSurfaceView(context);
            canvas.view = cameraView;
            canvas.renderMode = DingRtcRenderModeAuto;
            engine.setLocalViewConfig(canvas, DingRtcVideoTrackCamera);
            holder.renderContainer.addView(cameraView);
            holder.renderContainer.setVisibility(View.VISIBLE);
            holder.avatar.setVisibility(View.GONE);
        } else {
            if (user.isVideoStarted) {
                if (user.videoSubscribeState == DingRtcStatsSubscribeIdle || user.videoSubscribeState == DingRtcStatsNoSubscribe) {
                    engine.subscribeRemoteVideoStream(user.uid, DingRtcVideoTrackCamera, true);
                    DingRtcEngine.DingRtcVideoCanvas canvas = new DingRtcEngine.DingRtcVideoCanvas();
                    SurfaceView cameraView = DingRtcEngine.createRenderSurfaceView(context);
                    canvas.view = cameraView;
                    canvas.renderMode = DingRtcRenderModeFill;
                    engine.setRemoteViewConfig(canvas, user.uid, DingRtcVideoTrackCamera);
                    holder.renderContainer.addView(cameraView);
                    holder.renderContainer.setVisibility(View.VISIBLE);
                    holder.avatar.setVisibility(View.GONE);
                    user.subedStreamType = DingRtcVideoTrackCamera;
                }
            } else if (user.isScreenStarted) {
                if (user.videoSubscribeState == DingRtcStatsSubscribeIdle || user.videoSubscribeState == DingRtcStatsNoSubscribe) {
                    engine.subscribeRemoteVideoStream(user.uid, DingRtcVideoTrackScreen, true);
                    DingRtcEngine.DingRtcVideoCanvas canvas = new DingRtcEngine.DingRtcVideoCanvas();
                    SurfaceView cameraView = DingRtcEngine.createRenderSurfaceView(context);
                    canvas.view = cameraView;
                    canvas.renderMode = DingRtcRenderModeAuto;
                    engine.setRemoteViewConfig(canvas, user.uid, DingRtcVideoTrackScreen);
                    holder.renderContainer.addView(cameraView);
                    holder.renderContainer.setVisibility(View.VISIBLE);
                    holder.avatar.setVisibility(View.GONE);
                    user.subedStreamType = DingRtcVideoTrackScreen;
                }
            } else {
                holder.renderContainer.removeAllViews();
                holder.renderContainer.setVisibility(View.GONE);
                holder.avatar.setVisibility(View.VISIBLE);
            }
        }
    }

    @Override
    public int getItemCount() {
        return users.size();
    }

    public static class UserHolder extends RecyclerView.ViewHolder {

        public TextView nickNameTv;
        public ViewGroup renderContainer;
        public ImageView avatar;

        public UserHolder(@NonNull View itemView) {
            super(itemView);

            nickNameTv = itemView.findViewById(R.id.user_name);
            renderContainer = itemView.findViewById(R.id.user_render_container);
            avatar = itemView.findViewById(R.id.user_avatar);
        }
    }
}
