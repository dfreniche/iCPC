//
//  ICPC.m
//  iCPC
//
//  Created by Diego Freniche Brito on 24/03/2019.
//

#import "ICPC.h"

#import "ICPCScreen.h"
#import "ICPCFileManager.h"

@interface ICPC ()

@property (nonatomic, strong) ICPCScreen *screen;

@end

@implementation ICPC

- (void)start {
    
        
//
//    
//    AutoType_Init();
//    
//    if ( ReadConfig() )
//    {
//        NSLog(@"Read config: OK");
//        if ( InitMemCPC((char*)[cpc6128 bytes], (char*)[romdisc bytes]) )
//        {
//            NSLog(@"Init Memory CPC: OK");
//            
//            ResetZ80();
//            ResetUPD();
//            //    ResetCRTC();
//            InitPlateforme(pSurface, width0);
//            
//            NSLog(@"Begin emulation");
//            
//            Autoexec();
//            
//            [self performSelectorInBackground:@selector(Go) withObject:nil];
//            [self performSelectorInBackground:@selector(GoSound) withObject:nil];
//            
//            //           EjectDiskUPD();
//            // PauseSound();
//            
//        }
//        else
//        {
//            NSLog(@"Roms du CPC non trouvÈes");
//        }
//    }
//    
//    else {
//        NSLog(@ "Fichier de configuration du CPC non trouvÈ." );
//    }
//    
//    
//    return 0;
    
}

- (BOOL)initializeEmulator {
    
    BOOL success = YES;
    
    // Load BIN files
    NSData *cpc6128 = [ICPCFileManager loadBinFile:@"cpc6128"];
    NSData *romdisc = [ICPCFileManager loadBinFile:@"romdisc"];
 
    [self.screen setMonitorType:ColorMonitor];
    
    // Init Screen borders
    [self.screen initBorder:0 borderY:0];
    
//    ExecuteMenu(ID_COLOR_MONITOR, NULL);
//    ExecuteMenu(ID_SCREEN_AUTO, NULL);
//    
//    // ExecuteMenu(ID_SCREEN_OVERSCAN, NULL);
//    ExecuteMenu(ID_KEY_JOYSTICK, NULL);
//    ExecuteMenu(ID_NODISPFRAMERATE, NULL);
//    ExecuteMenu(ID_HACK_TABCOUL, NULL);
    
    
#ifdef USE_Z80_ORIG
    ExecInstZ80 = ExecInstZ80_orig;
    ResetZ80 = ResetZ80_orig;
    SetIRQZ80 = SetIRQZ80_orig;
#endif
    
#ifdef USE_Z80_ASM
    ExecInstZ80= ExecInstZ80_asm;
    ResetZ80 = ResetZ80_asm;
    SetIRQZ80 = SetIRQZ80_asm;
#endif
    
#ifdef USE_Z80_ASM2
    ExecInstZ80= ExecInstZ80_asm2;
    ResetZ80 = ResetZ80_asm2;
    SetIRQZ80 = SetIRQZ80_asm2;
#endif
    
//    strcpy(currentfile,"nofile");
    
    return success;
}

@end
