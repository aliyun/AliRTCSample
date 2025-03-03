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

@property (weak) IBOutlet NSView *contentView;
@property (weak) IBOutlet NSButton *openWhiteboard;
@property (weak) IBOutlet NSButton *pathTool;
@property (weak) IBOutlet NSButton *textTool;
@property (weak) IBOutlet NSTextField *message;

@property (strong, nonatomic) DingRtcEngine *engine;
@property (strong, nonatomic) DingRtcEngineWhiteboard *whiteboardEngine;
@property (strong, nonatomic) UserManager *userManager;

@end

@implementation WhiteboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (IBAction)clickOpenWhiteboard:(id)sender {
    NSLog(@"click \"Open Whiteboard\" button with %ld", self.openWhiteboard.state);
    BOOL isOpen = (self.openWhiteboard.state == NSControlStateValueOn) ? YES : NO;
    if (isOpen) {
        [self openWhiteboardEngine];
    } else {
        [self closeWhiteboardEngine];
    }
}

- (IBAction)clickPathTool:(id)sender {
    NSLog(@"click \"Path Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        self.pathTool.state = NSControlStateValueOff;
        return;
    }
    BOOL isOn = (self.pathTool.state == NSControlStateValueOn) ? YES : NO;
    if (isOn) {
        [self.whiteboardEngine setToolType:DingRtcWbToolPath];
    } else {
        [self.whiteboardEngine setToolType:DingRtcWbToolSelect];
    }

    // Reset text tool
    self.textTool.state = NSControlStateValueOff;
}

- (IBAction)clickTextTool:(id)sender {
    NSLog(@"click \"Text Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        self.textTool.state = NSControlStateValueOff;
        return;
    }
    BOOL isOn = (self.textTool.state == NSControlStateValueOn) ? YES : NO;
    if (isOn) {
        [self.whiteboardEngine setToolType:DingRtcWbToolText];
    } else {
        [self.whiteboardEngine setToolType:DingRtcWbToolSelect];
    }

    // Reset path tool
    self.pathTool.state = NSControlStateValueOff;
}

- (IBAction)clickClearAll:(id)sender {
    NSLog(@"click \"Text Tool\" button");
    if (!self.whiteboardEngine) {
        [self displayMessage:@"Whiteboard not Opened!"];
        return;
    }
    DingRtcWbClearParam *param = [DingRtcWbClearParam new];
    param.mode = DingRtcWbClearModeAll;
    [self.whiteboardEngine clearContents:param];
}

- (IBAction)leaveChannel:(id)sender {
    NSLog(@"click \"Leave Channel\" button");
    [self closeWhiteboardEngine];
    [self leaveChannel];
    [self destroyEngineKit];
    [self.view.window performClose:nil];
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
    
    NSString *userName = [@"Mac_" stringByAppendingString:ChannelInfo.userId];
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
        config.width = 800;
        config.height = 450;
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
    dispatch_async(dispatch_get_main_queue(), ^{
        NSLog(@"%@", message);
        self.message.stringValue = message;
        self.message.hidden = NO;
    });
}

//MARK: - DingRtcEngineDelegate

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
