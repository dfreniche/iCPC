//
//  ICPCMainViewController+ExternalKeyboard.m
//  iCPC
//
//  Created by Diego Freniche Brito on 22/02/2019.
//

#import "ICPCKeyboardViewController+ExternalKeyboard.h"

@implementation ICPCKeyboardViewController (ExternalKeyboard)

// External (BT) keyboard support
- (NSArray<UIKeyCommand *> *)keyCommands {
    return @[
             [UIKeyCommand keyCommandWithInput:UIKeyInputUpArrow modifierFlags:0 action:@selector(externalKeyPressed:) discoverabilityTitle:@"Cursor Up"
              ],
             [UIKeyCommand keyCommandWithInput:UIKeyInputDownArrow modifierFlags:0 action:@selector(externalKeyPressed:) discoverabilityTitle:@"Cursor Down"
              ],
             [UIKeyCommand keyCommandWithInput:UIKeyInputLeftArrow modifierFlags:0 action:@selector(externalKeyPressed:) discoverabilityTitle:@"Cursor Left"
              ],
             [UIKeyCommand keyCommandWithInput:UIKeyInputRightArrow modifierFlags:0 action:@selector(externalKeyPressed:) discoverabilityTitle:@"Cursor Right"
              ],
             [UIKeyCommand keyCommandWithInput:@"q" modifierFlags:0 action:@selector(externalKeyPressed:) discoverabilityTitle:@"Help"
              ]
             ];
}
             
@end
