//  Created by Miguel Vanhove on 31/10/10.
//  Copyright 2010 Kyuran. All rights reserved.
//

#import "SwitchCell.h"

@implementation SwitchCell

@synthesize switchControl;

- (id)initWithLabel:(NSString*)labelText reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
        self.selectionStyle = UITableViewCellSelectionStyleNone;
        
        self.textLabel.text = labelText;
        
        switchControl = [[UISwitch alloc] init];
        [self addSubview:switchControl];
    }
    return self;
}


- (void)layoutSubviews {
    [super layoutSubviews];
    
    CGRect rect = self.contentView.frame;
    CGSize size = self.switchControl.frame.size;
    
    CGFloat x = rect.origin.x + rect.size.width - size.width - 10;
    CGFloat y = rect.origin.y + rect.size.height / 2.0 - size.height / 2.0;
    
    switchControl.frame = CGRectMake(x, y, 0, 0);
}

- (void)setEnabled:(BOOL)enabled {
    self.textLabel.enabled = enabled;
    self.switchControl.enabled = enabled;
}

@end
