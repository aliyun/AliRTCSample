//
//  VideoFrameObserverViewController.m
//  AdvancedSamples
//
//  Created by mark on 2025/1/7.
//

#import "VideoFrameObserverViewController.h"

@interface DingRtcVideoFrameTask : NSObject{
    FILE * _inputFile;
}

@property (strong, nonatomic) NSString *channelId;
@property (strong, nonatomic) NSString *uid;
@property (assign, nonatomic) DingRtcVideoTrack track;
@property (assign, nonatomic) int type;
@property (strong, nonatomic) NSDateFormatter *dateFormatter;
@property (strong, nonatomic) NSString *dumpPath;
@property (strong, nonatomic) NSString *filePath;
@property (strong, nonatomic) dispatch_queue_t fileQueue;

@property (assign, nonatomic) BOOL isLandscape;
@property (assign, nonatomic) int outWidth;
@property (assign, nonatomic) int outHeight;

@end

@implementation DingRtcVideoFrameTask

- (instancetype)initWithChannelId:(NSString *)channelId uid:(NSString *)uid track:(DingRtcVideoTrack)track type:(int)type {
    self = [super init];
    if (self) {
        [self setupWithWithChannelId:channelId uid:uid track:track type:type];
        
        NSString *rootDir = @"DingRtc";
        NSString *rootPath = [self getRootPathWithSandbox:rootDir];
        _dumpPath = [NSString stringWithFormat:@"%@/%@", rootPath, @"VideoFrameObserver"];
        [self createDirectoryAtPath:self.dumpPath];
    }
    return self;
}

- (void)setupWithWithChannelId:(NSString *)channelId uid:(NSString *)uid track:(DingRtcVideoTrack)track type:(int)type {
    self.channelId = channelId;
    self.uid = uid;
    self.track = track;
    self.type = type;
    self.fileQueue = dispatch_queue_create([NSString stringWithFormat:@"saveVideo_%ld", (long)type].UTF8String, DISPATCH_QUEUE_SERIAL);
    self.filePath = nil;
    
    self.isLandscape = NO;
    self.outWidth = 0;
    self.outHeight = 0;
    
    NSString *msg = [NSString stringWithFormat:@"setup with uid:%@, type:%ld, track:%lu", uid, (long)self.type, (unsigned long)track];
    NSLog(@"%@",msg);
}

#pragma mark - Utils

- (NSString *)getRootPathWithSandbox:(NSString *)rootDir {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *rootPath = nil;
    if (paths.count) {
        NSString *documentPath = [paths objectAtIndex:0];
        rootPath = [documentPath stringByAppendingPathComponent:rootDir];
    }
    return rootPath;
}

- (BOOL)createDirectoryAtPath:(NSString *)absolutePath {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:absolutePath]) {  //如果没有路径则创建
        NSError *error = nil;
        [fileManager createDirectoryAtPath:absolutePath withIntermediateDirectories:YES attributes:nil error:&error];
        if (error) {
            return NO;
        }
    }
    return YES;
}

