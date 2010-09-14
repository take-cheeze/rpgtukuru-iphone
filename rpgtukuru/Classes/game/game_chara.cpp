/**
 * @file
 * @brief Game Character
 * @author project.kuto
 */

#include "game_chara.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_utility.h>
#include "game_map.h"
#include "game_field.h"
#include "game_collision.h"
#include "game_player.h"


GameChara::GameChara(GameField* field)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 9.f /* - 0.001f * position_.y - 0.01f * priority_ */)
, gameField_(field), walkTexturePosition_(0), faceTexturePosition_(0)
, direction_(rpg2k::EventDir::DOWN), position_(0, 0), movePosition_(0, 0), moveCount_(0)
, priority_(rpg2k::EventPriority::CHAR), moveResult_(kMoveResultNone)
, crossover_(true), talking_(false)
, visible_(true), throughColli_(false)
, routeIndex_(0x7FFFFFFF)
, moveWaitMax_(60)
{
}

bool GameChara::loadWalkTexture(const std::string& filename, uint position)
{
	if (filename.empty()) {
		walkTexture_.destroy();
		return true;
	}
	const rpg2k::model::Project& system = gameField_->getGameSystem();
	std::string walkTextureName = system.gameDir();
	walkTextureName += "/CharSet/";
	walkTextureName += filename;
	walkTexturePosition_ = position;
	return RPG2kUtil::LoadImage(walkTexture_, walkTextureName.c_str(), true);
}

bool GameChara::loadFaceTexture(const std::string& filename, uint position)
{
	if (filename.empty()) {
		faceTexture_.destroy();
		return true;
	}
	const rpg2k::model::Project& system = gameField_->getGameSystem();
	std::string faceTextureName = system.gameDir();
	faceTextureName += "/FaceSet/";
	faceTextureName += filename;
	faceTexturePosition_ = position;
	return RPG2kUtil::LoadImage(faceTexture_, faceTextureName.c_str(), true);
}

bool GameChara::move(rpg2k::EventDir::Type dir, bool throughMapColli, bool forceSet)
{
	if (isMoving() && !forceSet)
		return false;
	direction_ = dir;
	kuto::Point2 nextPos = movePosition_;
	switch (dir) {
	case rpg2k::EventDir::UP   : nextPos.y--; break;
	case rpg2k::EventDir::DOWN : nextPos.y++; break;
	case rpg2k::EventDir::LEFT : nextPos.x--; break;
	case rpg2k::EventDir::RIGHT: nextPos.x++; break;
	}
	if (gameField_) {
		if (throughColli_ || gameField_->getCollision()->isEnableMove(position_.x, position_.y, nextPos.x, nextPos.y, priority_, throughMapColli)) {
			movePosition_ = nextPos;
			moveResult_ = kMoveResultStart;
			return true;
		}
	} else {
		movePosition_ = nextPos;
		return true;
	}
	moveResult_ = kMoveResultCollied;
	return false;
}

void GameChara::update()
{
	if (isMoving()) {
		moveCount_++;
		if (moveCount_ >= ANIME_COUNT_MAX) {
			moveCount_ = 0;
			moveResult_ = kMoveResultDone;
			position_ = movePosition_;
		}
	}
}

