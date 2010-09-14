/**
 * @file
 * @brief Game Event Map Chip
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_texture.h>
#include <rpg2k/DataBase.hpp>
#include <rpg2k/MapUnit.hpp>

class GameMap;


class GameEventMapChip : public kuto::IRender2D, public kuto::TaskCreatorParam2<GameEventMapChip, rpg2k::model::DataBase&, GameMap*>
{
	friend class kuto::TaskCreatorParam2<GameEventMapChip, rpg2k::model::DataBase&, GameMap*>;
protected:
	GameEventMapChip(rpg2k::model::DataBase& ldb, GameMap* map);

	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

	const kuto::Point2& getPosition() const { return position_; }
	void setPosition(const kuto::Point2& pos); // { position_ = pos; }
	rpg2k::EventPriority::Type getPriority() const { return priority_; }
	void setPriority(rpg2k::EventPriority::Type value) { priority_ = value; }
	uint getPartsIndex() const { return partsIndex_; }
	void setPartsIndex(uint value) { partsIndex_ = value; }

protected:
	rpg2k::model::DataBase&			rpgLdb_;
	GameMap*			map_;
	uint				partsIndex_;
	kuto::Point2		position_;
	rpg2k::EventPriority::Type			priority_;
};	// class GameChara
