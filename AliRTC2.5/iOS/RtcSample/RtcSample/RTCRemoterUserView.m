//
//  RTCRemoterUserView.m
//  RtcSample
//
//  Created by ToT on 2021/5/21.
//  Copyright © 2021 tiantian. All rights reserved.
//

#import "RTCRemoterUserView.h"

@implementation RTCRemoterUserView
{
    UIView *viewRemote;
}

- (instancetype)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
        //设置远端流界面
        CGRect rc  = CGRectMake(0, 0, 140, 200);
        viewRemote = [[UIView alloc] initWithFrame:rc];
        self.backgroundColor = [UIColor clearColor];
        
        CGRect viewrc  = CGRectMake(0, 200, 140, 40);
        _view = [[UIView alloc] initWithFrame:viewrc];
        _view.backgroundColor = [UIColor darkGrayColor];
        [self addSubview:self.view];
        
        rc.origin.x = 8;
        rc.size = CGSizeMake(70, 40);
        _CameraMirrorLabel = [[UILabel alloc] initWithFrame:rc];
        self.CameraMirrorLabel.text = @"视频镜像";  //默认
        self.CameraMirrorLabel.textAlignment = 0;
        self.CameraMirrorLabel.font = [UIFont systemFontOfSize:17];
        self.CameraMirrorLabel.textColor = [UIColor whiteColor];
        self.CameraMirrorLabel.textAlignment = NSTextAlignmentLeft;
        [self.view addSubview:self.CameraMirrorLabel];
        
        rc.origin.x = 81;
        rc.origin.y = 4.5;
        rc.size = CGSizeMake(51, 31);
        _CameraMirrorSwitch = [[UISwitch alloc] initWithFrame:rc];
        _CameraMirrorSwitch.transform = CGAffineTransformMakeScale(0.8,0.8);
        [self.CameraMirrorSwitch addTarget:self action:@selector(onCameraMirrorClicked:)  forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:self.CameraMirrorSwitch];
    }
    return self;
}

- (void)updateUserRenderview:(UIView *)view {
    view.backgroundColor = [UIColor clearColor];
    view.frame = viewRemote.frame;
    viewRemote = view;
    [self addSubview:viewRemote];
}

- (void)onCameraMirrorClicked:(UISwitch *)switchView{
    if (self.switchblock) {
        self.switchblock(switchView.on);
    }
}

@end