void GameChara::controlRoute()
{
	if (!isEnableRoute() || isMoving())
		return;
	int com = route_.commands[routeIndex_];
	switch (com) {
	case rpg2k::Action::Move::UP   : move(rpg2k::EventDir::UP   ); break;
	case rpg2k::Action::Move::RIGHT: move(rpg2k::EventDir::RIGHT); break;
	case rpg2k::Action::Move::DOWN : move(rpg2k::EventDir::DOWN ); break;
	case rpg2k::Action::Move::LEFT : move(rpg2k::EventDir::LEFT ); break;
	case rpg2k::Action::Move::RIGHT_UP:
	case rpg2k::Action::Move::RIGHT_DOWN:
	case rpg2k::Action::Move::LEFT_DOWN:
	case rpg2k::Action::Move::LEFT_UP:
		move(rpg2k::EventDir::Type(com - rpg2k::Action::Move::RIGHT_UP), false, true);
		move(rpg2k::EventDir::Type((com - rpg2k::Action::Move::RIGHT_UP + 1) % 4), false, true);
		break;
	case rpg2k::Action::Move::RANDOM:
		move(rpg2k::EventDir::Type(kuto::random(4)*2));
		break;
	case rpg2k::Action::Move::TO_PARTY:
		controlApproach();
		break;
	case rpg2k::Action::Move::FROM_PARTY:
		controlEscape();
		break;
	case rpg2k::Action::Move::A_STEP:
		move(direction_);
		break;
	case rpg2k::Action::Face::UP:
	case rpg2k::Action::Face::RIGHT:
	case rpg2k::Action::Face::DOWN:
	case rpg2k::Action::Face::LEFT:
		setDirection(rpg2k::EventDir::Type(com - rpg2k::Action::Face::UP));
		break;
	case rpg2k::Action::Turn::RIGHT_90:
		setDirection(rpg2k::EventDir::Type((direction_ + 1) % 4));
		break;
	case rpg2k::Action::Turn::LEFT_90:
		setDirection(rpg2k::EventDir::Type((direction_ + 3) % 4));
		break;
	case rpg2k::Action::Turn::OPPOSITE:
		setDirection(rpg2k::EventDir::Type((direction_ + 2) % 4));
		break;
	case rpg2k::Action::Turn::RIGHT_OR_LEFT_90:
		setDirection(rpg2k::EventDir::Type((direction_ + (kuto::random(2)? 1 : 3)) % 4));
		break;
	case rpg2k::Action::Turn::RANDOM:
		setDirection(rpg2k::EventDir::Type(kuto::random(4)));
		break;
	case rpg2k::Action::Turn::TO_PARTY:
	case rpg2k::Action::Turn::OPPOSITE_OF_PARTY:
		{
			kuto::Point2 playerPos = gameField_->getPlayerLeader()->getPosition();
			rpg2k::EventDir::Type dir = direction_;
			if (playerPos.x < position_.x)
				dir = rpg2k::EventDir::LEFT;
			else if (playerPos.x > position_.x)
				dir = rpg2k::EventDir::RIGHT;
			else if (playerPos.y < position_.y)
				dir = rpg2k::EventDir::UP;
			else if (playerPos.y > position_.y)
				dir = rpg2k::EventDir::DOWN;
			if (com == rpg2k::Action::Turn::OPPOSITE_OF_PARTY)
				dir = rpg2k::EventDir::Type((dir + 2) % 4);
			setDirection(dir);
		}
		break;
	case rpg2k::Action::HALT: // TODO
		break;
	case rpg2k::Action::BEGIN_JUMP: // TODO
		break;
	case rpg2k::Action::END_JUMP: // TODO
		break;
	case rpg2k::Action::FIX_DIR: // TODO
		break;
	case rpg2k::Action::UNFIX_DIR: // TODO
		break;
	case rpg2k::Action::SPEED_UP: // TODO
		break;
	case rpg2k::Action::SPEED_DOWN: // TODO
		break;
	case rpg2k::Action::FREQ_UP: // TODO
		break;
	case rpg2k::Action::FREQ_DOWN: // TODO
		break;
	case rpg2k::Action::SWITCH_ON:
		gameField_->getGameSystem().getLSD().setFlag(route_.commands[++routeIndex_], true);
		break;
	case rpg2k::Action::SWITCH_OFF:
		gameField_->getGameSystem().getLSD().setFlag(route_.commands[++routeIndex_], false);
		break;
	case rpg2k::Action::CHANGE_CHAR_SET:
		{
			int strSize = route_.commands[++routeIndex_];
			std::vector< int32_t >::iterator offsetIt = route_.commands.begin() + routeIndex_;
			routeIndex_ += strSize;
			loadWalkTexture(
				rpg2k::vector2string( std::vector< int32_t >(offsetIt, offsetIt + strSize) ),
				route_.commands[routeIndex_++]
			);
		}
		break;
	case rpg2k::Action::PLAY_SOUND: // TODO
		break;
	case rpg2k::Action::BEGIN_SLIP:
	case rpg2k::Action::END_SLIP:
		setThroughColli(com == rpg2k::Action::BEGIN_SLIP);
		break;
	case rpg2k::Action::BEGIN_ANIME: // TODO
		break;
	case rpg2k::Action::END_ANIME: // TODO
		break;
	case rpg2k::Action::TRANS_UP: // TODO
		break;
	case rpg2k::Action::TRANS_DOWN: // TODO
		break;
	}
	routeIndex_++;
	if (!isEnableRoute())
		routeIndex_ = route_.repeat? 0 : 0x7FFFFFFF;
}

