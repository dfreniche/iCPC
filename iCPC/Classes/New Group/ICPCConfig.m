//
//  ICPCConfig.m
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import "ICPCConfig.h"

@implementation ICPCConfig

@synthesize useExternalKeyboard = _useExternalKeyboard;
@synthesize useFling = _useFling;
@synthesize useIcade = _useIcade;
@synthesize dskDemosAlreadyCopied = _dskDemosAlreadyCopied;

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
    
    self.useExternalKeyboard = [[NSUserDefaults standardUserDefaults] boolForKey:@"useExternalKeyboard"];
    self.useFling = [[NSUserDefaults standardUserDefaults] boolForKey:@"useFling"];
    self.useIcade = [[NSUserDefaults standardUserDefaults] boolForKey:@"useIcade"];
    self.dskDemosAlreadyCopied = [[NSUserDefaults standardUserDefaults] boolForKey:@"dskDemosAlreadyCopied"];
}

- (void)setUseExternalKeyboard:(bool)useExternalKeyboard {
    
    _useExternalKeyboard = useExternalKeyboard;
    [self saveSetting:useExternalKeyboard forKey:@"useExternalKeyboard"];
}

- (void)setUseFling:(bool)useFling {
    
    _useFling = useFling;
    [self saveSetting:useFling forKey:@"useIcade"];
}

- (void)setUseIcade:(bool)useIcade {
    
    _useIcade = useIcade;
    [self saveSetting:useIcade forKey:@"useIcade"];
}
    
- (void)setDskDemosAlreadyCopied:(bool)dskDemosAlreadyCopied {
    _dskDemosAlreadyCopied = dskDemosAlreadyCopied;
    [self saveSetting:dskDemosAlreadyCopied forKey:@"dskDemosAlreadyCopied"];
}

- (void)saveSetting:(bool)value forKey:(NSString *)key {
    
    [[NSUserDefaults standardUserDefaults] setBool:value forKey:key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
