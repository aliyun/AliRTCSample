//
//  RTCRemoterUserView.h
//  RtcSample
//
//  Created by ToT on 2021/5/21.
//  Copyright © 2021 tiantian. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface RTCRemoterUserView : UICollectionViewCell

/**
 @brief 用户流视图
 
 @param view renderview
 */
- (void)updateUserRenderview:(UIView *)view;

/**
 @brief Switch点击事件回调
 */
@property(nonatomic,copy) void(^switchblock)(BOOL);


/**
 @brief 灰色底View
 */
@property (nonatomic,strong) UIView *view;

/**
 @brief 视频(屏幕)镜像开关
 */
@property (nonatomic,strong) UISwitch *CameraMirrorSwitch;

/**
 @brief 视频(屏幕)镜像描述
 */
@property (nonatomic,strong) UILabel *CameraMirrorLabel;

- (void)onCameraMirrorClicked:(UISwitch *)switchView;

@end

NS_ASSUME_NONNULL_END
