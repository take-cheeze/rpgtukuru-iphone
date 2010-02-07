//
//  rpgtukuruAppDelegate.h
//  rpgtukuru
//
//  Created by takuto on 09/05/18.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class KutoEaglView;
@class KutoDebugMenuView;

@interface AppDelegate : NSObject <UIApplicationDelegate>
{
@private
    UIWindow*			window;
    KutoEaglView*		glView;
	KutoDebugMenuView*	debugView;
	
    NSTimer *animationTimer;
    NSTimeInterval animationInterval;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet KutoEaglView *glView;
@property NSTimeInterval animationInterval;
@property (nonatomic, assign) NSTimer *animationTimer;

- (void)startAnimation;
- (void)stopAnimation;
- (void)update;

@end

