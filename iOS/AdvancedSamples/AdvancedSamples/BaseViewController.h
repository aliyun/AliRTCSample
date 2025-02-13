//
//  BaseViewController.h
//  AdvancedSamples
//
//  Created by mark on 2025/1/7.
//

#import <UIKit/UIKit.h>
#import "ChannelInfo.h"
#import "UserInfo.h"

NS_ASSUME_NONNULL_BEGIN

@interface BaseViewController : UIViewController <DingRtcEngineDelegate>

@property (strong, nonatomic, nullable) DingRtcEngine *engine;
@property (strong, nonatomic) UserManager *userManager;

- (void)channelJoinSuccess;
- (void)channelJoinFailed;
- (void)leaveChannel;
- (void)destroyEngineKit;
- (void)displayMessage:(NSString *)message;

@end

NS_ASSUME_NONNULL_END
