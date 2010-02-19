/**
 * @file
 * @brief Game Character
 * @author project.kuto
 */

#include "game_chara.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
// #include "CRpgUtil.h"
#include "game_map.h"
#include "game_field.h"
#include "game_collision.h"
#include "game_player.h"

GameChara::GameChara(kuto::Task* parent, GameField* field)
: kuto::Task(parent)
, gameField_(field)
, walkTexturePosition_(0), faceTexturePosition_(0)
, direction_(kDirDown), position_(0, 0), movePosition_(0, 0), moveCount_(0)
, priority_(kPriorityNormal), moveResult_(kMoveResultNone)
, crossover_(true), talking_(false)
, visible_(true), throughColli_(false)
, routeIndex_(0x7FFFFFFF), moveWaitMax_(60)
{
}

GameChara::~GameChara()
{
}

bool GameChara::loadWalkTexture(const char* filename, u8 position)
{
	return false;
/*
	walkTexturePosition_ = position;
	return CRpgUtil::LoadImage(walkTexture_, filename, true);
 */
}

bool GameChara::loadFaceTexture(const char* filename, u8 position)
{
	return false;
/*
	faceTexturePosition_ = position;
	return CRpgUtil::LoadImage(faceTexture_, filename, true);
 */
}

bool GameChara::move(DirType dir, bool throughMapColli, bool forceSet)
{
	if (isMoving() && !forceSet)
		return false;
	direction_ = dir;
	Point nextPos = movePosition_;
	switch (dir) {
	case kDirUp:	nextPos.y--;	break;
	case kDirDown:	nextPos.y++;	break;
	case kDirLeft:	nextPos.x--;	break;
	case kDirRight:	nextPos.x++;	break;
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
/*
	if (!isEnableRoute() || isMoving())
		return;
	int com = route_.commands[routeIndex_];
	switch (com) {
	case CRpgRoute::kComMoveUp:
	case CRpgRoute::kComMoveRight:
	case CRpgRoute::kComMoveDown:
	case CRpgRoute::kComMoveLeft:
		move((DirType)com);
		break;
	case CRpgRoute::kComMoveRightUp:
	case CRpgRoute::kComMoveRightDown:
	case CRpgRoute::kComMoveLeftDown:
	case CRpgRoute::kComMoveLeftUp:
		move((DirType)(com - CRpgRoute::kComMoveRightUp), false, true);
		move((DirType)((com - CRpgRoute::kComMoveRightUp + 1) % 4), false, true);
		break;
	case CRpgRoute::kComMoveRandom:
		move((DirType)(rand() % 4));
		break;
	case CRpgRoute::kComApproachPlayer:
		controlApproach();
		break;
	case CRpgRoute::kComEscapePlayer:
		controlEscape();
		break;
	case CRpgRoute::kComMoveForward:
		move(direction_);
		break;
	case CRpgRoute::kComSightUp:
	case CRpgRoute::kComSightRight:
	case CRpgRoute::kComSightDown:
	case CRpgRoute::kComSightLeft:
		setDirection((DirType)(com - CRpgRoute::kComSightUp));
		break;
	case CRpgRoute::kComTurnRight90:
		setDirection((DirType)((direction_ + 1) % 4));
		break;
	case CRpgRoute::kComTurnLeft90:
		setDirection((DirType)((direction_ + 3) % 4));
		break;
	case CRpgRoute::kComTurn180:
		setDirection((DirType)((direction_ + 2) % 4));
		break;
	case CRpgRoute::kComTurnRandom90:
		setDirection((DirType)((direction_ + (rand() % 2 == 0? 1 : 3)) % 4));
		break;
	case CRpgRoute::kComTurnRandom:
		setDirection((DirType)(rand() % 4));
		break;
	case CRpgRoute::kComTurnPlayer:
	case CRpgRoute::kComTurnPlayerRev:
		{
			Point playerPos = gameField_->getPlayerLeader()->getPosition();
			DirType dir = direction_;
			if (playerPos.x < position_.x)
				dir = GameChara::kDirLeft;
			else if (playerPos.x > position_.x)
				dir = GameChara::kDirRight;			
			else if (playerPos.y < position_.y)
				dir = GameChara::kDirUp;
			else if (playerPos.y > position_.y)
				dir = GameChara::kDirDown;
			if (com == CRpgRoute::kComTurnPlayerRev)
				dir = (DirType)((dir + 2) % 4);
			setDirection(dir);
		}
		break;
	case CRpgRoute::kComPause:
		// Undefined
		break;
	case CRpgRoute::kComJumpStart:
		// Undefined
		break;
	case CRpgRoute::kComJumpEnd:
		// Undefined
		break;
	case CRpgRoute::kComSightPause:
		// Undefined
		break;
	case CRpgRoute::kComSightStart:
		// Undefined
		break;
	case CRpgRoute::kComSpeedUp:
		// Undefined
		break;
	case CRpgRoute::kComSpeedDown:
		// Undefined
		break;
	case CRpgRoute::kComFrequencyUp:
		// Undefined
		break;
	case CRpgRoute::kComFriquencyDown:
		// Undefined
		break;
	case CRpgRoute::kComSwitchOn:
		// Undefined
		break;
	case CRpgRoute::kComSwitchOff:
		// Undefined
		break;
	case CRpgRoute::kComGrapnicsChange:
		// Undefined
		break;
	case CRpgRoute::kComPlaySe:
		// Undefined
		break;
	case CRpgRoute::kComThroughStart:
	case CRpgRoute::kComThroughEnd:
		setThroughColli(com == CRpgRoute::kComThroughStart? true : false);
		break;
	case CRpgRoute::kComAnimePause:
		// Undefined
		break;
	case CRpgRoute::kComAnimeStart:
		// Undefined
		break;
	case CRpgRoute::kComAlphaUp:
		// Undefined
		break;
	case CRpgRoute::kComAlphaDown:
		// Undefined
		break;
	}
	routeIndex_++;
	if (!isEnableRoute())
		routeIndex_ = route_.repeat? 0 : 0x7FFFFFFF;
 */
}

void GameChara::controlApproach()
{
	Point playerPos = gameField_->getPlayerLeader()->getPosition();
	bool moving = isMoving();
	if (!moving) {
		if (playerPos.x < position_.x)
			moving = move(GameChara::kDirLeft);
		else if (playerPos.x > position_.x)
			moving = move(GameChara::kDirRight);			
	}
	if (!moving) {
		if (playerPos.y < position_.y)
			moving = move(GameChara::kDirUp);
		else if (playerPos.y > position_.y)
			moving = move(GameChara::kDirDown);			
	}
}

void GameChara::controlEscape()
{
	Point playerPos = gameField_->getPlayerLeader()->getPosition();
	bool moving = isMoving();
	if (!moving) {
		if (playerPos.x < position_.x)
			moving = move(GameChara::kDirRight);
		else if (playerPos.x > position_.x)
			moving = move(GameChara::kDirLeft);			
	}
	if (!moving) {
		if (playerPos.y < position_.y)
			moving = move(GameChara::kDirDown);
		else if (playerPos.y > position_.y)
			moving = move(GameChara::kDirUp);			
	}
}

void GameChara::draw()
{
	if (visible_)
		kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 9.f - 0.001f * position_.y - 0.01f * priority_);
}

void GameChara::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	
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
