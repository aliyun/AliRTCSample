//
//  ChannelViewController.m
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"

@interface ChannelViewController () <DingRtcEngineDelegate>

@property (strong, nonatomic) IBOutlet UIView * remoteView;
@property (strong, nonatomic) IBOutlet UIView * localView;
@property (strong, nonatomic) IBOutlet UIView * thumbnail1View;
@property (strong, nonatomic) IBOutlet UIView * thumbnail2View;
@property (strong, nonatomic) IBOutlet UILabel * message;

@property (strong, nonatomic) DingRtcEngine * engine;
@property (strong, nonatomic) UserManager * userManager;

@end

@implementation ChannelViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    UIApplication.sharedApplication.idleTimerDisabled = YES;
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (IBAction)enableAudio:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.engine muteLocalMic:!sender.selected mode:DingRtcMuteAudioModeDefault];
}

- (IBAction)switchSpeaker:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.engine enableSpeakerphone:sender.selected];
}

- (IBAction)enableVideo:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self startVideo];
    } else {
        [self stopVideo];
    }
}

- (IBAction)switchCamera:(UIButton *)sender {
    [self.engine switchCamera];
}

- (IBAction)exitChannel:(UIButton *)sender {
    [self leaveChannel];
    [self destroyEngineKit];
    UIApplication.sharedApplication.idleTimerDisabled = NO;
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)onBye:(DingRtcOnByeType)code {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Leave channel with indication: %ld.", (long)code]];
        UIApplication.sharedApplication.idleTimerDisabled = NO;
        [self dismissViewControllerAnimated:YES completion:nil];
    });
}

- (void)onOccurError:(int)error message:(NSString *_Nonnull)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Occur error: %ld, message:%@.", (long)error, message]];
    });
}

- (void)onOccurWarning:(int)warn message:(NSString *_Nonnull)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Occur warning: %ld, message:%@.", (long)warn, message]];
    });
}

- (void)onRemoteUserOnLineNotify:(NSString *_Nonnull)uid elapsed:(int)elapsed {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSDictionary *infoDic = [self.engine getUserInfo:uid];
        NSString *userName = infoDic[@"displayName"];
        [self displayMessage:[NSString stringWithFormat:@"User %@(%@) join channel.", uid, userName]];
        [self.userManager addUser:uid withName:userName];
    });
}

- (void)onRemoteUserOffLineNotify:(NSString *_Nonnull)uid offlineReason:(DingRtcUserOfflineReason)reason {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager removeUser:uid];
        [self displayMessage:[NSString stringWithFormat:@"User %@(%@) leave channel with reason: %ld.", uid, user.userName, reason]];
        if (user.videoView) {
            // Release render view
            [self stopRemoteVideo:uid withView:user.videoView];
            // Find a waiting user and subscribe it.
            UserInfo * waitingUser = [self.userManager findWatingUser];
            if (waitingUser) {
                waitingUser.videoView = user.videoView;
                [self subscribeVideo:waitingUser.userId withView:waitingUser.videoView];
            }
            // Clear video view of the leave user.
            user.videoView = nil;
        }
    });
}

- (void)onRemoteTrackAvailableNotify:(NSString *_Nonnull)uid audioTrack:(DingRtcAudioTrack)audioTrack videoTrack:(DingRtcVideoTrack)videoTrack {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        BOOL audioEnable = audioTrack != DingRtcAudioTrackNo;
        BOOL videoEnable = videoTrack == DingRtcVideoTrackBoth || videoTrack == DingRtcVideoTrackCamera;

        if (audioEnable != user.audioEnable) {
            user.audioEnable = audioEnable;
            if (audioEnable) {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) start audio.", uid, user.userName]];
            } else {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) stop audio.", uid, user.userName]];
            }
        }
        if (videoEnable != user.videoEnable) {
            user.videoEnable = videoEnable;
            if (videoEnable) {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) start video.", uid, user.userName]];
                user.videoView = [self findIdleVideoView];
                if (user.videoView) {
                    [self subscribeVideo:uid withView:user.videoView];
                }
            } else {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) stop video.", uid, user.userName]];
                if (user.videoView) {
                    // Release render view
                    [self stopRemoteVideo:uid withView:user.videoView];
                    // Find a waiting user and subscribe it.
                    UserInfo * waitingUser = [self.userManager findWatingUser];
                    if (waitingUser) {
                        waitingUser.videoView = user.videoView;
                        [self subscribeVideo:waitingUser.userId withView:waitingUser.videoView];
                    }
                    // Clear video view of the stoped video user.
                    user.videoView = nil;
                }
            }
        }
    });
}

- (void)onUserAudioMuted:(NSString *_Nonnull)uid audioMuted:(BOOL)isMute {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        user.audioMute = isMute;
        if (isMute) {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) mute audio.", uid, user.userName]];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) unmute audio.", uid, user.userName]];
        }
    });
}

