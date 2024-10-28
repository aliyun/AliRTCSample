//
//  ChannelInfo.m
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "ChannelInfo.h"

/* Please refer to Glossary to understand the meaning of App ID, Channel ID, Token, User ID, and User Name:
   请参考 名词解释 了解 App ID、Channel ID、Token、User ID、User Name 的含义：
   https://help.aliyun.com/document_detail/159037.html

   You can use temporary token for temporary testing:
   可以使用 临时token 来进行临时测试：
   https://help.aliyun.com/document_detail/146833.html
*/
static NSString * _appId = %% Your App ID %%;
static NSString * _token = %% Your Token %%;
static NSString * _gslbServer = @"https://gslb.dingrtc.com";
static NSString * _channelId = @"";
static NSString * _userId = @"";

@implementation ChannelInfo

+ (NSString *)appId {
    return _appId;
}

+ (NSString *)gslbServer {
    return _gslbServer;
}

+ (NSString *)token {
    return _token;
}

+ (NSString *)channelId {
    return _channelId;
}

+ (NSString *)userId {
    return _userId;
}

+ (void)setUserId:(NSString *)userId{
    _userId = userId;
}

+ (void)setChannelId:(NSString *)channelId {
    _channelId = channelId;
}

@end
