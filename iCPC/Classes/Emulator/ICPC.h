//
//  ICPC.h
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//  The main emulator file, emulates a CPC

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ICPC : NSObject


/**
 starts the emulator
 */
- (void)start;


/**
 stops the emulator and powers down the machine
 */
- (void)stop;


/**
 pauses emulation
 */
- (void)pause;


/**
 Resets emulator
 */
- (void)reset;

@end

NS_ASSUME_NONNULL_END
