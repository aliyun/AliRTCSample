//
//  RemoteUserCollectionViewItem.m
//  RtcSample
//
//  Created by daijian on 2019/4/8.
//  Copyright © 2019年 mt. All rights reserved.
//

#import "RemoteUserCollectionViewItem.h"

@interface RemoteUserCollectionViewItem ()
@property (nonatomic,assign)BOOL isClick;
@end

@implementation RemoteUserCollectionViewItem

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    self.view.wantsLayer = true;
    
}


- (IBAction)mirrorButtonClick:(id)sender {
    if (self.cameraMirrorBlock) {
        self.cameraMirrorBlock(!self.isClick);
    }
    self.isClick = !self.isClick;
}


@end
