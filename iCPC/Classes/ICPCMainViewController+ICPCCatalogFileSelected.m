//
//  ICPCMainViewController+ICPCCatalogFileSelected.m
//  iCPC
//
//  Created by Diego Freniche Brito on 23/02/2019.
//

#import "ICPCMainViewController+ICPCCatalogFileSelected.h"

#import "nds.h"
#import "upd.h"
#import "autotype.h"
#import "snapshot.h"
#import "plateform.h"

@implementation ICPCMainViewController (ICPCCatalogFileSelected) 

- (void)fileSelected:(NSString *)fileName shouldAutoStart:(BOOL)autoStart shouldReboot:(BOOL)reboot {
    
    NSString* extension = [[fileName pathExtension] lowercaseString];
    
    if ([extension isEqualToString:@"sna"]) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *fullFile = [(NSString *)[paths objectAtIndex:0] stringByAppendingPathComponent:fileName];
        
        NSData *rom=[[NSData alloc] initWithContentsOfFile:fullFile];
        LireSnapshotMem((u8*)[rom bytes]);
    }
    
    if ([extension isEqualToString:@"dsk"]) {
        char autofile[256];
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *fullFile = [(NSString *)[paths objectAtIndex:0] stringByAppendingPathComponent:fileName];
        
        NSData *dsk=[[NSData alloc] initWithContentsOfFile:fullFile];
        LireDiskMem((u8*)[dsk bytes], [dsk length], autofile);
        
        if ((autoStart) && (autofile[0]!=0)) {
            char buffer[256];
            sprintf(buffer,"run\"%s\n", autofile);
            AutoType_SetString(buffer, reboot);
        }
    }
    
    [[self keyboardController] focus];
}

    
@end
