//
//  RTCSampleChatViewController.m
//  RtcSample
//
//  Created by daijian on 2019/2/27.
//  Copyright © 2019年 tiantian. All rights reserved.
//

#import "RTCSampleChatViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "UIViewController+RTCSampleAlert.h"
#import "RTCSampleRemoteUserManager.h"
#import "RTCSampleRemoteUserModel.h"
#import "RTCRemoterUserView.h"

@interface RTCSampleChatViewController ()<AliRtcEngineDelegate,UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>


/**
 @brief SDK实例
 */
@property (nonatomic, strong) AliRtcEngine *engine;


/**
 @brief 远端用户管理
 */
@property(nonatomic, strong) RTCSampleRemoteUserManager *remoteUserManager;


/**
 @brief 远端用户视图
 */
@property(nonatomic, strong) UICollectionView *remoteUserView;


/**
 @brief 是否入会
 */
@property(nonatomic, assign) BOOL isJoinChannel;

@end

@implementation RTCSampleChatViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //导航栏名称等基本设置
    [self baseSetting];
    
    //初始化SDK内容
    [self initializeSDK];
    
    //开启本地预览
    [self startPreview];
    
    //加入房间
    [self joinBegin];
    
    //添加页面控件
    [self addSubviews];
    
}


#pragma mark - baseSetting
/**
 @brief 基础设置
 */
- (void)baseSetting{
    self.title = @"视频通话";
    self.view.backgroundColor = [UIColor whiteColor];
}

#pragma mark - initializeSDK
/**
 @brief 初始化SDK
 */
- (void)initializeSDK{
    // 创建SDK实例，注册delegate，extras可以为空
    _engine = [AliRtcEngine sharedInstance:self extras:@""];
    
}

- (void)startPreview{
    // 设置本地预览视频
    AliVideoCanvas *canvas   = [[AliVideoCanvas alloc] init];
    UIView *viewLocal = [[UIView alloc] initWithFrame:self.view.bounds];
    canvas.view = viewLocal;
    canvas.renderMode = AliRtcRenderModeAuto;
    [self.view addSubview:viewLocal];
    [self.engine setLocalViewConfig:canvas forTrack:AliRtcVideoTrackCamera];
    
    // 开启本地预览
    [self.engine startPreview];
}

#pragma mark - action (需手动填写鉴权信息)

/**
 @brief 登陆服务器，并开始推流
 */
- (void)joinBegin{
    //token鉴权信息参见：https://help.aliyun.com/document_detail/159037.html?spm=a2c4g.11186623.6.629.6392466bWKmZNv
    //AliRtcAuthInfo 配置项
    NSString *AppID   =  @"";
    NSString *userID  =  @"";
    NSString *channelID  =  @"";
    NSString *nonce  =  @"";
    long long timestamp = 0;
    NSString *token  =  @"";
    NSArray <NSString *> *GSLB  =  @[@""];
    NSArray <NSString *> *agent =  @[@""];
    
    
    //配置SDK
    
    //随机生成用户名，仅是demo展示使用
    NSString *userName = [NSString stringWithFormat:@"iOSUser%u",arc4random()%1234];
    
    //AliRtcAuthInfo:各项参数均需要客户App Server(客户的server端) 通过OpenAPI来获取，然后App Server下发至客户端，客户端将各项参数赋值后，即可joinChannel
    AliRtcAuthInfo *authInfo = [[AliRtcAuthInfo alloc] init];
    authInfo.appId = AppID;
    authInfo.userId = userID;
    authInfo.channelId = channelID;
    authInfo.nonce = nonce;
    authInfo.timestamp = timestamp;
    authInfo.token = token;
    authInfo.gslb = GSLB;
    authInfo.agent = agent;
    
    //加入频道
    [self.engine joinChannel:authInfo name:userName onResult:^(NSInteger errCode, NSString * _Nonnull channel, NSInteger elapsed) {
        //加入频道回调处理
        NSLog(@"joinChannel result: %d", (int)errCode);
        dispatch_async(dispatch_get_main_queue(), ^{
            if (errCode != 0) {
                //入会失败
            }
            _isJoinChannel = YES;
        });
    }];
    
    //防止屏幕锁定
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

/**
 @brief 离开频道
 */
- (void)leaveChannel:(UIButton *)sender {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self leaveChannel];  //退出房间
        [UIApplication sharedApplication].idleTimerDisabled = NO;
        [self exitApplication];  //关闭应用
        
    });
}

#pragma mark - private

/**
 @brief 离开频需要取消本地预览、离开频道、销毁SDK
 */
