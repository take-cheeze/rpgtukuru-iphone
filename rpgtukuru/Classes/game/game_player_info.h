/**
 * @file
 * @brief Player Information
 * @author puroject.kuto
 */
#pragma once

#include <kuto/kuto_static_string.h>
#include "game_chara_status.h"
#include "CRpgLdb.h"


struct GamePlayerInfo
{
	const CRpgLdb::Player*	baseInfo;
	GameCharaStatus			status;
	std::string				name;
	std::string				title;
	std::string				walkGraphicName;
	int						walkGraphicPos;
	std::string				faceGraphicName;
	int						faceGraphicPos;
};

