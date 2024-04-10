//
//  ChannelViewController.m
//  BasicDemo
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"

@interface ChannelViewController () <DingRtcEngineDelegate>

@property (weak) IBOutlet NSView *remote1View;
@property (weak) IBOutlet NSView *thumbnail1View;
@property (weak) IBOutlet NSView *thumbnail2View;
@property (weak) IBOutlet NSView *local1View;
@property (weak) IBOutlet NSView *local2View;
@property (weak) IBOutlet NSButton *sendAudio;
@property (weak) IBOutlet NSButton *sendVideo;
@property (weak) IBOutlet NSButton *sendScreen;
@property (weak) IBOutlet NSTextField *message;

@property (strong, nonatomic) DingRtcEngine * engine;
@property (strong, nonatomic) UserManager * userManager;

@end

@implementation ChannelViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    self.view.window.title = ChannelInfo.channelId;
}

- (IBAction)clickSendAudio:(id)sender {
    NSLog(@"click \"Send Audio\" button with %ld", self.sendAudio.state);
    BOOL isSend = (self.sendAudio.state == NSControlStateValueOn) ? YES : NO;
    [self.engine muteLocalMic:!isSend mode:DingRtcMuteAudioModeDefault];
}

- (IBAction)clickSendVideo:(id)sender {
    NSLog(@"click \"Send Video\" button with %ld", self.sendVideo.state);
    BOOL isSend = (self.sendVideo.state == NSControlStateValueOn) ? YES : NO;
    if (isSend) {
        [self startVideo];
    } else {
        [self stopVideo];
    }
}

- (IBAction)clickSendScreen:(id)sender {
    NSLog(@"click \"Send Screen\" button with %ld", self.sendScreen.state);
    BOOL isSend = (self.sendScreen.state == NSControlStateValueOn) ? YES : NO;
    if (isSend) {
        [self startScreenShare];
    } else {
        [self stopScreenShare];
    }
}

- (IBAction)leaveChannel:(id)sender {
    NSLog(@"click \"Leave Channel\" button");
    [self leaveChannel];
    [self destroyEngineKit];
    [self.view.window performClose:nil];
}

- (void)onBye:(DingRtcOnByeType)code {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Leave channel with indication: %ld.", (long)code]];
        [self.view.window performClose:nil];
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
        BOOL screenEnable = videoTrack == DingRtcVideoTrackBoth || videoTrack == DingRtcVideoTrackScreen;

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
                    if (user.videoView == self.remote1View) {
                        // Move thumbnail view user to big remote view if exist
                        UserInfo *tmpUser = [self.userManager findUserWithView:self.thumbnail2View];
                        if (tmpUser) {
                            [self stopRemoteVideo:tmpUser.userId withView:tmpUser.videoView];
                            tmpUser.videoView = self.remote1View;
                            [self subscribeVideo:tmpUser.userId withView:tmpUser.videoView];
                        }
                        user.videoView = self.thumbnail2View;
                    }
                    if (user.screenView) {
                        // Release render screen share
                        [self stopRemoteScreen:uid withView:user.screenView];
                        user.screenView = nil;
                    }
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
        if (screenEnable != user.screenEnable) {
            user.screenEnable = screenEnable;
            if (screenEnable) {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) start screen share.", uid, user.userName]];
                if (user.videoView == self.remote1View) {
                    // Only display remote view user screen share
                    [self subscribeScreen:uid withView:self.thumbnail1View];
                    user.screenView = self.thumbnail1View;
                }
            } else {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) stop screen share.", uid, user.userName]];
                if (user.screenView) {
                    // Release render screen share
                    [self stopRemoteScreen:uid withView:user.screenView];
                    user.screenView = nil;
                }
            }
        }
    });
}

- (void)onUserAudioMuted:(NSString *_Nonnull)uid audioMuted:(BOOL)isMute {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        if (isMute) {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) mute audio.", uid, user.userName]];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) unmute audio.", uid, user.userName]];
        }
    });
}

- (void)onUserVideoMuted:(NSString *_Nonnull)uid videoMuted:(BOOL)isMute track:(DingRtcVideoTrack)track {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
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
    
    NSString *userName = [@"Mac_" stringByAppendingString:ChannelInfo.userId];
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
    dispatch_async(dispatch_get_main_queue(), ^{
        NSLog(@"%@", message);
        self.message.stringValue = message;
        self.message.hidden = NO;
    });
}

- (NSView *)findIdleVideoView {
    NSArray<NSView *> *videoViews = @[self.remote1View,
                                      self.thumbnail2View];
    NSView * videoView = nil;
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
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.local1View isPub:YES trackType:DingRtcVideoTrackCamera];
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackCamera];
    [self.engine startPreview];
}

- (void)stopVideo {
    [self.engine muteLocalCamera:YES forTrack:DingRtcVideoTrackCamera];
    [self.engine enableLocalVideo:NO];
    
    //关闭预览
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.local1View isPub:YES trackType:DingRtcVideoTrackCamera];
    canvasConfig.toBeRemoved = YES;
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackCamera];
    [self.engine stopPreview];
}

- (void)startScreenShare {
    //开启屏幕共享
    NSArray<DingRtcScreenSourceInfo *> *deskSources = [self.engine getScreenShareSourceInfoWithType:DingRtcScreenShareDesktop];
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.local2View isPub:YES trackType:DingRtcVideoTrackScreen];
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackScreen];
    [self.engine startScreenShareWithDesktopId:deskSources.firstObject.sourceId config:[DingRtcScreenShareConfig new]];
}

- (void)stopScreenShare {
    //关闭渲染
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:self.local2View isPub:YES trackType:DingRtcVideoTrackScreen];
    canvasConfig.toBeRemoved = YES;
    [self.engine setLocalViewConfig:canvasConfig forTrack:DingRtcVideoTrackScreen];
    [self.engine stopScreenShare];
}

- (void)subscribeVideo:(NSString *)userId withView:(NSView *)view {
    [self.engine subscribeRemoteVideoStream:userId track:DingRtcVideoTrackCamera sub:YES];
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackCamera];
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackCamera];
}

- (void)stopRemoteVideo:(NSString *)userId withView:(NSView *)view {
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackCamera];
    canvasConfig.toBeRemoved = YES;
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackCamera];
}

- (void)subscribeScreen:(NSString *)userId withView:(NSView *)view {
    [self.engine subscribeRemoteVideoStream:userId track:DingRtcVideoTrackScreen sub:YES];
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackScreen];
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackScreen];
}

- (void)stopRemoteScreen:(NSString *)userId withView:(NSView *)view {
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view isPub:NO trackType:DingRtcVideoTrackScreen];
    canvasConfig.toBeRemoved = YES;
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackScreen];
}

- (DingRtcVideoCanvas *)getCanvasWithView:(NSView *)view isPub:(BOOL)isPub trackType:(DingRtcVideoTrack )trackType {
    // mirror mode
    DingRtcRenderMirrorMode mirrorMode = DingRtcRenderMirrorModeAllDisabled;
    if (isPub && trackType == DingRtcVideoTrackCamera) {
        mirrorMode = DingRtcRenderMirrorModeAllEnabled;
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
