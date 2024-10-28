//
//  UserInfo.h
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface UserInfo : NSObject

@property (copy, nonatomic, readonly) NSString * userId;
@property (copy, nonatomic, readonly) NSString * _Nullable userName;
@property (assign, nonatomic) BOOL audioEnable;
@property (assign, nonatomic) BOOL videoEnable;
@property (assign, nonatomic) BOOL screenEnable;
@property (strong, nonatomic) NSView * _Nullable videoView;
@property (strong, nonatomic) NSView * _Nullable screenView;

@end


@interface UserManager : NSObject

- (UserInfo *)addUser:(NSString *)userId withName:(NSString *)userName;
- (UserInfo *)removeUser:(NSString *)userId;
- (UserInfo *)findUser:(NSString *)userId;

- (UserInfo *)findUserWithView:(NSView *)view;
- (UserInfo *)findWatingUser;

@end

NS_ASSUME_NONNULL_END
