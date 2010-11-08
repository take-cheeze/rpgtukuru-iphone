/**
 * @file
 * @brief Game Collision
 * @author project.kuto
 */
#pragma once

#include <vector>
#include <algorithm>
#include <kuto/kuto_math.h>
#include <kuto/kuto_task.h>

class GameMap;
class GameChara;
class GameEventMapChip;


class GameCollision : public kuto::Task, public kuto::TaskCreator<GameCollision>
{
	friend class kuto::TaskCreator<GameCollision>;
private:
	GameCollision();

	virtual void update();

public:
	// virtual void render(kuto::Graphics2D* g) const;
	bool isEnableMove(int nowX, int nowY, int nextX, int nextY, int priority, bool throughMapColli = false) const;
	void setMap(GameMap* map) { map_ = map; }
	void addChara(GameChara* chara) { characters_.push_back(chara); }
	void removeChara(GameChara* chara) { characters_.erase(std::find(characters_.begin(), characters_.end(), chara)); }
	void addEventObject(const GameEventMapChip* obj) { eventObjects_.push_back(obj); }
	void removeEventObject(const GameEventMapChip* obj) { eventObjects_.erase(std::find(eventObjects_.begin(), eventObjects_.end(), obj)); }

private:

private:
	GameMap*								map_;
	std::vector<GameChara*>					characters_;
	std::vector<const GameEventMapChip*>	eventObjects_;
};

