//
//  ICPCScreen.m
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#import "ICPCScreen.h"

@interface ICPCScreen ()

@property (nonatomic, strong) ICPCColors *colors;

@end

@implementation ICPCScreen

- (void) setMonitorType:(MonitorType)type {
    switch (type) {
        case ColorMonitor:
            [self.colors setPalette:1];
            break;
        
        case GreenMonitor:
            [self.colors setPalette:0];
            break;
            
        default:
            break;
    }
}

- (void) initBorder:(int)borderX borderY:(int)borderY {

    self.borderX = borderX;
    self.borderY = borderY;
}


@end
