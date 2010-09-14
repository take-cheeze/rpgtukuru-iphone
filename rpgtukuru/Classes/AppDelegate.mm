/**
 * @file
 * @brief AppDelegate
 * @author project.kuto
 */

#import "AppDelegate.h"
#import <kuto/kuto_debug_menu_view.h>
#import <kuto/kuto_eagl_view.h>

#include "AppMain.h"
#include <kuto/kuto_section_manager.h>
#include <kuto/kuto_utility.h>

#include <rpg2k/Define.hpp>


static AppMain* sAppMain = NULL;

AppMain* GetAppMain() { return sAppMain; }

@implementation AppDelegate

@synthesize window;
@synthesize glView;
@synthesize animationTimer;
@synthesize animationInterval;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	CGRect rect = [[UIScreen mainScreen] bounds];
	
	// create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];
	UINavigationController* naviController = [[UINavigationController alloc] init];
	
	// create app main
	sAppMain = new AppMain();
	sAppMain->initialize();
	
	// create the OpenGL view and add it to the window
	glView = [[KutoEaglView alloc] initWithFrame:rect];
	debugView = [[KutoDebugMenuView alloc] initWithFrame:rect];
	[naviController pushViewController:debugView animated:NO];
	
	[window addSubview:naviController.view];	// set debug menu
	[window addSubview:glView];
	
	// show the window
	[window makeKeyAndVisible];
	
	//[application setStatusBarHidden:YES  animated:NO];	// info.plist
	
	self.animationInterval = 1.0 / double(rpg2k::FRAME_PER_SECOND);
	//[self startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	self.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	self.animationInterval = 1.0 / double(rpg2k::FRAME_PER_SECOND);
}


- (void)update
{
	if (!kuto::SectionManager::instance()->getCurrentTask()) {
		[window bringSubviewToFront:debugView.navigationController.view];
		
		if (debugView.selectedSectionName) {
			kuto::SectionManager::instance()->beginSection(debugView.selectedSectionName);
			debugView.selectedSectionName = NULL;
		}
	} else {
		[window bringSubviewToFront:glView];
		
		[glView preUpdate];
		sAppMain->update();
		[glView postUpdate];
	}
}

- (void)startAnimation {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(update) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
	if (self.animationTimer)
		[self.animationTimer invalidate];
    self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
	if (animationTimer)
		[animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
    }
	[self startAnimation];
}

- (void)dealloc {
    
    [self stopAnimation];
	[window release];
	[debugView release];
	[glView release];
	kuto::safeDelete(sAppMain);
	[super dealloc];
}

@end
