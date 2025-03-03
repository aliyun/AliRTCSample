//
//  WhiteboardViewController.m
//  Whiteboard
//
//  Created by mark on 2025/2/17.
//

#import "WhiteboardViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"

@interface WhiteboardViewController () <DingRtcEngineDelegate, DingRtcEngineWhiteboardDelegate>

@property (weak, nonatomic) IBOutlet UIView *contentView;
@property (weak, nonatomic) IBOutlet UIButton *openWhiteboard;
@property (weak, nonatomic) IBOutlet UIButton *pathTool;
@property (weak, nonatomic) IBOutlet UIButton *textTool;
@property (weak, nonatomic) IBOutlet UILabel *message;

@property (strong, nonatomic) DingRtcEngine *engine;
@property (strong, nonatomic) DingRtcEngineWhiteboard *whiteboardEngine;
@property (strong, nonatomic) UserManager *userManager;

@end

@implementation WhiteboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (IBAction)exitChannel:(UIButton *)sender {
    [self closeWhiteboardEngine];
    [self leaveChannel];
    [self destroyEngineKit];
    UIApplication.sharedApplication.idleTimerDisabled = NO;
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)clickOpenWhiteboard:(UIButton *)sender {
    self.openWhiteboard.selected = !self.openWhiteboard.isSelected;
    NSLog(@"click \"Open Whiteboard\" button with %ld", self.openWhiteboard.isSelected);
    if (self.openWhiteboard.isSelected) {
        [self openWhiteboardEngine];
    } else {
        [self closeWhiteboardEngine];
    }
}

- (IBAction)clickPathTool:(UIButton *)sender {
    NSLog(@"click \"Path Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        self.pathTool.selected = NO;
        return;
    }
    self.pathTool.selected = !self.pathTool.isSelected;
    if (self.pathTool.isSelected) {
        [self.whiteboardEngine setToolType:DingRtcWbToolPath];
    } else {
        [self.whiteboardEngine setToolType:DingRtcWbToolSelect];
    }
    
    // Reset text tool
    self.textTool.selected = NO;
}

- (IBAction)clickTextTool:(UIButton *)sender {
    NSLog(@"click \"Text Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        self.textTool.selected = NO;
        return;
    }
    self.textTool.selected = !self.textTool.isSelected;
    if (self.textTool.isSelected) {
        [self.whiteboardEngine setToolType:DingRtcWbToolText];
    } else {
        [self.whiteboardEngine setToolType:DingRtcWbToolSelect];
    }
    
    // Reset path tool
    self.pathTool.selected = NO;
}

- (IBAction)clickClearAll:(UIButton *)sender {
    NSLog(@"click \"Text Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        return;
    }
    DingRtcWbClearParam *param = [DingRtcWbClearParam new];
    param.mode = DingRtcWbClearModeAll;
    [self.whiteboardEngine clearContents:param];
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
    
    NSString *userName = [@"iOS_" stringByAppendingString:ChannelInfo.userId];
    int result = [self.engine joinChannel:info name:userName onResultWithUserId:^(NSInteger errCode, NSString * _Nonnull channel, NSString * _Nonnull userId, NSInteger elapsed) {
        if (!errCode) {
            [self displayMessage:[NSString stringWithFormat:@"Join channel successfully."]];
            
            //注意：不要在SDK回调线程里调用SDK接口！
            dispatch_async(dispatch_get_main_queue(), ^{
                [self openWhiteboardEngine];
            });
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

- (void)openWhiteboardEngine {
    DingRtcEngineWhiteboardManager *wbManager = [self.engine getWhiteboardManager];
    DingRtcEngineWhiteboard *wb = [wbManager getWhiteboard:@"default"];
    if (!wb) {
        DingRtcWbConfig *config = [DingRtcWbConfig new];
        config.mode = DingRtcWbModeBasic;
        config.width = 400;
        config.height = 600;
        wb = [wbManager createWhiteboard:@"default" withConfig:config];
    }
    [wb setDelegate:self];
    [wb setRole:DingRtcWbRoleTypeAdmin];
    [wb open:self.contentView];
    self.whiteboardEngine = wb;
}

- (void)closeWhiteboardEngine {
    [self.whiteboardEngine close];
    [self.whiteboardEngine leave];
    self.whiteboardEngine = nil;
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

//MARK: - DingRtcEngineDelegate

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

//MARK: - DingRtcEngineWhiteboardDelegate

- (void)whiteboard:(NSString *)whiteboardId onJoinResult:(int)result {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"whiteboard:%@, join result: %d.", whiteboardId, result]];
    });
}

- (void)whiteboard:(NSString *)whiteboardId onLeaveResult:(int)result {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"whiteboard:%@, leave result: %d.", whiteboardId, result]];
    });
}

@end
