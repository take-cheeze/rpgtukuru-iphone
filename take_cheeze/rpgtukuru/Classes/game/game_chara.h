/**
 * @file
 * @brief Game Character
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_math.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_irender.h>
// #include "CRpgEvent.h"

class GameField;

class GameChara : public kuto::Task, public kuto::IRender
{
public:
	enum DirType
	{
		kDirUp,
		kDirRight,
		kDirDown,
		kDirLeft,
	};
	class Point
	{
	public:
		Point() {}
		Point(int x, int y) : x(x), y(y) {}
		
		bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
		bool operator!=(const Point& rhs) const { return x != rhs.x || y != rhs.y; }
		
	public:
		int		x;
		int		y;
	};
	enum {
		ANIME_COUNT_MAX		= 8,
		CHARA_WIDTH			= 24,
		CHARA_HEIGHT		= 32,
		CHARA_MOVE_WIDTH	= 16,
		CHARA_MOVE_HEIGHT	= 16,
		CHARA_FACE_WIDTH	= 48,
		CHARA_FACE_HEIGHT	= 48,
	};
	enum Priority {
		kPriorityLow,
		kPriorityNormal,
		kPriorityHigh,
	};
	enum MoveResult {
		kMoveResultNone,
		kMoveResultCollied,
		kMoveResultStart,
		kMoveResultDone,
	};
	
public:
	static GameChara* createTask(kuto::Task* parent, GameField* field) { return new GameChara(parent, field); }

protected:
	GameChara(kuto::Task* parent, GameField* field);
	virtual ~GameChara();

	virtual void update();
	virtual void draw();

public:
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	void renderWalk();
	void renderFace(kuto::Graphics2D& g, const kuto::Vector2& pos);

	bool move(DirType dir, bool throughMapColli = false, bool forceSet = false);
	bool isMoving() const { return position_ != movePosition_; }

	bool loadWalkTexture(const char* filename, u8 position);
	bool loadFaceTexture(const char* filename, u8 position);
	
	const Point& getPosition() const { return position_; }
	void setPosition(const Point& pos) { position_ = pos; movePosition_ = pos; }
	DirType getDirection() const { return direction_; }
	void setDirection(DirType dir) { direction_ = dir; }
	const Point& getMovePoisition() const { return movePosition_; }
	Priority getPriority() const { return priority_; }
	bool isCrossover() const { return crossover_; }
	void startTalking(DirType dir) { direction_ = dir; talking_ = true; }
	void endTalking() { talking_ = false; }
	bool isTalking() const { return talking_; }
	MoveResult getMoveResult() const { return moveResult_; }
	bool isVisible() const { return visible_; }
	void setVisible(bool value) { visible_ = value; }
	bool isThroughColli() const { return throughColli_; }
	void setThroughColli(bool value) { throughColli_ = value; }
	
	// const CRpgRoute& getRoute() const { return route_; }
	// void setRoute(const CRpgRoute& value) { route_ = value; }
	// bool isEnableRoute() const { return routeIndex_ < (int)route_.commands.size(); }
	void startRoute() { routeIndex_ = 0; }
	void endRoute() { routeIndex_ = 0x7FFFFFFF; }
	void controlApproach();
	void controlEscape();
	void controlRoute();
	
protected:
	GameField*			gameField_;
	kuto::Texture		walkTexture_;
	kuto::Texture		faceTexture_;
	u8					walkTexturePosition_;
	u8					faceTexturePosition_;
	DirType				direction_;
	Point				position_;
	Point				movePosition_;
	int					moveCount_;
	Priority			priority_;
	MoveResult			moveResult_;
	bool				crossover_;
	bool				talking_;
	bool				visible_;
	bool				throughColli_;
	// CRpgRoute			route_;
	u32					routeIndex_;
	int					moveWaitMax_;
};	// class GameField

