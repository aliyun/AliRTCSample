//
//  BaseViewController.m
//  AdvancedSamples
//
//  Created by mark on 2025/1/7.
//

#import "BaseViewController.h"

@interface BaseViewController ()

@property (nonatomic, strong) UILabel *message;

@end

@implementation BaseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.message = [[UILabel alloc] init];
    [self.view addSubview:self.message];
    self.message.textColor = [UIColor yellowColor];
    self.message.backgroundColor = [UIColor lightGrayColor];
    self.message.alpha = 0.5;
    self.message.hidden = YES;
    
    self.message.translatesAutoresizingMaskIntoConstraints = NO;
    [self.message.centerYAnchor constraintEqualToAnchor:self.view.centerYAnchor constant:0].active = YES;
    [self.message.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor constant:0].active = YES;
    [self.message.widthAnchor constraintEqualToConstant:350].active = YES;
    [self.message.heightAnchor constraintEqualToConstant:100].active = YES;
    
    UIApplication.sharedApplication.idleTimerDisabled = YES;
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (void)channelJoinSuccess {
    
}

- (void)channelJoinFailed {
    
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
            [self channelJoinSuccess];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)errCode]];
            [self channelJoinFailed];
        }
    }];
    if (result != 0) {
        [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)result]];
        [self channelJoinFailed];
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

@end
