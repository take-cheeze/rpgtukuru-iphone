/**
 * @file
 * @brief Font Library
 * @author project.kuto
 */

#include <vector>
#include <kuto/kuto_font.h>
#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_gl.h>
#include <kuto/kuto_types.h>


namespace {

// Vertex Array
const GLfloat panelVertices[] = {
     0,  0, // LeftTop
     0, -1, // LeftBottom
     1,  0, // RightTop
     1, -1, // RightBottom
};

// Vertex Array
const GLfloat panelUVs[] = {
     0,  0, // LeftTop
     0,  1, // LeftBottom
     1,  0, // RightTop
     1,  1, // RightBottom
};

const int FONT_TEXTURE_WIDTH = 1024;
const int FONT_TEXTURE_HEIGHT = 32;
const float FONT_BASE_SIZE = 24.0f;

struct FontInfo
{
	unsigned int	code;
	float			width;
	float			x;
	GLint			texture;
};

class FontTexture
{
public:
	FontTexture() {
		bitmapBuffer = (char*)malloc(FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT);
		std::memset(bitmapBuffer, 0, FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT);
		bitmapContext = CGBitmapContextCreate(
			bitmapBuffer, 
			FONT_TEXTURE_WIDTH, 
			FONT_TEXTURE_HEIGHT, 
			8, 
			FONT_TEXTURE_WIDTH, 
			NULL, 
			kCGImageAlphaOnly);
		kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
		// generate texture
		texture = NULL;
		glGenTextures(1, &texture);  
		device->setTexture2D(true, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
			0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmapBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		currentX = 0;
	}
	~FontTexture() {
		if (texture)
			glDeleteTextures(1, &texture);
		free(bitmapBuffer);
		bitmapBuffer = NULL;
	}
	void redrawTexture() {
		kuto::GraphicsDevice* device = kuto::GraphicsDevice::instance();
		// redraw texture
		device->setTexture2D(true, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
			0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmapBuffer);
	}

public:
	CGContextRef 			bitmapContext;
	char*					bitmapBuffer;
	GLuint 					texture;
	std::vector<FontInfo>	fontInfoList;
	float					currentX;
	unsigned int			minCode;
	unsigned int			maxCode;
};

}

@interface KutoFontImageCreater : NSObject
{
	CGRect						imageRect;
	std::vector<FontTexture*>*	pFontTextureList;
	int							currentTexture;
}

- (void)drawText:(NSString*)str position:(const kuto::Vector2&)position
				fontSize:(float)fontSize color:(const kuto::Color&)color;
- (kuto::Vector2)getTextSize:(NSString*)str fontSize:(float)fontSize;
- (const FontInfo&)getFontInfo:(unsigned int)code;

@end

@implementation KutoFontImageCreater

- (id)init
{
	if (self = [super init]) {
		imageRect = CGRectMake(0.0f, 0.0f, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT);
		pFontTextureList = new std::vector<FontTexture*>();
		currentTexture = -1;
	}
	return self;
}

- (void)dealloc
{
	for (unsigned int i = 0; i < pFontTextureList->size(); i++)
		delete (*pFontTextureList)[i];
	delete pFontTextureList;
	[super dealloc];
}

- (void)drawText:(NSString*)str position:(const kuto::Vector2&)position fontSize:(float)fontSize
				color:(const kuto::Color&)color
{
	FontTexture& fontTexture = *(*pFontTextureList)[currentTexture];
	UIFont* font = [UIFont systemFontOfSize:fontSize];
	// draw font to bitmap
	UIGraphicsPushContext(fontTexture.bitmapContext);
	[[UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a] set];
	[str drawAtPoint:CGPointMake(position.x, position.y) withFont:font];
	UIGraphicsPopContext();
	
#if 0
	CGSize size = [str sizeWithFont:font];
	u8 r = (u8)(color.r * 255.f);
	u8 g = (u8)(color.g * 255.f);
	u8 b = (u8)(color.b * 255.f);	
	for (int x = (int)position.x; x < (int)position.x + size.width; x++) {
		for (int y = FONT_TEXTURE_HEIGHT -  size.height; y < FONT_TEXTURE_HEIGHT; y++) {
			fontTexture.bitmapBuffer[x * 4 + y * FONT_TEXTURE_WIDTH * 4 + 0] = r;
			fontTexture.bitmapBuffer[x * 4 + y * FONT_TEXTURE_WIDTH * 4 + 1] = g;
			fontTexture.bitmapBuffer[x * 4 + y * FONT_TEXTURE_WIDTH * 4 + 2] = b;
		}
	}
#endif
	fontTexture.redrawTexture();
}

- (kuto::Vector2)getTextSize:(NSString*)str fontSize:(float)fontSize
{
	UIFont* font = [UIFont systemFontOfSize:fontSize];
	CGSize size = [str sizeWithFont:font];
	return kuto::Vector2((float)size.width, (float)size.height);
}

- (const FontInfo&)getFontInfo:(unsigned int)code
{
	std::vector<FontTexture*>&	fontTextureList = *pFontTextureList;
	
	for (unsigned int texIndex = 0; texIndex < fontTextureList.size(); texIndex++) {
		std::vector<FontInfo>& fontInfoList = fontTextureList[texIndex]->fontInfoList;
		for (unsigned int i = 0; i < fontInfoList.size(); i++) {
			if (code == fontInfoList[i].code) {
				return fontInfoList[i];
			}
		}
	}
	
	int codeLen = (code & 0x80)? 3:1;
	NSString* str = [[NSString alloc] initWithBytes:&code length:codeLen encoding:NSUTF8StringEncoding];
	FontInfo info;
	info.code = code;
	info.width = [self getTextSize:str fontSize:FONT_BASE_SIZE].x;
	if (fontTextureList.empty() || fontTextureList[currentTexture]->currentX + info.width > FONT_TEXTURE_WIDTH) {
		fontTextureList.push_back(new FontTexture());
		currentTexture++;
	}
	FontTexture& fontTexture = *fontTextureList[currentTexture];
	info.x = fontTexture.currentX;
	[self drawText:str position:kuto::Vector2(info.x, 0.f) fontSize:FONT_BASE_SIZE color:kuto::Color(1.f, 1.f, 1.f, 1.f)];
	info.texture = fontTexture.texture;
	fontTexture.fontInfoList.push_back(info);
	fontTexture.currentX += floor(info.width);
	[str release];
	return fontTexture.fontInfoList.back();
}

@end



static KutoFontImageCreater* fontImageCreater = NULL;


namespace kuto {

Font::Font()
{
	fontImageCreater = [[KutoFontImageCreater alloc] init];
}

Font::~Font()
{
	[fontImageCreater release];
}

void Font::drawText(const char* str, const Vector2& pos, const Color& color, float size, FONT_TYPE type)
{
	GraphicsDevice* device = GraphicsDevice::instance();
#if 1
	//NSString* nsStr = [[NSString alloc] initWithUTF8String:str];
	//const wchar_t* unicodeStr0 = (const wchar_t*)[nsStr cStringUsingEncoding:NSUTF16LittleEndianStringEncoding];
	//int strLength = wcslen(unicodeStr0);
	//const u16* unicodeStr = (const u16*)unicodeStr0;
	static GLfloat uvs[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 0.f,
		1.f, 1.f,		
	};
	device->setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device->setTexCoordPointer(2, GL_FLOAT, 0, uvs);
	device->setVertexState(true, false, true, false);
	device->setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device->setColor(color);
	float x = pos.x;
	float sizeRatio = size / FONT_BASE_SIZE;
	for (int i = 0; i < 512; i++) {
		u8 s = str[i];
		if (s == 0)
			break;
		u32 code = (u32)str[i] & 0xFF;
		if (s & 0x80) {
			if (s & 0x40) {
				i++;
				code = (((u32)str[i] << 8) & 0xFF00) | code;				
				if (s & 0x20) {
					i++;
					code = (((u32)str[i] << 16) & 0xFF0000) | code;			
				}
			}
		}
	//	for (int i = 0; i < strLength; i++) {
	//	u16 code = unicodeStr[i];
		const FontInfo& info = [fontImageCreater getFontInfo:code];
		device->setTexture2D(true, info.texture);
		uvs[0] = info.x / FONT_TEXTURE_WIDTH; uvs[1] = 0.f;
		uvs[2] = info.x / FONT_TEXTURE_WIDTH; uvs[3] = 1.f;
		uvs[4] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[5] = 0.f;
		uvs[6] = (info.x + info.width) / FONT_TEXTURE_WIDTH; uvs[7] = 1.f;
		
		Matrix mt, ms;
		mt.translation(Vector3(x, pos.y + FONT_TEXTURE_HEIGHT * sizeRatio, 0.f));
		ms.scaling(Vector3(info.width * sizeRatio, FONT_TEXTURE_HEIGHT * sizeRatio, 1.f));
		Matrix m = ms * mt;
		device->setModelMatrix(m);
		device->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
		x += info.width * sizeRatio;
	}
#else
	NSString* nsStr = [[NSString alloc] initWithUTF8String:str];
	[fontImageCreater drawText:nsStr position:pos fontSize:size color:color];
	//kuto:Vector2 texcoord = [fontImageCreater getTextSize:nsStr fontSize:size];
	[nsStr release];

	// render string
	static const Color WHITE_COLOR(1.f, 1.f, 1.f, 1.f);
	device->setVertexPointer(2, GL_FLOAT, 0, panelVertices);
	device->setTexCoordPointer(2, GL_FLOAT, 0, panelUVs);
	device->setVertexState(true, false, true, false);
	device->setTexture2D(true, fontImageCreater.texture);
	device->setBlendState(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	device->setColor(WHITE_COLOR);
	Matrix mt, ms;
	mt.translation(Vector3(pos.x, pos.y + FONT_TEXTURE_HEIGHT, 0.f));
	ms.scaling(Vector3(FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, 1.f));
	Matrix m = ms * mt;
	device->setModelMatrix(m);
	device->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
}

kuto::Vector2 Font::getTextSize(const char* str, float size, FONT_TYPE type)
{
	NSString* nsStr = [[NSString alloc] initWithUTF8String:str];
	kuto:Vector2 ret = [fontImageCreater getTextSize:nsStr fontSize:size];
	[nsStr release];
	return ret;
}

}	// namespace kuto
