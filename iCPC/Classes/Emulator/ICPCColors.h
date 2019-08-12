//
//  ICPCColors.h
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#ifndef ICPColors_h
#define ICPColors_h


#import <Foundation/Foundation.h>

#define RGB15(R,G,B) ((((R) & 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3));

NS_ASSUME_NONNULL_BEGIN

@interface ICPCColors : NSObject

- (int) cpcColor:(int)color;
- (void) setPalette:(int)color;

@end

NS_ASSUME_NONNULL_END

#endif
