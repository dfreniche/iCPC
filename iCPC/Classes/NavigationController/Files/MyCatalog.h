//
//  MyCatalog.h
//  crocods
//
//  Created by Miguel Vanhove on 23/08/11.
//  Copyright 2011 Kyuran. All rights reserved.
//

@protocol ICPCCatalogFileSelectable <NSObject>

    - (void)fileSelected:(NSString *)fileName shouldAutoStart:(BOOL)autoStart shouldReboot:(BOOL)reboot;

@end

@interface MyCatalog : UITableViewController <UISearchDisplayDelegate, UISearchBarDelegate> {
    NSMutableArray *kfiles;
    
    NSMutableArray *indexLetters;
    NSMutableArray *indexLettersAll;
    
    NSMutableArray * _currentEntries;
    
	NSMutableArray * _entries;
	NSMutableArray * _qualifiedEntries;
    
	UISearchDisplayController *searchDisplayController;
    
    bool autoStart, rebootWhenStart;
}


@property (nonatomic, strong) NSMutableArray * _entries;
@property (nonatomic, strong) NSMutableArray * _qualifiedEntries;
@property (nonatomic, strong) UISearchDisplayController *searchDisplayController;
@property (nonatomic, strong) id <ICPCCatalogFileSelectable> delegate;

-(void)refreshData:(NSNotification *)notification;
-(void)RebuildIndex:(NSMutableArray *)_entries0;

- (void)filterEntriesForSearchText:(NSString*)searchText;

NSInteger entrySort(NSString *e1, NSString * e2, void *context);

@end
