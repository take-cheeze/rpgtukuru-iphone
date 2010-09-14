/**
 * @file
 * @brief Game BGM (Sample)
 * @author project.kuto
 */

#include "game_bgm.h"


GameBgm::GameBgm(const char* filename)
: count_(0), endCount_(30), playing_(false)
{
}

void GameBgm::update()
{
	if (isPlaying())
		count_++;
}
