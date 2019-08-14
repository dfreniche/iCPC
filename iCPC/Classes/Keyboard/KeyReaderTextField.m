//
//  KeyReaderTextField.m
//  iCPC
//
//  Created by Diego Freniche Brito on 07/04/2019.
//

#import "KeyReaderTextField.h"

@implementation KeyReaderTextField

- (void)deleteBackward {
    [super deleteBackward];
    
    if ([_keyReaderDelegate respondsToSelector:@selector(textFieldDidDelete)]){
        [_keyReaderDelegate textFieldDidDelete];
    }
}

@end
