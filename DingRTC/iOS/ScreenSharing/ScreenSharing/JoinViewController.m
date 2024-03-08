//
//  JoinViewController.m
//  BasicVideoCall
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "JoinViewController.h"
#import "ChannelInfo.h"

@interface JoinViewController () <UITextFieldDelegate>

@property (strong, nonatomic) IBOutlet UITextField * channelId;
@property (strong, nonatomic) IBOutlet UITextField * userId;

@end

@implementation JoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _channelId.delegate = self;
    _userId.delegate = self;
    _channelId.text = ChannelInfo.channelId;
    _userId.text = ChannelInfo.userId;
}

- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(nullable id)sender {
    [self.view endEditing:YES];
    return _channelId.text.length > 0 ? YES : NO;
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    if (textField == _channelId) {
        [ChannelInfo setChannelId:_channelId.text];
    } else if (textField == _userId) {
        [ChannelInfo setUserId:_userId.text];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
}

@end
