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

@end


@interface UserManager : NSObject

- (UserInfo *)addUser:(NSString *)userId withName:(NSString *)userName;
- (UserInfo *)removeUser:(NSString *)userId;
- (UserInfo *)findUser:(NSString *)userId;

@end

NS_ASSUME_NONNULL_END