- (void)onUserVideoMuted:(NSString *_Nonnull)uid videoMuted:(BOOL)isMute track:(DingRtcVideoTrack)track {
    if (track == DingRtcVideoTrackScreen) {
        // 仅示例相机流
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        user.videoMute = isMute;
        if (isMute) {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) mute video.", uid, user.userName]];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) unmute video.", uid, user.userName]];
        }
    });
}

- (void)onAudioPublishStateChanged:(DingRtcPublishState)oldState newState:(DingRtcPublishState)newState elapseSinceLastState:(NSInteger)elapseSinceLastState channel:(NSString *_Nonnull)channel  {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Audio publish state changed with oldState: %ld newState: %ld.", (long)oldState, (long)newState]];
    });
}

- (void)onVideoPublishStateChanged:(DingRtcPublishState)oldState newState:(DingRtcPublishState)newState elapseSinceLastState:(NSInteger)elapseSinceLastState channel:(NSString *_Nonnull)channel {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Video publish state changed with oldState: %ld newState: %ld.", (long)oldState, (long)newState]];
    });
}

- (void)createEngineKit {
    self.engine = [DingRtcEngine sharedInstance:self extras:nil];
}

- (void)destroyEngineKit {
    [DingRtcEngine destroy];
    self.engine = nil;
}

- (void)joinChannel {
    DingRtcAuthInfo *info = [[DingRtcAuthInfo alloc] init];
    info.channelId = ChannelInfo.channelId;
    info.userId = ChannelInfo.userId;
    info.appId = ChannelInfo.appId;
    info.token = ChannelInfo.token;
    info.gslbServer = ChannelInfo.gslbServer;
    
    [self.engine publishLocalAudioStream:YES];
    [self.engine publishLocalVideoStream:YES];
    [self.engine subscribeAllRemoteVideoStreams:NO];
    
    NSString *userName = [@"iOS_" stringByAppendingString:ChannelInfo.userId];
    int result = [self.engine joinChannel:info name:userName onResultWithUserId:^(NSInteger errCode, NSString * _Nonnull channel, NSString * _Nonnull userId, NSInteger elapsed) {
        if (!errCode) {
            [self displayMessage:[NSString stringWithFormat:@"Join channel successfully."]];
            [self startVideo];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)errCode]];
        }
    }];
    if (result != 0) {
        [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)result]];
    }
}

- (void)leaveChannel {
    [self.engine leaveChannel];
}

- (void)initUserManager {
    self.userManager = [[UserManager alloc] init];
}

- (void)displayMessage:(NSString *)message {
    NSLog(@"%@", message);
    self.message.text = message;
    self.message.hidden = NO;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        self.message.hidden = YES;
    });
}

- (UIView *)findIdleVideoView {
    NSArray<UIView *> *videoViews = @[self.remoteView,
                                      self.thumbnail1View,
                                      self.thumbnail2View];
    UIView * videoView = nil;
    for (videoView in videoViews) {
        if ([self.userManager findUserWithView:videoView] == nil) {
            break;
        }
    }
    return videoView;
}

- (void)startVideo {
    [self.engine muteLocalCamera:NO forTrack:DingRtcVideoTrackCamera];
    [self.engine enableLocalVideo:YES];
    
    //开启预览
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.localView isPub:YES trackType:DingRtcVideoTrackCamera];
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackCamera];
    [self.engine startPreview];
}

- (void)stopVideo {
    [self.engine muteLocalCamera:YES forTrack:DingRtcVideoTrackCamera];
    [self.engine enableLocalVideo:NO];
    
    //关闭预览
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.localView isPub:YES trackType:DingRtcVideoTrackCamera];
    canvasConfig.toBeRemoved = YES;
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackCamera];
    [self.engine stopPreview];
}

- (void)subscribeVideo:(NSString *)userId withView:(UIView *)view {
    [self.engine subscribeRemoteVideoStream:userId track:DingRtcVideoTrackCamera sub:YES];
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackCamera];
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackCamera];
}

- (void)stopRemoteVideo:(NSString *)userId withView:(UIView *)view {
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackCamera];
    canvasConfig.toBeRemoved = YES;
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackCamera];
}

- (DingRtcVideoCanvas *)getCanvasWithView:(UIView *)view isPub:(BOOL)isPub trackType:(DingRtcVideoTrack )trackType {
    // mirror mode
    DingRtcRenderMirrorMode mirrorMode = DingRtcRenderMirrorModeAllDisabled;
    if (isPub && trackType == DingRtcVideoTrackCamera) {
        mirrorMode = DingRtcRenderMirrorModeOnlyFrontCameraPreviewEnabled;
    }
    
    DingRtcRotationMode rotationMode = DingRtcRotationMode_0;
    DingRtcRenderMode renderMode = DingRtcRenderModeFill;
    
    DingRtcVideoCanvas *canvas = [[DingRtcVideoCanvas alloc] init];
    canvas.view = view;
    canvas.renderMode = renderMode;
    canvas.mirrorMode = mirrorMode;
    canvas.backgroundColor = 0x000000;
    canvas.rotationMode = rotationMode;
    return canvas;
}

@end
