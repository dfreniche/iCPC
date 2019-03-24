//
//  ICPCMainViewController+ExternalKeyboard.h
//  iCPC
//
//  Created by Diego Freniche Brito on 22/02/2019.
//

#import "ICPCKeyboardViewController.h"

NS_ASSUME_NONNULL_BEGIN

@protocol ICPCExternalKeyboardSupportable <NSObject>

- (void)externalKeyPressed:(UIKeyCommand *)key;

@end

@interface ICPCKeyboardViewController (ExternalKeyboard)
    
@end

NS_ASSUME_NONNULL_END
