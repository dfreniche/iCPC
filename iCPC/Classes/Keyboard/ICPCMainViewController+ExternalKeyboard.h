//
//  ICPCMainViewController+ExternalKeyboard.h
//  iCPC
//
//  Created by Diego Freniche Brito on 22/02/2019.
//

#import "ICPCMainViewController.h"

NS_ASSUME_NONNULL_BEGIN

@protocol ICPCExternalKeyboardSupportable <NSObject>

- (void)externalKeyPressed:(UIKeyCommand *)key;

@end

@interface ICPCMainViewController (ExternalKeyboard)
    
@end

NS_ASSUME_NONNULL_END