//保存视频文件
- (void)saveWithVideoFrame:(DingRtcVideoFrame *)videoFrame {
    //串行队列
    dispatch_sync(self.fileQueue, ^{
        bool isLandscape = (videoFrame.rotation == 90 || videoFrame.rotation == 270);
        int outWidth = isLandscape ? videoFrame.height : videoFrame.width;
        int outHeight = isLandscape ? videoFrame.width : videoFrame.height;
        
        if (isLandscape != self.isLandscape ||
            outWidth != self.outWidth ||
            outHeight != self.outHeight) {
            
            self.isLandscape = isLandscape;
            self.outWidth = outWidth;
            self.outHeight = outHeight;
            
            if (_inputFile) {
                fclose(_inputFile);
                _inputFile = NULL;
            }
            
            NSString *keyString = [self getFileName:self.channelId uid:self.uid track:self.track type:self.type width:self.outWidth height:self.outHeight];
            self.filePath = [NSString stringWithFormat:@"%@/%@", self.dumpPath, keyString];
            _inputFile = fopen([self.filePath UTF8String], "ab+");
            
            NSString *msg = [NSString stringWithFormat:@"start save type:%ld, filePath: %@", (long)self.type, self.filePath];
            NSLog(@"%@",msg);
        }
        
        if (videoFrame.data) {
            if (videoFrame.stride.count > 1) {
                size_t yPlaneSize = outWidth * outHeight;
                size_t uvPlaneSize = (outWidth / 2) * (outHeight / 2);
                unsigned char *rotatedYPlane = malloc(yPlaneSize);
                unsigned char *rotatedUPlane = malloc(uvPlaneSize);
                unsigned char *rotatedVPlane = malloc(uvPlaneSize);
                
                const unsigned char *srcY = videoFrame.data + videoFrame.offset[0].intValue;
                const unsigned char *srcU = videoFrame.data + videoFrame.offset[1].intValue;
                const unsigned char *srcV = videoFrame.data + videoFrame.offset[2].intValue;
                
                [DingRtcEngine I420RotateWithSrcY:srcY srcStrideY:videoFrame.stride[0].intValue
                                             srcU:srcU srcStrideU:videoFrame.stride[1].intValue
                                             srcV:srcV srcStrideV:videoFrame.stride[1].intValue
                                             dstY:rotatedYPlane dstStrideY:outWidth
                                             dstU:rotatedUPlane dstStrideU:outWidth / 2
                                             dstV:rotatedVPlane dstStrideV:outWidth / 2
                                            width:videoFrame.width height:videoFrame.height mode:videoFrame.rotation];
                
                fwrite(rotatedYPlane, 1, yPlaneSize, _inputFile);
                fwrite(rotatedUPlane, 1, uvPlaneSize, _inputFile);
                fwrite(rotatedVPlane, 1, uvPlaneSize, _inputFile);
                
                free(rotatedYPlane);
                free(rotatedUPlane);
                free(rotatedVPlane);

            } else {
                NSString *msg = [NSString stringWithFormat:@"stride info is error, count:%ld, type:%ld, filePath: %@", (long)videoFrame.stride.count, (long)self.type, self.filePath];
                NSLog(@"%@",msg);
            }
        } else {
            NSString *msg = [NSString stringWithFormat:@"data is null type:%ld, filePath: %@", (long)self.type, self.filePath];
            NSLog(@"%@",msg);
        }
    });
}

- (void)stop {
    if (_inputFile) {
        fclose(_inputFile);
        _inputFile = NULL;
        NSString *msg = [NSString stringWithFormat:@"stop with type:%ld, filePath: %@", (long)self.type, self.filePath];
        NSLog(@"%@",msg);
    }
}

- (void)dealloc {
    NSString *msg = [NSString stringWithFormat:@"dealloc with type:%ld, filePath: %@", (long)self.type, self.filePath];
    NSLog(@"%@",msg);
}

/**
 * 获取文件的唯一key值，用于保存值，防止重复创建
 */
- (NSString *)getFileName:(NSString *)channnelId uid:(NSString *)uid track:(DingRtcVideoTrack)track type:(int)type width:(int)width height:(int)height {
    NSString *dateString = [self.dateFormatter stringFromDate:[NSDate date]];
    NSString *keyString = [NSString stringWithFormat:@"%@_uid_%@_type_%d_track_%ld_%dx%d_%@.yuv", channnelId, uid, type, track, width, height, dateString];
    return keyString;
}

- (NSDateFormatter *)dateFormatter {
    if (!_dateFormatter) {
        _dateFormatter = [[NSDateFormatter alloc] init];
        [_dateFormatter setFormatterBehavior:NSDateFormatterBehavior10_4];
        [_dateFormatter setDateFormat:@"HH-mm-ss-SSS"];
    }
    return _dateFormatter;
}

@end

@interface VideoFrameObserverViewController () <DingRtcVideoFrameDelegate>

@property (strong, nonatomic) NSMutableDictionary <NSString*, DingRtcVideoFrameTask *> *tasks;
@property (assign, nonatomic) DingRtcVideoObservePosition position;

@end

@implementation VideoFrameObserverViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.position = DingRtcVideoObservePositionPostCapture | DingRtcVideoObservePositionPreRender | DingRtcVideoObservePositionPreEncoder;
}

- (void)channelJoinSuccess {
    [self.engine publishLocalVideoStream:YES];
    [self.engine subscribeAllRemoteVideoStreams:YES];
}

