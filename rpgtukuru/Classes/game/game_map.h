/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_point2.h>
#include <kuto/kuto_texture.h>

#include <rpg2k/Define.hpp>

#include <stack>
#include <vector>

class GameField;
class GameMapObject;
class GameParty;

namespace rpg2k
{
	namespace model
	{
		class DataBase;
		class MapUnit;
		class Project;
		class SaveData;
	}
	namespace structure
	{
		class Array1D;
		class EventState;
	}
}


class GameMap : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameMap, GameField&>
{
	friend class kuto::TaskCreatorParam1<GameMap, GameField&>;
private:
	GameMap(GameField&);

	virtual bool initialize();
	virtual void update();
	virtual void render(kuto::Graphics2D& g) const;

public:
	void setPlayerPosition(const kuto::Vector2& pos);
	const kuto::Vector2& offsetPosition() const { return screenOffset_; }
	int chipFlag(int x, int y, bool upper) const;
	bool isCounter(int x, int y) const;
	int terrainID(int x, int y) const;
	bool isEnableScroll() const { return enableScroll_; }
	void enableScroll(bool value = true) { enableScroll_ = value; }
	void scroll(int x, int y, float speed);
	void scrollBack(float speed);
	bool isScrolling() const { return scrollRatio_ < 1.f; }
	int startX() const { return (int)(-screenOffset_.x / 16.f); }
	int startY() const { return (int)(-screenOffset_.y / 16.f); }

	unsigned count() const { return counter_; }
	GameField& field() { return field_; }
	unsigned pageNo(unsigned evID) const { return pageNo_[evID]; }
	void moveMap(unsigned mapID, unsigned x, unsigned y);
	rpg2k::structure::Array1D const& page(unsigned evID) const;

	bool move(unsigned evID, rpg2k::EventDir::Type dir);
	void move(unsigned evID, kuto::Point2 dst);
	bool canPass(unsigned evID, rpg2k::EventDir::Type dir) const;

	static kuto::Point2 directionAdd(rpg2k::EventDir::Type const t);
	static kuto::Point2 directionAdd(rpg2k::CharSet::Dir::Type const t);

	bool justMoved() const { return justMoved_; }

	std::stack<unsigned>& touchFromEvent() { return touchFromEvent_; }
	std::stack<unsigned>& touchFromParty() { return touchFromParty_; }
	std::stack<unsigned>& keyEnter() { return keyEnter_; }

	bool moveWhenWait() const { return cache_.moveWhenWait; }
	rpg2k::structure::EventState& party() { return(*cache_.party); }
	rpg2k::structure::EventState const& party() const { return(*cache_.party); }

protected:
	bool isAbove(int chipID) const;
	bool isCounter(int chipID) const;
	uint8_t pass(int chipID) const;
	int terrainID(int chipID) const;

	bool canPassMap(rpg2k::EventDir::Type dir
	, kuto::Point2 const& cur, kuto::Point2 const& nxt) const;
	bool canPassEvent(unsigned evID, rpg2k::EventDir::Type dir
	, kuto::Point2 const& cur, kuto::Point2 const& nxt) const;
	bool canPassEvent(unsigned evID, rpg2k::EventDir::Type dir
	, kuto::Point2 const& cur, kuto::Point2 const& nxt, bool collision);

	void drawChip(kuto::Graphics2D& g
	, kuto::Texture const& src, kuto::Vector2 const& dstP, unsigned chipID) const;
	void drawBlockD(kuto::Graphics2D& g
	, kuto::Texture const& src, kuto::Vector2 const& dstP, unsigned x, unsigned y) const;
	void drawBlockA_B(kuto::Graphics2D& g, kuto::Texture const& src
	, kuto::Vector2 const& dstP, unsigned x, unsigned y, unsigned z, unsigned anime) const;

	void drawChar(kuto::Graphics2D& g
	, rpg2k::RPG2kString const& name, unsigned pos
	, rpg2k::CharSet::Dir::Type, rpg2k::CharSet::Pat::Type
	, kuto::Vector2 const& dstP
	) const;

private:
	GameField&			field_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	int					renderCount_;
	bool				enableScroll_;
	bool				scrolled_;
	float				scrollRatio_;
	kuto::Vector2		scrollBase_;
	kuto::Vector2		scrollOffset_;
	float				scrollSpeed_;
	kuto::Vector2		screenOffsetBase_;
	kuto::Vector2		panoramaAutoScrollOffset_;
	unsigned counter_;

	std::vector<GameMapObject*> objects_;
	std::vector< std::vector< std::multimap<unsigned, unsigned> > > eventMap_; // (priority, y, x)
	std::vector<unsigned> pageNo_;
	std::stack<unsigned> touchFromEvent_, touchFromParty_, keyEnter_;

	bool justMoved_;

	GameParty& partyObj_;

	struct
	{
		rpg2k::model::Project* project;
		rpg2k::model::DataBase* ldb;
		rpg2k::model::MapUnit* lmu;
		rpg2k::model::SaveData* lsd;
		std::vector<uint8_t> const* upperChipFlag;
		std::vector<uint8_t> const* lowerChipFlag;
		std::vector<uint16_t> const* terrain;
		bool moveWhenWait;
		rpg2k::structure::EventState* party;
		kuto::Point2 mapSize;
		int scrollFlag;
		std::string panorama;
	} cache_;
	void updateCache();
};
