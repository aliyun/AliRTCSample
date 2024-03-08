//
//  UserInfo.h
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UserInfo : NSObject

@property (copy, nonatomic, readonly) NSString * userId;
@property (copy, nonatomic, readonly) NSString * _Nullable userName;
@property (assign, nonatomic) BOOL screenEnable;
@property (assign, nonatomic) BOOL screenMute;
@property (strong, nonatomic) UIView * _Nullable screenView;

@end


@interface UserManager : NSObject

- (UserInfo *)addUser:(NSString *)userId withName:(NSString *)userName;
- (UserInfo *)removeUser:(NSString *)userId;
- (UserInfo *)findUser:(NSString *)userId;

- (UserInfo *)findUserWithView:(UIView *)view;
- (UserInfo *)findWatingUser;

@end

NS_ASSUME_NONNULL_END
