//
//  UIViewController+RTCSampleAlert.h
//  RtcSample
//
//  Created by daijian on 2019/2/28.
//  Copyright © 2019年 tiantian. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (RTCSampleAlert)


/**
 @brief 提示框提醒

 @param message 提示信息
 */
- (void)showAlertWithMessage:(NSString *)message handler:(void (^)(UIAlertAction *action))handler;

@end

NS_ASSUME_NONNULL_END