- (IBAction)dumpVideo:(UIButton *)sender {
    sender.selected = !sender.selected;
    
    if (sender.selected) {
        [self startDump];
    } else {
        [self stopDump];
    }
}

- (IBAction)exitChannel:(UIButton *)sender {
    [self stopDump];
    [self leaveChannel];
    [self destroyEngineKit];
    UIApplication.sharedApplication.idleTimerDisabled = NO;
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)startDump {
    [self displayMessage:@"start video frame observer..."];
    [self.engine setVideoFrameDelegate:self];
    [self.engine enableVideoFrameObserver:YES position:self.position];
}

- (void)stopDump {
    [self displayMessage:@"stop video frame observer..."];
    [self.engine enableVideoFrameObserver:NO position:self.position];
    [self.engine setVideoFrameDelegate:nil];
    [self stopAllTask];
}

- (void)stopAllTask {
    if (self.tasks.count) {
        self.position = 0;
        [self.tasks.allValues enumerateObjectsUsingBlock:^(DingRtcVideoFrameTask * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [obj stop];
        }];
        [self.tasks removeAllObjects];
    }
}

- (DingRtcVideoFrameTask *)createUniqueIdWithUId:(NSString *)uid track:(DingRtcVideoTrack)track type:(int)type {
    DingRtcVideoFrameTask *task = [[DingRtcVideoFrameTask alloc] initWithChannelId:ChannelInfo.channelId uid:uid track:track type:type];
    NSString *uniqueId = [self getUniqueIdWithUId:uid track:track type:type];
    [self.tasks setObject:task forKey:uniqueId];
    return task;
}

- (NSString *)getUniqueIdWithUId:(NSString *)uid track:(DingRtcVideoTrack)track type:(int)type {
    NSString *uniqueId = [NSString stringWithFormat:@"%d_%@_%ld", type, uid, track];
    return uniqueId;
}

//MARK: - DingRtcVideoFrameDelegate

- (DingRtcVideoFormat)getVideoFormatPreference {
    return DingRtcVideoFormat_I420;
}

//1: 采集视频数据
- (BOOL)onCaptureVideoFrame:(DingRtcVideoFrame * _Nonnull)frame {
    NSString *userId = @"0";
    DingRtcVideoTrack track = DingRtcVideoTrackCamera;
    NSString *uniqueId = [self getUniqueIdWithUId:userId track:track type:DingRtcVideoObservePositionPostCapture];
    DingRtcVideoFrameTask *task = [self.tasks objectForKey:uniqueId];
    if (!task) {
        task = [self createUniqueIdWithUId:userId track:track type:DingRtcVideoObservePositionPostCapture];
    }
    [task saveWithVideoFrame:frame];
    return YES;
}

//2: 远端渲染视频数据，没有本端的
- (BOOL)onRemoteVideoFrame:(DingRtcVideoFrame * _Nonnull)frame userId:(NSString *_Nonnull)userId videoTrack:(DingRtcVideoTrack)videoTrack {
    NSString *uniqueId = [self getUniqueIdWithUId:userId track:videoTrack type:DingRtcVideoObservePositionPreRender];
    DingRtcVideoFrameTask *task = [self.tasks objectForKey:uniqueId];
    if (!task) {
        task = [self createUniqueIdWithUId:userId track:videoTrack type:DingRtcVideoObservePositionPreRender];
    }
    [task saveWithVideoFrame:frame];
    return YES;
}

//4: 本端编码前视频数据（其实就是本端的渲染数据）
- (BOOL)onPreEncodeVideoFrame:(DingRtcVideoFrame * _Nonnull)frame videoTrack:(DingRtcVideoTrack)videoTrack {
    NSString *userId = @"0";
    NSString *uniqueId = [self getUniqueIdWithUId:userId track:videoTrack type:DingRtcVideoObservePositionPreEncoder];
    DingRtcVideoFrameTask *task = [self.tasks objectForKey:uniqueId];
    if (!task) {
        task = [self createUniqueIdWithUId:userId track:videoTrack type:DingRtcVideoObservePositionPreEncoder];
    }
    [task saveWithVideoFrame:frame];
    return YES;
}

@end
