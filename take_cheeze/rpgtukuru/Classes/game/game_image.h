#include <kuto/kuto_texture.h>

class GameImage
{
public:
	static bool LoadImage(
		kuto::Texture& texture, const std::string& filename,
		bool useAlphaPalette, int hue = 0
	);
};
