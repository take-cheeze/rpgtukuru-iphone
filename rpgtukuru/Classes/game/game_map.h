/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <rpg2k/Project.hpp>

#include <memory>


class GameMap : public kuto::Task, public kuto::IRender
{
public:
	static GameMap* createTask(kuto::Task* parent) { return new GameMap(parent); }
private:
	GameMap(kuto::Task* parent);

	virtual bool initialize();
	virtual void update();
	virtual void draw();
public:
	bool load(int mapIndex, rpg2k::model::Project& sys, const char* folder);
	virtual void render();
	bool isEnableMove(int nowX, int nowY, int nextX, int nextY) const;
	void setPlayerPosition(const kuto::Vector2& pos);
	const kuto::Vector2& getOffsetPosition() const { return screenOffset_; }
	rpg2k::model::MapUnit& getRpgLmu() { return *rpgLmu_; }
	// const rpg2k::model::MapUnit& getRpgLmu() const { return *rpgLmu_; }
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
	void drawChipSet(kuto::Vector2 const& dstP, int chipID);
	void drawBlockD(kuto::Texture const& src, kuto::Vector2 const& dstP, int x, int y);
	void drawBlockA_B(kuto::Texture const& src, kuto::Vector2 const& dstP, int x, int y, int z, int anime);

private:
	struct DefferdCommand {
		kuto::Vector2			pos;
		// MapUnit::TextureInfo	info;
	};

	void drawLowerChips(bool high);
	void drawUpperChips(bool high);
private:
/*
	rpg2k::model::DataBase*			rpgLdb_;
 */
	rpg2k::model::MapUnit* rpgLmu_;
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
};

