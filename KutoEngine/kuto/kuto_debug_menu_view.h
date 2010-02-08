/**
 * @file
 * @brief Debug Menu UI View
 * @author project.kuto
 */
#pragma once

#import <UIKit/UIKit.h>


@interface KutoDebugMenuView : UIViewController <UITableViewDelegate, UITableViewDataSource>
{
@private
	UITableView*		tableView_;
	const char*			selectedSequenceName_;
	CGRect				frame_;
}

- (id)initWithFrame:(CGRect)frame;

@property (readonly, assign) UITableView* tableView;
@property (nonatomic, assign) const char* selectedSequenceName;

@end

