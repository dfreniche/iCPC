//
//  ICPCFileManager.m
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import "ICPCFileManager.h"
#import "ICPCConfig.h"

@implementation ICPCFileManager

+ (void)copyAllDSKFilesToDocumentDirectory {
    
    if ([ICPCConfig sharedInstance].dskDemosAlreadyCopied) {
        return;
    }
    
    // Copy all DSK to Documents directory
    
    NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[[NSBundle mainBundle] bundlePath] error:nil];
    NSArray *dsks = [files filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH[cd] '.dsk'"]];
    
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    
    for (NSString *dsk in dsks) {
        [[NSFileManager defaultManager] copyItemAtPath:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:dsk] toPath:[NSString stringWithFormat:@"%@/%@",documentsDirectory,dsk] error:nil];
    }
    
    [[ICPCConfig sharedInstance] setDskDemosAlreadyCopied:true];
}

+ (NSData *)loadBinFile:(NSString *)named {
    
    NSData *loaded = [[NSData alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:named ofType:@"bin"]];
    return loaded;
}

@end
