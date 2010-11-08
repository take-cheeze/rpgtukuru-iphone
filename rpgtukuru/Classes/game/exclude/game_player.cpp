/**
 * @file
 * @brief Player Class
 * @author project.kuto
 */

#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>

#include "game.h"
#include "game_player.h"
#include "game_map.h"
#include "game_field.h"

#include <rpg2k/Project.hpp>


GamePlayer::GamePlayer(GameField& field, int playerId, GameCharaStatus& status)
: GameChara(field)
, playerId_(playerId)
, status_(status)
{
	const rpg2k::model::Project& system = field_.project();
	// const GamePlayerInfo& player = system.playerInfo(playerId_);
	loadWalkTexture(system.charSet(playerId_), system.charSetPos(playerId_));
	loadFaceTexture(system.faceSet(playerId_), system.faceSetPos(playerId_));
}

void GamePlayer::update()
{
	moveResult_ = kMoveResultNone;
	if (isEnableRoute())
		controlRoute();
	else
		controlPad();

	if (isMoving()) {
		if (field_.game().config().playerDash) {
			moveCount_++;
		}
	}
	GameChara::update();
}

void GamePlayer::controlPad()
{
	if (!talking_) {
		bool throughMapColli = false; // field_? field_.project().config().throughCollision : false;
		kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
		if (virtualPad.on(kuto::VirtualPad::KEY_UP)) {
			move(rpg2k::EventDir::UP, throughMapColli);
		}
		if (virtualPad.on(kuto::VirtualPad::KEY_DOWN)) {
			move(rpg2k::EventDir::DOWN, throughMapColli);
		}
		if (virtualPad.on(kuto::VirtualPad::KEY_LEFT)) {
			move(rpg2k::EventDir::LEFT, throughMapColli);
		}
		if (virtualPad.on(kuto::VirtualPad::KEY_RIGHT)) {
			move(rpg2k::EventDir::RIGHT, throughMapColli);
		}
	}
	updateMapPosition();
}

void GamePlayer::updateMapPosition()
{
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
	field_.map().setPlayerPosition(pos);
}
