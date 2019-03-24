//
//  ICPCKeyboardViewController.m
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#import "ICPCKeyboardViewController.h"
#import "ICPCKeyboardViewController+ExternalKeyboard.h"

@interface ICPCKeyboardViewController () <ICPCExternalKeyboardSupportable, UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITextField *hiddenTextField;

@end

@implementation ICPCKeyboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.hiddenTextField.delegate = self;
    [self.hiddenTextField becomeFirstResponder];
}

- (BOOL)textField:(UITextField *)textField
shouldChangeCharactersInRange:(NSRange)range
replacementString:(NSString *)string {

    
    NSData *asciiChar = [string dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    
    NSLog(@"key %@", string);
    
    PressKey([string characterAtIndex:0]);
    
    return NO;
    
}

- (void)externalKeyPressed:(UIKeyCommand *)key {
    NSLog(@"Key pressed %@", key.input);
    
    if ([key.input isEqualToString:UIKeyInputLeftArrow]) {
//        [self keypadKeyPressed:KEY_LEFT];
    }
    else if ([key.input isEqualToString:UIKeyInputRightArrow]) {
//        [self keypadKeyPressed:KEY_RIGHT];
    }
    else if ([key.input isEqualToString:UIKeyInputUpArrow]) {
//        [self keypadKeyPressed:KEY_UP];
    }
    else if ([key.input isEqualToString:UIKeyInputDownArrow]) {
//        [self keypadKeyPressed:KEY_DOWN];
    }
    
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
