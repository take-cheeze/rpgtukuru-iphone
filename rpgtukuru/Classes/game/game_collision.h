/**
 * @file
 * @brief Game Collision
 * @author takuto
 */
#pragma once

#include <vector>
#include <algorithm>
#include <kuto/kuto_math.h>
#include <kuto/kuto_task.h>

class GameMap;
class GameChara;


class GameCollision : public kuto::Task
{
public:
	static GameCollision* createTask(kuto::Task* parent) { return new GameCollision(parent); }

private:
	GameCollision(kuto::Task* parent);
	
	virtual void update();
	virtual void draw();

public:
	virtual void render();
	bool isEnableMove(int nowX, int nowY, int nextX, int nextY, int priority, bool throughMapColli = false) const;
	void setMap(GameMap* map) { map_ = map; }
	void addChara(GameChara* chara) { characters_.push_back(chara); }
	void removeChara(GameChara* chara) { characters_.erase(std::find(characters_.begin(), characters_.end(), chara)); }

private:

private:
	GameMap*						map_;
	std::vector<GameChara*>			characters_;
};

