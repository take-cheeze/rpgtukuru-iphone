/**
 * @file
 * @brief Game Character
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_texture.h>

#include <rpg2k/Define.hpp>

#include <vector>

#include <CRpgLmu.h>

class GameField;


class GameChara : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameChara, GameField&>
{
	friend class kuto::TaskCreatorParam1<GameChara, GameField&>;
public:
	enum {
		ANIME_COUNT_MAX		= 8,
		CHARA_WIDTH			= 24,
		CHARA_HEIGHT		= 32,
		CHARA_MOVE_WIDTH	= 16,
		CHARA_MOVE_HEIGHT	= 16,
		CHARA_FACE_WIDTH	= 48,
		CHARA_FACE_HEIGHT	= 48,
	};
	enum MoveResult {
		kMoveResultNone,
		kMoveResultCollied,
		kMoveResultStart,
		kMoveResultDone,
	};

protected:
	GameChara(GameField& field);

	virtual void update();
	virtual void render(kuto::Graphics2D& g) const;

public:
	void renderWalk(kuto::Graphics2D& g);
	void renderFace(kuto::Graphics2D& g, const kuto::Vector2& pos);

	bool move(rpg2k::EventDir::Type dir, bool throughMapColli = false, bool forceSet = false);
	bool isMoving() const { return position_ != movePosition_; }

	bool loadWalkTexture(const std::string& filename, uint position);
	bool loadFaceTexture(const std::string& filename, uint position);

	const kuto::Point2& position() const { return position_; }
	void setPosition(const kuto::Point2& pos) { position_ = pos; movePosition_ = pos; }
	rpg2k::EventDir::Type direction() const { return direction_; }
	void setDirection(rpg2k::EventDir::Type dir) { direction_ = dir; }
	const kuto::Point2& movePosition() const { return movePosition_; }
	rpg2k::EventPriority::Type priority() const { return priority_; }
	bool isCrossover() const { return crossover_; }
	void startTalking(rpg2k::EventDir::Type dir) { direction_ = dir; talking_ = true; }
	void endTalking() { talking_ = false; }
	bool isTalking() const { return talking_; }
	MoveResult moveResult() const { return moveResult_; }
	bool isVisible() const { return visible_; }
	void setVisible(bool value) { visible_ = value; }
	bool isThroughColli() const { return throughColli_; }
	void setThroughColli(bool value) { throughColli_ = value; }

	const CRpgRoute& route() const { return route_; }
	void setRoute(const CRpgRoute& value) { route_ = value; }
	bool isEnableRoute() const { return routeIndex_ < route_.commands.size(); }
	void startRoute() { routeIndex_ = 0; }
	void endRoute() { routeIndex_ = 0x7FFFFFFF; }
	void controlApproach();
	void controlEscape();
	void controlRoute();

protected:
	GameField&			field_;
	kuto::Texture		walkTexture_;
	kuto::Texture		faceTexture_;
	uint				walkTexturePosition_;
	uint				faceTexturePosition_;
	rpg2k::EventDir::Type				direction_;
	kuto::Point2		position_;
	kuto::Point2		movePosition_;
	int					moveCount_;
	rpg2k::EventPriority::Type			priority_;
	MoveResult			moveResult_;
	bool				crossover_;
	bool				talking_;
	bool				visible_;
	bool				throughColli_;
	CRpgRoute			route_;
	uint				routeIndex_;
	int					moveWaitMax_;
};	// class GameChara
