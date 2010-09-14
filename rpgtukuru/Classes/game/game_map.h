/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <rpg2k/Project.hpp>
#include <CRpgLmu.h>

#include <deque>
#include <map>
#include <memory>
#include <stack>


class GameMap : public kuto::IRender2D, public kuto::TaskCreator<GameMap>
{
	friend class kuto::TaskCreator<GameMap>;
private:
	GameMap();

	virtual bool initialize();
	virtual void update();

public:
	bool load(int mapIndex, rpg2k::model::Project& sys);
	virtual void render(kuto::Graphics2D* g) const;
	bool isEnableMove(int nowX, int nowY, int nextX, int nextY) const;
	void setPlayerPosition(const kuto::Vector2& pos);
	const kuto::Vector2& getOffsetPosition() const { return screenOffset_; }
	CRpgLmu& getRpgLmu() { return rpgLmu_; }
	const CRpgLmu& getRpgLmu() const { return rpgLmu_; }
	const rpg2k::structure::Array1D& getChipSet() const { return gameSystem_->chipSet(); }
	int getChipFlag(int x, int y, bool upper) const;
	bool isCounter(int x, int y) const;
	int getTerrainId(int x, int y) const;
	int getMapId() const { return mapId_; }
	bool isEnableScroll() const { return enableScroll_; }
	void setEnableScroll(bool value) { enableScroll_ = value; }
	void scroll(int x, int y, float speed);
	void scrollBack(float speed);
	bool isScrolling() const { return scrollRatio_ < 1.f; }
	int getStartX() const { return (int)(-screenOffset_.x / 16.f); }
	int getStartY() const { return (int)(-screenOffset_.y / 16.f); }

protected:
	void drawChip(kuto::Vector2 const& dstP, int const chipID) const;
	void drawBlockD(kuto::Texture const& src, kuto::Vector2 const& dstP, int const x, int const y) const;
	void drawBlockA_B(kuto::Texture const& src, kuto::Vector2 const& dstP, int const x, int const y, int const z, int const anime) const;

private:
	struct DefferdCommand {
		kuto::Vector2			pos;
		CRpgLmu::TextureInfo	info;
	};

	void drawLowerChips(bool high) const;
	void drawUpperChips(bool high) const;

private:
	rpg2k::model::DataBase*			rpgLdb_;
	CRpgLmu rpgLmu_;
	rpg2k::model::Project* gameSystem_;
	kuto::Texture chipSetTex_;
	int					mapId_;
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

	std::stack<uint> touchFromParty_, touchFromEvent_;

	// (priority, y, x)
	std::deque< std::map< uint, std::multimap<uint, uint> > > eventMap_;
	std::map<uint, uint> pageNum_;
};
