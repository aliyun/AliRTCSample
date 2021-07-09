//
//  RTCSampleRemoteUserModel.h
//  RtcSample
//
//  Created by daijian on 2019/4/11.
//  Copyright © 2019年 tiantian. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AliRTCSdk/AliRTCSdk.h>


NS_ASSUME_NONNULL_BEGIN

@interface RTCSampleRemoteUserModel : NSObject

@property (nonatomic, strong) UIView *view;

@property (nonatomic, strong) NSString *uid;

@property (nonatomic, assign) AliRtcVideoTrack track;

@end

NS_ASSUME_NONNULL_END
