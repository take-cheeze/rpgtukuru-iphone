/**
 * @file
 * @brief Player Class
 * @author project.kuto
 */
#pragma once

#include "game_chara.h"
#include "game_chara_status.h"


class GamePlayer : public GameChara
{
public:
	static GamePlayer* createTask(GameField* field, int playerId, GameCharaStatus& status) { return new GamePlayer(field, playerId, status); }

	int getPlayerId() const { return playerId_; }
	void updateMapPosition();
	GameCharaStatus& getStatus() { return status_; }
	const GameCharaStatus& getStatus() const { return status_; }

private:
	GamePlayer(GameField* field, int playerId, GameCharaStatus& status);
	virtual void update();
	void controlPad();

private:
	int					playerId_;
	GameCharaStatus&	status_;
};
