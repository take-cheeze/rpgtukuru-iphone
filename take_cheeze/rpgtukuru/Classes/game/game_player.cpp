/**
 * @file
 * @brief Player Class
 * @author project.kuto
 */

#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>
#include "game_player.h"
#include "game_map.h"
#include "game_field.h"
#include "game_system.h"

using namespace rpg2kLib::structure;


GamePlayer::GamePlayer(GameField* field, int playerId, GameCharaStatus& status)
: GameChara(field, field)
, playerId_(playerId)
, status_(status)
{
	GameSystem& system = gameField_->getGameSystem();
	const Array1D& player = system.getRpgLdb().getCharacter()[playerId_];

	std::string walkTextureName = system.getRootFolder();
	walkTextureName += "/CharSet/";
	walkTextureName += player[3].get_string();
	loadWalkTexture( walkTextureName.c_str(), player[4].get_int() );

	std::string faceTextureName = system.getRootFolder();
	faceTextureName += "/FaceSet/";
	faceTextureName += player[15].get_string();
	loadFaceTexture( faceTextureName.c_str(), player[16].get_int() );
}

void GamePlayer::update()
{
/*
	moveResult_ = kMoveResultNone;
	if (isEnableRoute())
		controlRoute();
	else
		controlPad();
	if (isMoving()) {
		if (gameField_ && gameField_->getGameSystem().getConfig().playerDash) {
			moveCount_++;
		}
	}
	GameChara::update();
 */
}

void GamePlayer::controlPad()
{
	if (!talking_) {
		bool throughMapColli = gameField_? gameField_->getGameSystem().getConfig().throughCollision : false;
		kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
		if (virtualPad->on(kuto::VirtualPad::KEY_UP)) {
			move(GameChara::kDirUp, throughMapColli);
		}
		if (virtualPad->on(kuto::VirtualPad::KEY_DOWN)) {
			move(GameChara::kDirDown, throughMapColli);
		}
		if (virtualPad->on(kuto::VirtualPad::KEY_LEFT)) {
			move(GameChara::kDirLeft, throughMapColli);
		}
		if (virtualPad->on(kuto::VirtualPad::KEY_RIGHT)) {
			move(GameChara::kDirRight, throughMapColli);
		}
	}
	updateMapPosition();
}

void GamePlayer::updateMapPosition()
{
	if (gameField_) {
		kuto::Vector2 pos;
		if (!isMoving()) {
			pos.x = position_.x;
			pos.y = position_.y;
		} else {
			float ratio = ((float)moveCount_ / (float)ANIME_COUNT_MAX);
			pos.x = position_.x + (movePosition_.x - position_.x) * ratio;
			pos.y = position_.y + (movePosition_.y - position_.y) * ratio;
		}
		pos *= CHARA_MOVE_WIDTH;
		gameField_->getMap()->setPlayerPosition(pos);
	}
}

