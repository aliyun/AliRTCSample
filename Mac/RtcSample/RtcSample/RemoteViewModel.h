//
//  RemoteViewModel.h
//  RtcSample
//
//  Created by 舒毅文 on 2019/3/22.
//  Copyright © 2019 mt. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AliRTCSdk/AliRtcEngine.h>

NS_ASSUME_NONNULL_BEGIN

@interface RemoteViewModel : NSObject

@property (nonatomic, copy) NSString *uid;
@property (nonatomic, assign) AliRtcVideoTrack videoTrack;
@property (nonatomic, strong) AliVideoCanvas *remoteCanvas;

@end

NS_ASSUME_NONNULL_END
