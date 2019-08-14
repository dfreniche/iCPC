//
//  ICPCScreen.h
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//


#ifndef ICPCScreen_h
#define ICPCScreen_h

#import <Foundation/Foundation.h>
#import "ICPCColors.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM( NSUInteger, MonitorType ) {
    ColorMonitor,
    GreenMonitor
};

@interface ICPCScreen : NSObject

@property (nonatomic) int borderX;
@property (nonatomic) int borderY;


/**
 Sets which kind of monitor (Green, Color) are we using

 @param type MonitorType
 */
- (void) setMonitorType:(MonitorType)type;
- (void) initBorder:(int)borderX borderY:(int)borderY;

@end

NS_ASSUME_NONNULL_END

#endif

