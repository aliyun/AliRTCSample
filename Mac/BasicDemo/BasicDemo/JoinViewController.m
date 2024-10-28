//
//  JoinViewController.m
//  BasicDemo
//
//  Copyright © 2024 DingTalk. All rights reserved.
//

#import "JoinViewController.h"
#import "ChannelInfo.h"

@interface JoinViewController () <NSWindowDelegate>

@property (weak) IBOutlet NSTextField *channelId;
@property (weak) IBOutlet NSTextField *userId;

@property (nonatomic, strong) NSWindowController *channelWindowController;

@end

@implementation JoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    self.channelId.stringValue = ChannelInfo.channelId;
    self.userId.stringValue = ChannelInfo.userId;
}

- (IBAction)clickJoinButton:(id)sender {
    NSLog(@"click \"Join Room\" button");
    if (self.channelId.stringValue.length == 0 || self.userId.stringValue.length == 0) {
        NSLog(@"Please input join channel id or user id!");
        return;
    }
    [ChannelInfo setChannelId:self.channelId.stringValue];
    [ChannelInfo setUserId:self.userId.stringValue];
    // create and show room window
    if (self.channelWindowController == nil) {
        NSStoryboard * mainStoryboard = [NSStoryboard storyboardWithName:@"Main" bundle:nil];
        self.channelWindowController = [mainStoryboard instantiateControllerWithIdentifier:@"ChannelWindow"];
        self.channelWindowController.window.delegate = self;
    }
    [self.channelWindowController showWindow:nil];
    [self.channelWindowController.window center];

    // hide join window
    [self.view.window orderOut:nil];
}

//MARK: - NSWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender {
    NSLog(@"[JoinViewController windowShouldClose], window = %@", sender);
    if (sender == self.channelWindowController.window) {
        self.channelWindowController = nil;
        
        // show join window
        [self.view.window makeKeyAndOrderFront:nil];
    }
    return YES;
}

@end
