/**
 * @file
 * @brief Player Class
 * @author project.kuto
 */
#pragma once

#include "game_chara.h"
#include "game_chara_status.h"


class GamePlayer : public GameChara, public kuto::TaskCreatorParam3<GamePlayer, GameField&, int, GameCharaStatus&>
{
	friend class kuto::TaskCreatorParam3<GamePlayer, GameField&, int, GameCharaStatus&>;
public:
	int playerID() const { return playerId_; }
	void updateMapPosition();
	GameCharaStatus& status() { return status_; }
	const GameCharaStatus& status() const { return status_; }

private:
	GamePlayer(GameField& field, int playerId, GameCharaStatus& status);
	virtual void update();
	void controlPad();

private:
	int					playerId_;
	GameCharaStatus&	status_;
};