- (void)leaveChannel {
    
    [self.remoteUserManager removeAllUser];
    
    //停止本地预览
    [self.engine stopPreview];
    
    if (_isJoinChannel) {
        //离开频道
        [self.engine leaveChannel];
    }

    [self.remoteUserView removeFromSuperview];
    
    //销毁SDK实例
    [AliRtcEngine destroy];
}

- (void)exitApplication{
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    [UIView animateWithDuration:0.5f animations:^{
        window.alpha = 0;
        window.frame = CGRectMake(window.bounds.size.width/2.0, window.bounds.size.width, 0, 0);
    } completion:^(BOOL finished) {
        exit(0);
    }];
}

#pragma mark - uicollectionview delegate & datasource

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return [self.remoteUserManager allOnlineUsers].count;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    RTCRemoterUserView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"cell" forIndexPath:indexPath];
    RTCSampleRemoteUserModel *model =  [self.remoteUserManager allOnlineUsers][indexPath.row];
    UIView *view = model.view;
    [cell updateUserRenderview:view];
    
    //记录UID
    NSString *uid = model.uid;
    //视频流类型
    AliRtcVideoTrack track = model.track;
    if (track == AliRtcVideoTrackScreen) {  //默认为视频镜像,如果是屏幕则替换屏幕镜像
        cell.CameraMirrorLabel.text = @"屏幕镜像";
    }
    
    cell.switchblock = ^(BOOL isOn) {
        [self switchClick:isOn track:track uid:uid];
    };
    
    return cell;
}

//远端用户镜像按钮点击事件
- (void)switchClick:(BOOL)isOn track:(AliRtcVideoTrack)track uid:(NSString *)uid {
    AliVideoCanvas *canvas = [[AliVideoCanvas alloc] init];
    canvas.renderMode = AliRtcRenderModeFill;
    if (track == AliRtcVideoTrackCamera) {
        canvas.view = (UIView *)[self.remoteUserManager cameraView:uid];
    }
    else if (track == AliRtcVideoTrackScreen) {
        canvas.view = (UIView *)[self.remoteUserManager screenView:uid];
    }
    
    if (isOn) {
        canvas.mirrorMode = AliRtcRenderMirrorModeAllEnabled;
    }else{
        canvas.mirrorMode = AliRtcRenderMirrorModeAllDisabled;
    }
    [self.engine setRemoteViewConfig:canvas uid:uid forTrack:track];
}

#pragma mark - alirtcengine delegate
- (void)onRemoteTrackAvailableNotify:(NSString *_Nonnull)uid audioTrack:(AliRtcAudioTrack)audioTrack videoTrack:(AliRtcVideoTrack)videoTrack {
    
    //收到远端订阅回调
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.remoteUserManager updateRemoteUser:uid forTrack:videoTrack];
        if (videoTrack == AliRtcVideoTrackCamera) {
            AliVideoCanvas *canvas = [[AliVideoCanvas alloc] init];
            canvas.renderMode = AliRtcRenderModeAuto;
            canvas.view = [self.remoteUserManager cameraView:uid];
            [self.engine setRemoteViewConfig:canvas uid:uid forTrack:AliRtcVideoTrackCamera];
        }else if (videoTrack == AliRtcVideoTrackScreen) {
            AliVideoCanvas *canvas2 = [[AliVideoCanvas alloc] init];
            canvas2.renderMode = AliRtcRenderModeAuto;
            canvas2.view = [self.remoteUserManager screenView:uid];
            [self.engine setRemoteViewConfig:canvas2 uid:uid forTrack:AliRtcVideoTrackScreen];
        }else if (videoTrack == AliRtcVideoTrackBoth) {
            
            AliVideoCanvas *canvas = [[AliVideoCanvas alloc] init];
            canvas.renderMode = AliRtcRenderModeAuto;
            canvas.view = [self.remoteUserManager cameraView:uid];
            [self.engine setRemoteViewConfig:canvas uid:uid forTrack:AliRtcVideoTrackCamera];
            
            AliVideoCanvas *canvas2 = [[AliVideoCanvas alloc] init];
            canvas2.renderMode = AliRtcRenderModeAuto;
            canvas2.view = [self.remoteUserManager screenView:uid];
            [self.engine setRemoteViewConfig:canvas2 uid:uid forTrack:AliRtcVideoTrackScreen];
        }
        [self.remoteUserView reloadData];
    });
}

- (void)onRemoteUserOnLineNotify:(NSString *)uid {
    
}

