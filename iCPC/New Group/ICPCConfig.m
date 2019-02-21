//
//  ICPCConfig.m
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import "ICPCConfig.h"

@interface ICPCConfig() {

bool _useIcade;
bool _useFling;
bool _useExternalKeyboard;

}
@end

@implementation ICPCConfig

+ (instancetype)sharedInstance {
    
    static dispatch_once_t once;
    static id sharedInstance;
    
    dispatch_once(&once, ^
                  {
                      sharedInstance = [[ICPCConfig alloc] initUniqueInstance];
                      [sharedInstance loadSettings];
                  });
    return sharedInstance;
}

-(instancetype)initUniqueInstance {
    return [super init];
}

- (void)loadSettings {
    
    _useExternalKeyboard = [[NSUserDefaults standardUserDefaults] boolForKey:@"useExternalKeyboard"];
    _useFling= [[NSUserDefaults standardUserDefaults] boolForKey:@"useFling"];
    _useIcade = [[NSUserDefaults standardUserDefaults] boolForKey:@"useIcade"];
    
    //    int timeout=[[NSUserDefaults standardUserDefaults] integerForKey:@"timeout"];
    
    
}

- (void)setUseExternalKeyboard:(bool)useExternalKeyboard {
    [[NSUserDefaults standardUserDefaults] setBool:useExternalKeyboard forKey:@"useExternalKeyboard"];
    _useExternalKeyboard=useExternalKeyboard;
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)setUseFling:(bool)useFling {
    [[NSUserDefaults standardUserDefaults] setBool:useFling forKey:@"useFling"];
    _useFling=useFling;
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)setUseIcade:(bool)useIcade {
    [[NSUserDefaults standardUserDefaults] setBool:useIcade forKey:@"useIcade"];
    _useIcade=useIcade;
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (bool)useExternalKeyboard {
    return _useExternalKeyboard;
}

- (bool)useFling {
    return _useFling;
}

- (bool)useIcade {
    return _useIcade;
}

@end
