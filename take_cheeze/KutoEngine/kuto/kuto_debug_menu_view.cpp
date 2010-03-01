/**
 * @file
 * @brief UI Debug Menu
 * @author project.kuto
 */

#include "kuto_debug_menu_view.hpp"
#include "kuto_section_manager.h"


namespace kuto
{

// @implementation KutoDebugMenuView

// @synthesize tableView = tableView_;
// @synthesize selectedSectionName = selectedSectionName_;


// - (id)initWithFrame:(CGRect)frame
DebugMenuView* DebugMenuView::initWithFrame(QRect frame)
{
	DebugMenuView* ret = new DebugMenuView();
	ret->frame_ = frame;
	return ret;
/*
	if (self = [super init]) {
		self.title = @"Debug Menu";
		selectedSectionName_ = NULL;
		frame_ = frame;
	}
	return self;
 */
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
/*
- (void)viewDidLoad
{
    [super viewDidLoad];
	
	//CGRect applicationFrame = [[UIScreen mainScreen] applicationFrame];
	CGRect applicationFrame = frame_;
	CGRect naviFrame = self.navigationController.view.frame;
	CGRect tableFrame = CGRectMake(0, naviFrame.size.height, applicationFrame.size.width, applicationFrame.size.height - 48);
	tableView_ = [[UITableView alloc] initWithFrame:tableFrame];
	tableView_.delegate = self;
	tableView_.dataSource = self;
	self.view = tableView_;
}

// how many sections in the table
- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
{
	return 1;
}

// how many rows in the section
- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
{
	return kuto::SectionManager::instance()->getSectionHandles().size();
}

// create table cell view
- (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
{	
	static NSString* CellIdentifier = @"Cell";
	
	UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
	}
	
	const char* name = kuto::SectionManager::instance()->getSectionHandles()[indexPath.row]->getName().c_str();
	cell.text = [[NSString stringWithUTF8String:name] retain];
	return cell;
}

- (void)viewWillAppear:(BOOL)animated
{
	NSIndexPath* selection = [tableView_ indexPathForSelectedRow];
	if (selection) {
		[tableView_ deselectRowAtIndexPath:selection animated:YES];
	}
	[tableView_ reloadData];
}

- (void)viewDidAppear:(BOOL)animated
{
	[tableView_ flashScrollIndicators];
}

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
	selectedSectionName_ = kuto::SectionManager::instance()->getSectionHandles()[indexPath.row]->getName().c_str();
}
 */

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

/*
- (void)didReceiveMemoryWarning
{
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc
{
	[tableView_ release];
	[super dealloc];
}


@end

 */

 };
 