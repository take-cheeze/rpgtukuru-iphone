/**
 * @file
 * @brief Skill animation
 * @autor project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_irender.h>
#include <kuto/kuto_static_vector.h>
#include "game_system.h"

class GameBattleEnemy;


class GameSkillAnime : public kuto::Task, public kuto::IRender
{
public:
	static GameSkillAnime* createTask(kuto::Task* parent, const rpg2k::model::Project& gameSystem, int animeId) { return new GameSkillAnime(parent, gameSystem, animeId); }

private:
	GameSkillAnime(kuto::Task* parent, const rpg2k::model::Project& gameSystem, int animeId);

	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();

	void play() { played_ = true; }
	bool isFinished() const { return finished_; }
	void addEnemy(GameBattleEnemy* enemy) { enemies_.push_back(enemy); }
	void setPlayPosition(const kuto::Vector2& value) { setPlayPosition_ = true; playPosition_ = value; }
	void setDeleteFinished(bool value) { deleteFinished_ = value; }

private:
	const rpg2k::model::Project&			gameSystem_;
	int							animeId_;
	kuto::Texture				texture_;
	int							counter_;
	bool						played_;
	bool						finished_;
	bool						setPlayPosition_;
	bool						deleteFinished_;
	kuto::StaticVector<GameBattleEnemy*, 16>	enemies_;
	kuto::Vector2				playPosition_;
};

