#include <kuto/kuto_utility.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_file.h>

#include "game_image.h"

bool GameImage::LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue)
{
	// search current directory
	std::string temp = filename + ".bmp";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = filename + ".png";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = filename + ".xyz";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	// search runtime directory
	std::string dirName = kuto::File::getFileName(kuto::File::getDirectoryName(filename));
	std::string rtpPath = "User/Media/Photos/RPG2000/RTP/" + dirName + "/" + kuto::File::getFileName(filename);
	temp = rtpPath + ".png";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = rtpPath + ".bmp";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = filename + ".xyz";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	return false;
}
