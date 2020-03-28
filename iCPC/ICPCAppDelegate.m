//
//  ICPCAppDelegate.m
//  iCPC
//
//  Copyright 2019 Diego Freniche All rights reserved.
//


#import "ICPCAppDelegate.h"
#import "ICPCFileManager.h"

@implementation ICPCAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    // We start the using iCPC.storyboard as Main Interface
    
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:nil];
    
    // on startup we copy all bundled demo DSK files to the app doc directory
    [ICPCFileManager copyAllDSKFilesToDocumentDirectory];
    
    return YES;
}

- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation {
    
 /*
    if ([_fileManager.ksync handleOpenURL:url]) {
        return YES;
    }
  */
    
    // Get the filename
    NSString *filename = [url lastPathComponent];
    
    // Get the full path of where we're going to move the file
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *path = [documentsDirectory stringByAppendingPathComponent:filename];
    
    NSURL *newUrl = [NSURL fileURLWithPath:path];
    
    // Move input file into documents directory
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager removeItemAtURL:newUrl error:nil];
    [fileManager moveItemAtURL:url toURL:newUrl error:nil];
    [fileManager removeItemAtPath:[documentsDirectory stringByAppendingPathComponent:@"Inbox"] error:nil];
    
    return YES;
}

@end
