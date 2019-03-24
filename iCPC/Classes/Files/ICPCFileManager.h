//
//  ICPCFileManager.h
//  iCPC
//
//  Created by Diego Freniche Brito on 21/02/2019.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ICPCFileManager : NSObject


/**
 Copies all bundled DSK files to Documents Directory
 */
+ (void)copyAllDSKFilesToDocumentDirectory;



/**
 Load a `bin` file (ROM files, etc.)

 @param named file name containing the .bin
 @return NSDATA with loaded bytes
 */
+ (NSData *)loadBinFile:(NSString *)named;


@end

NS_ASSUME_NONNULL_END
