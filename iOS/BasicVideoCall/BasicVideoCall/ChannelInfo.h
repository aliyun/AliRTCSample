//
//  ChannelInfo.h
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DingRtc/DingRtcEngine.h"

NS_ASSUME_NONNULL_BEGIN

@interface ChannelInfo : NSObject

+ (NSString *)appId;
+ (NSString *)gslbServer;
+ (NSString *)token;
+ (NSString *)channelId;
+ (NSString *)userId;

+ (void)setChannelId:(NSString *)channelId;
+ (void)setUserId:(NSString *)userId;

@end

NS_ASSUME_NONNULL_END
