//
//  UIViewController+RTCSampleAlert.m
//  RtcSample
//
//  Created by daijian on 2019/2/28.
//  Copyright © 2019年 tiantian. All rights reserved.
//

#import "UIViewController+RTCSampleAlert.h"

@implementation UIViewController (RTCSampleAlert)

- (void)showAlertWithMessage:(NSString *)message handler:(nonnull void (^)(UIAlertAction * _Nonnull))handler{
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"提示"
                                                                             message:message
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *confirm = [UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        if (handler) {
            handler(action);
        }
    }];
    [alertController addAction:confirm];
    [self.navigationController presentViewController:alertController animated:YES completion:nil];
}

@end
