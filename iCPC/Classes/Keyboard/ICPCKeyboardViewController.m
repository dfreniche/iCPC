//
//  ICPCKeyboardViewController.m
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#import "ICPCKeyboardViewController.h"
#import "ICPCKeyboardViewController+ExternalKeyboard.h"
#import "plateform.h"
#import "KeyReaderTextField.h"

@interface ICPCKeyboardViewController () <ICPCExternalKeyboardSupportable, KeyReaderTextFieldDelegate>
@property (weak, nonatomic) IBOutlet KeyReaderTextField *hiddenTextField;

@end

@implementation ICPCKeyboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.hiddenTextField.delegate = self;
    self.hiddenTextField.keyReaderDelegate = self;
    
    [self.hiddenTextField becomeFirstResponder];
}

- (void)focus {
    
    [self.hiddenTextField becomeFirstResponder];
}

- (BOOL)textField:(UITextField *)textField
shouldChangeCharactersInRange:(NSRange)range
replacementString:(NSString *)string {

    NSDictionary *keyMap = @{
                             @"º": @(15), // CPC_ESC
                             
                             @"1": @(16), // CPC_1
                             @"2": @(17), // CPC_2
                             @"3": @(18), // CPC_3
                             @"4": @(19), // CPC_4
                             @"5": @(20), // CPC_5
                             @"6": @(21), // CPC_6
                             @"7": @(22), // CPC_7
                             @"8": @(23), // CPC_8
                             @"9": @(24), // CPC_9
                             @"0": @(25), // CPC_ZERO
                             
                             @"_": @(1),    // CPC_F1
                             
                             @"q": @(31),   // CPC_Q
                             @"w": @(32),   // CPC_W
                             @"e": @(33),   // CPC_E
                             @"r": @(34),   // CPC_R
                             @"t": @(35),   // CPC_T
                             @"y": @(36),   // CPC_Y
                             @"u": @(37),   // CPC_U
                             @"i": @(38),   // CPC_I
                             @"o": @(39),   // CPC_O
                             @"p": @(40),   // CPC_P
                             
                             @"a": @(45),   // CPC_A
                             @"s": @(46),   // CPC_S
                             @"d": @(47),   // CPC_D
                             @"f": @(48),   // CPC_F
                             @"g": @(49),   // CPC_G
                             @"h": @(50),   // CPC_H
                             @"j": @(51),   // CPC_J
                             @"k": @(52),   // CPC_K
                             @"l": @(53),   // CPC_L

                             @"z": @(58),   // CPC_Z
                             @"x": @(59),   // CPC_X
                             @"c": @(60),   // CPC_C
                             @"v": @(61),   // CPC_V
                             @"b": @(62),   // CPC_B
                             @"n": @(63),   // CPC_N
                             @"m": @(64),   // CPC_M
                             

                             
                             @" ": @(72),   // CPC_SPACE
                             @"_": @(1),    // CPC_F1
                             
                             };
    
    
    if ([@"\n" isEqualToString:string]) {
        PressKey(43); // CPC_RETURN
        return NO;
    }
    
    CPC_KEY key = (CPC_KEY)((NSNumber *)keyMap[string]).intValue;

    NSLog(@"iOS key %@, keymap: %@ %d", string, keyMap[string], key);
    
    if (key != 0) {
        PressKey(key);
    }
    
    return NO;
}

- (void)textFieldDidDelete {
    PressKey(29);
}

// External (BT) keyboard support
- (void)externalKeyPressed:(UIKeyCommand *)key {
    NSLog(@"Key pressed %@", key.input);
    
    if ([key.input isEqualToString:UIKeyInputLeftArrow]) {
        PressKey(12);
//        [self keypadKeyPressed:KEY_LEFT];
    }
    else if ([key.input isEqualToString:UIKeyInputRightArrow]) {
        PressKey(14);
//        [self keypadKeyPressed:KEY_RIGHT];
    }
    else if ([key.input isEqualToString:UIKeyInputUpArrow]) {
        PressKey(11);
//        [self keypadKeyPressed:KEY_UP];
    }
    else if ([key.input isEqualToString:UIKeyInputDownArrow]) {
        PressKey(13);
//        [self keypadKeyPressed:KEY_DOWN];
    }
    
}

- (void)hideSystemKeyboard {
    
    [self.hiddenTextField resignFirstResponder];
//    [self.hiddenTextField setHidden:YES];
}

- (void)didMoveToParentViewController:(UIViewController *)parent {
    
    [self hideSystemKeyboard];
}


@end
