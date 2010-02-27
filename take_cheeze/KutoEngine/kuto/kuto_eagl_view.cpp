/**
 * @file
 * @brief OpenGL View
 * @author project.kuto
 */

// #import <QuartzCore/QuartzCore.h>
// #import <OpenGLES/EAGLDrawable.h>
#include "kuto_eagl_view.hpp"
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_font.h>
#include <kuto/kuto_touch_pad.h>


#define USE_DEPTH_BUFFER 0

/*
// A class extension to declare private methods
@interface KutoEaglView ()

@property (nonatomic, retain) EAGLContext*	context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;
- (void) setTouchPad:(NSSet*)touches withEvent:(UIEvent*)event;

@end
 */

/*
@implementation KutoEaglView

@synthesize context;


// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithFrame:(CGRect)frame {
    
    if ((self = [super initWithFrame:frame])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
    }
    return self;
}


- (void)preUpdate {    
    [EAGLContext setCurrentContext:context];
}

- (void)postUpdate {
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    // initialize GraphicsDevice
    kuto::GraphicsDevice::instance()->initialize(viewRenderbuffer, viewFramebuffer, depthRenderbuffer,
    											backingWidth, backingHeight);
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (void)dealloc {
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
    [super dealloc];
}

- (void) setTouchPad:(NSSet*)touches withEvent:(UIEvent*)event
{
	kuto::TouchInfo info[kuto::TouchPad::MAX_TOUCH];
	int infoSize = 0;
	for (UITouch *touch in touches) {
		CGPoint touchLocation = [touch locationInView:self];
		CGPoint touchPrevLocation = [touch previousLocationInView:self];
		
		info[infoSize].position_.set(touchLocation.x, touchLocation.y);
		info[infoSize].prevPosition_.set(touchPrevLocation.x, touchPrevLocation.y);
		
		switch (touch.phase) {
		case UITouchPhaseBegan:
			info[infoSize].onFlag_ = true;
			info[infoSize].pressFlag_ = true;
			break;
		case UITouchPhaseMoved:
			info[infoSize].onFlag_ = true;
			info[infoSize].moveFlag_ = true;
			break;
		case UITouchPhaseStationary:
			info[infoSize].onFlag_ = true;
			break;
		case UITouchPhaseEnded:
			info[infoSize].releaseFlag_ = true;
			if (touch.tapCount >= 1) {
				info[infoSize].clickFlag_ = true;
				if (touch.tapCount == 2)
					info[infoSize].doubleClickFlag_ = true;
			}
			break;
		case UITouchPhaseCancelled:
			break;
		}
		infoSize++;
	}
	kuto::TouchPad::instance()->setTouches(info, infoSize);
}

- (BOOL)isMultipleTouchEnabled {
    return YES;
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	// touches has Began touch event only... Need to send allTouches.
	[self setTouchPad:[event allTouches] withEvent:event];
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self setTouchPad:[event allTouches] withEvent:event];
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self setTouchPad:[event allTouches] withEvent:event];
}

@end

 */
