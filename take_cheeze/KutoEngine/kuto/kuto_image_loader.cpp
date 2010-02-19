/**
 * @file
 * @brief Image Loader
 * @author project.kuto
 */

#include "kuto_image_loader.h"
#include "kuto_graphics_device.h"
#include "kuto_error.h"
#include "kuto_utility.h"
#include "kuto_load_texture_core.h"


namespace kuto {

ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
}

bool ImageLoader::createTexture(const char* filename, LoadTextureCore& core)
{
/*
	NSString* path = [[NSString stringWithUTF8String:filename] retain];
    UIImage* image = [[[UIImage alloc] initWithContentsOfFile:path] retain];
    [path release];
    if (!image)
    	return false;
    
    // get image information
    CGImageRef imageRef = [image CGImage];   
    int orgWidth_ = CGImageGetWidth(imageRef);
    int orgHeight_ = CGImageGetHeight(imageRef);
    int width_ = fixPowerOfTwo(orgWidth_);
    int height_ = fixPowerOfTwo(orgHeight_);
    char* data_ = new char[width_ * height_ * 4];
    kuto_assert(data_);

    // check alpha
    CGImageAlphaInfo info = CGImageGetAlphaInfo(imageRef);
    bool hasAlpha = ((info == kCGImageAlphaPremultipliedLast) || 
        (info == kCGImageAlphaPremultipliedFirst) || 
        (info == kCGImageAlphaLast) || 
        (info == kCGImageAlphaFirst));
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    size_t bitsPerComponent = hasAlpha? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast;
    CGContextRef contextRef = CGBitmapContextCreate(data_, width_, height_, 
        8, 4 * width_, colorSpace, bitsPerComponent | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

	// No Scaling
    //if ((orgWidth_ != width_) || (orgHeight_ != height_)) {
        //CGContextScaleCTM(contextRef, (CGFloat)width_ / orgWidth_,
        //    (CGFloat)height_ / orgHeight_);
    //}
    CGContextDrawImage(contextRef, CGRectMake(0, 0, CGImageGetWidth(imageRef),
        CGImageGetHeight(imageRef)), imageRef);
    CGContextRelease(contextRef);
	[image release];
	
	return core.createTexture(data_, width_, height_, orgWidth_, orgHeight_, GL_RGBA);
 */
	return false;
}
 

}	// namespace kuto

