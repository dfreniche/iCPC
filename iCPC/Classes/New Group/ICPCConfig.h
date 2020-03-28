//
//  ICPCConfig.h
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ICPCConfig : NSObject

+ (instancetype)sharedInstance;

- (instancetype)init __attribute__((unavailable("init not available, call sharedInstance instead")));

@property (nonatomic, assign) bool useIcade;
@property (nonatomic, assign) bool useFling;
@property (nonatomic, assign) bool useExternalKeyboard;
@property (nonatomic, assign) bool dskDemosAlreadyCopied;

- (void)setDskDemosAlreadyCopied:(bool)dskDemosAlreadyCopied;

@end

NS_ASSUME_NONNULL_END
