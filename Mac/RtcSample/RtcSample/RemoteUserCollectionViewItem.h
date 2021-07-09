//
//  RemoteUserCollectionViewItem.h
//  RtcSample
//
//  Created by daijian on 2019/4/8.
//  Copyright © 2019年 mt. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface RemoteUserCollectionViewItem : NSCollectionViewItem

/**
 @brief 镜像点击事件回调
 */
@property(nonatomic,copy) void(^cameraMirrorBlock)(BOOL);

@property (weak) IBOutlet NSButtonCell *cameraMirrorButton;
@end

NS_ASSUME_NONNULL_END
