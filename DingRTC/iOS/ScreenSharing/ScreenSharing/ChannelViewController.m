//
//  ChannelViewController.m
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"
#import <ReplayKit/ReplayKit.h>

@interface ChannelViewController () <DingRtcEngineDelegate>

@property (strong, nonatomic) IBOutlet UIView * remoteView;
@property (strong, nonatomic) IBOutlet UIView * thumbnail1View;
@property (strong, nonatomic) IBOutlet UIView * thumbnail2View;
@property (strong, nonatomic) IBOutlet UILabel * message;

@property (strong, nonatomic) DingRtcEngine * engine;
@property (strong, nonatomic) UserManager * userManager;

@property (strong, nonatomic) RPSystemBroadcastPickerView *broadcastPickerView;

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

- (IBAction)enableScreen:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self.engine startScreenShare:[ChannelInfo appGroupId] mode:DingRtcScreenShareAll];
        [self launchBroadcastPickerView];
    } else {
        [self.engine stopScreenShare];
    }
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
        if (user.screenView) {
            // Release render view
            [self stopRemoteVideo:uid withView:user.screenView];
            // Find a waiting user and subscribe it.
            UserInfo * waitingUser = [self.userManager findWatingUser];
            if (waitingUser) {
                waitingUser.screenView = user.screenView;
                [self subscribeScreen:waitingUser.userId withView:waitingUser.screenView];
            }
            // Clear video view of the leave user.
            user.screenView = nil;
        }
    });
}

- (void)onRemoteTrackAvailableNotify:(NSString *_Nonnull)uid audioTrack:(DingRtcAudioTrack)audioTrack videoTrack:(DingRtcVideoTrack)videoTrack {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        BOOL screenEnable = videoTrack == DingRtcVideoTrackBoth || videoTrack == DingRtcVideoTrackScreen;

        if (screenEnable != user.screenEnable) {
            user.screenEnable = screenEnable;
            if (screenEnable) {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) start screen sharing.", uid, user.userName]];
                user.screenView = [self findIdleVideoView];
                if (user.screenView) {
                    [self subscribeScreen:uid withView:user.screenView];
                }
            } else {
                [self displayMessage:[NSString stringWithFormat:@"User %@(%@) stop screen.", uid, user.userName]];
                if (user.screenView) {
                    // Release render view
                    [self stopRemoteVideo:uid withView:user.screenView];
                    // Find a waiting user and subscribe it.
                    UserInfo * waitingUser = [self.userManager findWatingUser];
                    if (waitingUser) {
                        waitingUser.screenView = user.screenView;
                        [self subscribeScreen:waitingUser.userId withView:waitingUser.screenView];
                    }
                    // Clear video view of the leave user.
                    user.screenView = nil;
                }
            }
        }
    });
}

- (void)onUserVideoMuted:(NSString *_Nonnull)uid videoMuted:(BOOL)isMute track:(DingRtcVideoTrack)track {
    if (track == DingRtcVideoTrackCamera) {
        // 仅示例屏幕共享流
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:uid];
        user.screenMute = isMute;
        if (isMute) {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) mute screen.", uid, user.userName]];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"User %@(%@) unmute screen.", uid, user.userName]];
        }
    });
}

- (void)onScreenSharePublishStateChanged:(DingRtcPublishState)oldState newState:(DingRtcPublishState)newState elapseSinceLastState:(NSInteger)elapseSinceLastState channel:(NSString *_Nonnull)channel {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Screen Sharing publish state changed with oldState: %ld newState: %ld.", (long)oldState, (long)newState]];
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
    
    [self.engine subscribeAllRemoteVideoStreams:NO];
    
    NSString *userName = [@"iOS_" stringByAppendingString:ChannelInfo.userId];
    int result = [self.engine joinChannel:info name:userName onResultWithUserId:^(NSInteger errCode, NSString * _Nonnull channel, NSString * _Nonnull userId, NSInteger elapsed) {
        if (!errCode) {
            [self displayMessage:[NSString stringWithFormat:@"Join channel successfully."]];
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

- (void)subscribeScreen:(NSString *)userId withView:view {
    [self.engine subscribeRemoteVideoStream:userId track:DingRtcVideoTrackScreen sub:YES];
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view];
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackScreen];
}

- (void)stopRemoteVideo:(NSString *)userId withView:(UIView *)view {
    DingRtcVideoCanvas *canvasConfig = [self getCanvasWithView:view];
    canvasConfig.toBeRemoved = YES;
    [self.engine setRemoteViewConfig:canvasConfig uid:userId forTrack:DingRtcVideoTrackScreen];
}

- (DingRtcVideoCanvas *)getCanvasWithView:(UIView *)view {
    DingRtcRenderMirrorMode mirrorMode = DingRtcRenderMirrorModeAllDisabled;
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

- (void)launchBroadcastPickerView {
    if (@available(iOS 12.0, *)) {
        if (!self.broadcastPickerView) {
            RPSystemBroadcastPickerView *pickerView = [[RPSystemBroadcastPickerView alloc] initWithFrame:CGRectMake(0, 0, 44, 44)];
            pickerView.showsMicrophoneButton = NO;
            pickerView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin;
            NSString *pluginPath = [NSBundle mainBundle].builtInPlugInsPath;
            NSArray *contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pluginPath error:nil];
            for (NSString *content in contents) {
                if (![content hasSuffix:@".appex"]) {
                    continue;
                }
                NSBundle *bundle = [NSBundle bundleWithPath:[[NSURL fileURLWithPath:pluginPath] URLByAppendingPathComponent:content].path];
                if (bundle) {
                    NSString *identifier = [bundle.infoDictionary valueForKeyPath:@"NSExtension.NSExtensionPointIdentifier"];
                    if ([identifier isEqualToString:@"com.apple.broadcast-services-upload"]) {
                        pickerView.preferredExtension = bundle.bundleIdentifier;
                    }
                }
            }
            self.broadcastPickerView = pickerView;
        }
        
        for (UIView *view in self.broadcastPickerView.subviews) {
            if ([view isKindOfClass:[UIButton class]]) {
                [(UIButton *)view sendActionsForControlEvents:UIControlEventAllEvents];
            }
        }
    } else {
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            
        }];
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:@"Screen Sharing not support below iOS 11 Device." preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:cancel];
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

@end
