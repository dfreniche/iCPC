//
//  KeyReaderTextField.h
//  iCPC
//
//  Created by Diego Freniche Brito on 07/04/2019.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol KeyReaderTextFieldDelegate <NSObject>
@optional
- (void)textFieldDidDelete;
@end

@interface KeyReaderTextField : UITextField<UIKeyInput>

@property (nonatomic, assign) id<KeyReaderTextFieldDelegate> keyReaderDelegate;

@end

NS_ASSUME_NONNULL_END
