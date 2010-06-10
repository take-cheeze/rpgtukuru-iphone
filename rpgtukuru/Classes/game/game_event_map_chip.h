/**
 * @file
 * @brief Game Event Map Chip
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_math.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_irender.h>
#include "CRpgEvent.h"
#include "CRpgLdb.h"
#include "CRpgLmu.h"

class GameMap;


class GameEventMapChip : public kuto::Task, public kuto::IRender
{
public:
	typedef CRpgMapEvent::DrawPriority Priority;
	
public:
	static GameEventMapChip* createTask(kuto::Task* parent, CRpgLdb& ldb, GameMap* map) { return new GameEventMapChip(parent, ldb, map); }

protected:
	GameEventMapChip(kuto::Task* parent, CRpgLdb& ldb, GameMap* map);
	virtual ~GameEventMapChip();

	virtual void update();
	virtual void draw();

public:
	virtual void render();

	const kuto::Point2& getPosition() const { return position_; }
	void setPosition(const kuto::Point2& pos) { position_ = pos; }
	Priority getPriority() const { return priority_; }
	void setPriority(Priority value) { priority_ = value; }
	uint getPartsIndex() const { return partsIndex_; }
	void setPartsIndex(uint value) { partsIndex_ = value; }

protected:
	CRpgLdb&			rpgLdb_;
	GameMap*			map_;
	uint				partsIndex_;
	kuto::Point2		position_;
	Priority			priority_;
};	// class GameChara
