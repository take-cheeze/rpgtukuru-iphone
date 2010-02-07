/**
 * @file
 * @brief Game Collision
 * @author takuto
 */

#include "game_collision.h"
#include "game_chara.h"
#include "game_map.h"


GameCollision::GameCollision(kuto::Task* parent)
: kuto::Task(parent)
, map_(NULL)
{
}

void GameCollision::update()
{
}

void GameCollision::draw()
{
}

void GameCollision::render()
{
}


bool GameCollision::isEnableMove(int nowX, int nowY, int nextX, int nextY, int priority, bool throughMapColli) const
{
	for (u32 i = 0; i < characters_.size(); i++) {
		GameChara* chara = characters_[i];
		if (chara->isFreeze() || chara->getPriority() != priority)
			continue;
		if (nextX == chara->getPosition().x && nextY == chara->getPosition().y)
			return false;
		if (chara->isMoving()) {
			if (nextX == chara->getMovePoisition().x && nextY == chara->getMovePoisition().y)
				return false;
		}
	}
	return map_ && !throughMapColli? map_->isEnableMove(nowX, nowY, nextX, nextY) : true;
}