- (void)onRemoteUserOffLineNotify:(NSString *)uid {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.remoteUserManager remoteUserOffLine:uid];
        [self.remoteUserView reloadData];
    });
}

- (void)onOccurError:(int)error {
    if (error == AliRtcErrSessionRemoved) {
        // timeout - leaveChannel.
        [self showAlertWithMessage:@"Session已经被移除,请点击确定退出" handler:^(UIAlertAction * _Nonnull action) {
            [self leaveChannel:nil];
        }];
    }
    else if (error == AliRtcErrIceConnectionHeartbeatTimeout) {
        [self showAlertWithMessage:@"信令心跳超时，请点击确定退出" handler:^(UIAlertAction * _Nonnull action) {
            [self leaveChannel:nil];
        }];
    }
    else if (error == AliRtcErrJoinBadAppId) {
        [self showAlertWithMessage:@"AppId不存在，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinInvalidAppId) {
        [self showAlertWithMessage:@"AppId已失效，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinBadChannel) {
        [self showAlertWithMessage:@"频道不存在，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinInvalidChannel) {
        [self showAlertWithMessage:@"频道已失效，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinBadToken) {
        [self showAlertWithMessage:@"token不存在，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinTimeout) {
        [self showAlertWithMessage:@"加入频道超时，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrJoinBadParam) {
        [self showAlertWithMessage:@"参数错误，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrMicOpenFail) {
        [self showAlertWithMessage:@"采集设备初始化失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrSpeakerOpenFail) {
        [self showAlertWithMessage:@"播放设备初始化失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrMicInterrupt) {
        [self showAlertWithMessage:@"采集过程中出现异常，请重新join(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrSpeakerInterrupt) {
        [self showAlertWithMessage:@"播放过程中出现异常，请重新join(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrMicAuthFail) {
        [self showAlertWithMessage:@"麦克风设备未授权，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrMicNotAvailable) {
        [self showAlertWithMessage:@"无可用的音频采集设备，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrSpeakerNotAvailable) {
        [self showAlertWithMessage:@"无可用的音频播放设备，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrCameraOpenFail) {
        [self showAlertWithMessage:@"采集设备初始化失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrCameraInterrupt) {
        [self showAlertWithMessage:@"采集过程中出现异常，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrDisplayOpenFail) {
        [self showAlertWithMessage:@"渲染设备初始化失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrDisplayInterrupt) {
        [self showAlertWithMessage:@"渲染过程中出现异常，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrIceConnectionConnectFail) {
        [self showAlertWithMessage:@"媒体通道建立失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrIceConnectionReconnectFail) {
        [self showAlertWithMessage:@"媒体通道重连失败，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrSdkInvalidState) {
        [self showAlertWithMessage:@"sdk状态错误，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
    else if (error == AliRtcErrInner) {
        [self showAlertWithMessage:@"其他错误，请重新pub、sub(仅提示)" handler:^(UIAlertAction * _Nonnull action) {
            
        }];
    }
}

- (void)onBye:(int)code {
    if (code == AliRtcOnByeChannelTerminated) {
        // channel结束
    }
}

#pragma mark - add subviews

- (void)addSubviews {
    
    UIButton *exitButton = [UIButton buttonWithType:UIButtonTypeCustom];
    exitButton.frame = CGRectMake(0, 0, 60, 40);
    [exitButton setTitle:@"退出" forState:UIControlStateNormal];
    [exitButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [exitButton addTarget:self action:@selector(leaveChannel:) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:exitButton];
    
    CGRect rcScreen = [UIScreen mainScreen].bounds;
    CGRect rc = rcScreen;
    rc.origin.x = 10;
    rc.origin.y = [UIApplication sharedApplication].statusBarFrame.size.height+20+44;
    rc.size = CGSizeMake(self.view.frame.size.width-20, 280);
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc] init];
    flowLayout.itemSize = CGSizeMake(140, 280);
    flowLayout.minimumLineSpacing = 10;
    flowLayout.minimumInteritemSpacing = 10;
    flowLayout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    self.remoteUserView = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:flowLayout];
    self.remoteUserView.frame = rc;
    self.remoteUserView.backgroundColor = [UIColor clearColor];
    self.remoteUserView.delegate   = self;
    self.remoteUserView.dataSource = self;
    self.remoteUserView.showsHorizontalScrollIndicator = NO;
    [self.remoteUserView registerClass:[RTCRemoterUserView class] forCellWithReuseIdentifier:@"cell"];
    [self.view addSubview:self.remoteUserView];
    
    _remoteUserManager = [RTCSampleRemoteUserManager shareManager];
    
}

@end
