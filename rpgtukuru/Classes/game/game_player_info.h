/**
 * @file
 * @brief Player Information
 * @author puroject.kuto
 */
#pragma once

#include <kuto/kuto_static_string.h>
#include "game_chara_status.h"
#include <rpg2k/DataBase.hpp>


struct GamePlayerInfo
{
	const rpg2k::structure::Array1D*	baseInfo;
	GameCharaStatus			status;
	std::string				name;
	std::string				title;
	std::string				walkGraphicName;
	int						walkGraphicPos;
	bool					walkGraphicSemi;
	std::string				faceGraphicName;
	int						faceGraphicPos;
};