void GameChara::controlApproach()
{
	kuto::Point2 playerPos = gameField_->getPlayerLeader()->getPosition();
	bool moving = isMoving();
	if (!moving) {
		if (playerPos.x < position_.x)
			moving = move(rpg2k::EventDir::RIGHT);
		else if (playerPos.x > position_.x)
			moving = move(rpg2k::EventDir::LEFT);
	}
	if (!moving) {
		if (playerPos.y < position_.y)
			moving = move(rpg2k::EventDir::UP);
		else if (playerPos.y > position_.y)
			moving = move(rpg2k::EventDir::DOWN);
	}
}

void GameChara::controlEscape()
{
	kuto::Point2 playerPos = gameField_->getPlayerLeader()->getPosition();
	bool moving = isMoving();
	if (!moving) {
		if (playerPos.x < position_.x)
			moving = move(rpg2k::EventDir::RIGHT);
		else if (playerPos.x > position_.x)
			moving = move(rpg2k::EventDir::LEFT);
	}
	if (!moving) {
		if (playerPos.y < position_.y)
			moving = move(rpg2k::EventDir::DOWN);
		else if (playerPos.y > position_.y)
			moving = move(rpg2k::EventDir::UP);
	}
}

void GameChara::render(kuto::Graphics2D* g) const
{
	if (!visible_) return;

	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 size(CHARA_WIDTH, CHARA_HEIGHT);
	kuto::Vector2 pos;
	int animePos = 1;
	if (!isMoving()) {
		pos.x = position_.x;
		pos.y = position_.y;
	} else {
		float ratio = ((float)moveCount_ / (float)ANIME_COUNT_MAX);
		pos.x = position_.x + (movePosition_.x - position_.x) * ratio;
		pos.y = position_.y + (movePosition_.y - position_.y) * ratio;
		animePos = (moveCount_ / 2);
		if (animePos > 2)
			animePos = 1;
	}
	pos *= CHARA_MOVE_WIDTH;
	pos.x -= (CHARA_WIDTH - CHARA_MOVE_WIDTH) / 2;
	pos.y -= (CHARA_HEIGHT - CHARA_MOVE_HEIGHT);
	if (gameField_) {
		pos += gameField_->getMap()->getOffsetPosition();
	}

	if (pos.x + size.x < 0.f || pos.x > 320.f || pos.y + size.y < 0.f || pos.y > 240.f)
		return;

	kuto::Vector2 sizeUV((float)CHARA_WIDTH / walkTexture_.getWidth(), (float)CHARA_HEIGHT / walkTexture_.getHeight());
	kuto::Vector2 texcoord0;
	texcoord0.x = ((walkTexturePosition_ % 4) * 3 + animePos) * sizeUV.x;
	texcoord0.y = ((walkTexturePosition_ / 4) * 4 + direction_) * sizeUV.y;
	kuto::Vector2 texcoord1 = texcoord0 + sizeUV;

	g->drawTexture(walkTexture_, pos, size, color, texcoord0, texcoord1);
}

void GameChara::renderFace(const kuto::Vector2& pos)
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();

	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 size(CHARA_FACE_WIDTH, CHARA_FACE_HEIGHT);
	kuto::Vector2 sizeUV((float)CHARA_FACE_WIDTH / faceTexture_.getWidth(),
					(float)CHARA_FACE_HEIGHT / faceTexture_.getHeight());
	kuto::Vector2 texcoord0;
	texcoord0.x = (faceTexturePosition_ % 4) * sizeUV.x;
	texcoord0.y = (faceTexturePosition_ / 4) * sizeUV.y;
	kuto::Vector2 texcoord1 = texcoord0 + sizeUV;

	g->drawTexture(faceTexture_, pos, size, color, texcoord0, texcoord1);
}
