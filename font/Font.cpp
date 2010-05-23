#include "Font.hpp"
#include <kuto/kuto_types.h>

namespace rpg2kLib
{
	namespace font
	{

void Font::getTen(int& ku, int& ten, uint8_t sbyte)
{
	if(sbyte <= 0x9f) {
		ku--;
		if(sbyte <0x7f)
			ten = sbyte - 0x3f;
		else
			ten = sbyte - 0x7f + 63;
	} else ten = sbyte - 0x9e;
}

const void* Font::getGothic(const char* input0)
{
	const unsigned char* input = (const unsigned char*)input0;
	if( *input < 0x80 ) {
		return HANKAKU[static_cast<int>(*input)];
	} else if( (0x80 <  *input) && (*input < 0xa0) ) {
		int ku = (*input - 0x80) * 2, ten;
		getTen(ku, ten, *(input+1));
		return GOTHIC[ku][ten];
	} else if( (0xa0 <  *input) && (*input < 0xe0) ) {
		return HANKAKU[static_cast<int>(*input)];
	} else if( (0xe0 <= *input) && (*input < 0xfd) ) {
		int ku = (*input - 0xdf) * 2, ten;
		getTen(ku, ten, *(input+1));
		return GOTHIC[ku][ten];
	} else return NULL;
}
const void* Font::getMincho(const char* input0)
{
	const unsigned char* input = (const unsigned char*)input0;
	if( *input < (u8)0x80 ) {
		return HANKAKU[static_cast<int>(*input)];
	} else if( (0x80 <  *input) && (*input < 0xa0) ) {
		int ku = (*input - 0x80) * 2, ten;
		getTen(ku, ten, *(input+1));
		return MINCHO[ku][ten];
	} else if( (0xa0 <  *input) && (*input < 0xe0) ) {
		return HANKAKU[static_cast<int>(*input)];
	} else if( (0xe0 <= *input) && (*input < 0xfd) ) {
		int ku = (*input - 0xdf) * 2, ten;
		getTen(ku, ten, *(input+1));
		return MINCHO[ku][ten];
	} else return NULL;
}

const uint16_t* Font::getExternalChar(char c)
{
	if( ( 'a' <= c ) && ( c <= 'z' ) ) return EXTERNAL_LW[c-'a'];
	else if( ( 'A' <= c ) && ( c <= 'Z' ) ) return EXTERNAL_UP[c-'A'];
	else return NULL;
}

bool Font::isFullChar(const char* input0)
{
	const unsigned char* input = (const unsigned char*)input0;
	if( ( (0x80 <  *input) && (*input < 0xa0) ) ||
			( (0xe0 <= *input) && (*input < 0xfd) )
	) {
		return true;
	} else return false;
}
bool Font::isHalfChar(const char* input0)
{
	const unsigned char* input = (const unsigned char*)input0;
	if( ( *input < (u8)0x80 ) ||
			( (0xa0 <  *input) && (*input < 0xe0) )
	) {
		return true;
	} else return false;
}

	}; // namespace rpg2kLib
}; // namespace font
