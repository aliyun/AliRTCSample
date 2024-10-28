//
//  UserInfo.m
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "UserInfo.h"

@implementation UserInfo

- (instancetype)initWithId:(NSString *)userId name:(NSString *)userName {
    self = [super init];
    if (nil != self) {
        _userId = userId;
        _userName = userName;
        _audioEnable = NO;
        _videoEnable = NO;
        _screenEnable = NO;
        _videoView = nil;
        _screenView = nil;
    }
    return self;
}

@end


@interface UserManager ()

@property (strong, nonatomic) NSMutableArray<UserInfo *> * userInfos;

@end

@implementation UserManager

- (instancetype)init {
    self = [super init];
    if (nil != self) {
        self.userInfos = [NSMutableArray arrayWithCapacity:10];
    }
    return self;
}

- (UserInfo *)addUser:(NSString *)userId withName:(NSString *)userName {
    UserInfo * userInfo = [[UserInfo alloc] initWithId:userId name:userName];
    [self.userInfos addObject:userInfo];
    return userInfo;
}

- (UserInfo *)removeUser:(NSString *)userId {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if ([userInfo.userId isEqualToString:userId]) {
            [self.userInfos removeObject:userInfo];
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findUser:(NSString *)userId {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if ([userInfo.userId isEqualToString:userId]) {
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findUserWithView:(NSView *)view {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.videoView == view) {
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findWatingUser {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.videoEnable && userInfo.videoView == nil) {
            break;
        }
    }
    return userInfo;
}

@end
