//
//  SampleHandler.m
//  ScreenSharingExt
//
//  Created by mark on 2021/4/21.
//


#import "SampleHandler.h"
#import <DingRtc/DingRtcScreenShareExt.h>

/**
 * 配置 App Group ID。
 * 如何获取 App Group ID？
 * 1. Xcode 工程中进入 Signing & Capabilities
 * 2. Capability 添加 App Groups
 * 3. 点击“+”新增自定义 App Group ID
 */
static NSString *kAppGroupId = <#NSString * Your App Group ID#>;

@interface SampleHandler () <DingScreenShareExtDelegate>

@end

@implementation SampleHandler

- (void)broadcastStartedWithSetupInfo:(NSDictionary<NSString *,NSObject *> *)setupInfo {
    // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    [[DingRtcScreenShareExt sharedInstance] setupWithAppGroup:kAppGroupId delegate:self];
}

- (void)broadcastPaused {
    // User has requested to pause the broadcast. Samples will stop being delivered.
}

- (void)broadcastResumed {
    // User has requested to resume the broadcast. Samples delivery will resume.
}

- (void)broadcastFinished {
    // User has requested to finish the broadcast.
}

- (void)processSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType {
    [[DingRtcScreenShareExt sharedInstance] sendSampleBuffer:sampleBuffer type:sampleBufferType];
}

//MARK: - DingScreenShareExtDelegate

- (void)finishBroadcastWithError:(DingRtcScreenShareExt *)broadcast error:(NSError *)error {
    [self finishBroadcastWithError:error];
}

@end
