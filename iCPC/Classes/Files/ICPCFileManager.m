//
//  ICPCFileManager.m
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import "ICPCFileManager.h"

@implementation ICPCFileManager

+ (void)copyAllDSKFilesToDocumentDirectory {
    // Copy all DSK to Documents directory
    
    NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[[NSBundle mainBundle] bundlePath] error:nil];
    NSArray *dsks = [files filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH[cd] '.dsk'"]];
    
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    
    for (NSString *dsk in dsks) {
        [[NSFileManager defaultManager] copyItemAtPath:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:dsk] toPath:[NSString stringWithFormat:@"%@/%@",documentsDirectory,dsk] error:nil];
    }
}

@end
