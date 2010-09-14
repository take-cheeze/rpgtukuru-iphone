/**
 * @file
 * @brief Save Data
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_types.h>
#include "game_chara_status.h"

class GameField;


class GameSaveDataHeader
{
public:
	void save(GameField* gameField);
public:
	uint64_t		time_;					///< タイムスタンプ
	std::string		leaderName_;			///< 先頭のキャラの名前
	int				leaderLevel_;			///< 先頭のキャラのLv
	int				leaderHp_;				///< 先頭のキャラのHP
	int				partyNum_;				///< パーティー人数
	int				partyId_[4];			///< パーティーのID
	std::string		partyImage_[4];			///< 1人目顔絵ファイル
	int				partyImageNo_[4];		///< 1人目顔絵番号
};

class GameSaveDataSystem
{
public:
	void save(GameField* gameField);
	void load(GameField* gameField);
private:
	std::string			systemTexture_;			///< システムグラフィック
	bool				switches_[5000];		///< スイッチ
	int					vars_[5000];			///< 変数
	bool				enableTeleport_;		///< テレポート可能か
	bool				enableEscape_;			///< エスケープ可能か
	bool				enableSave_;			///< セーブ可能か
	bool				enableMenu_;			///< メニューの呼び出し可能か
	std::string			battleMap_;				///< 戦闘背景
	int					saveCount_;				///< セーブ回数
	int					battleCount_;			///< バトル回数
	int					winCount_;				///< 勝利回数
	int					loseCount_;				///< 敗北回数
	int					escapeCount_;			///< 逃走回数
};

class GameSaveDataLocation
{
public:
	void save(GameField* gameField);

	int getMapId() const { return mapId_; }
	int getX() const { return x_; }
	int getY() const { return y_; }
	int getDirection() const { return dir_; }
private:
	int				mapId_;		///< マップ番号
	int				x_;			///< X座標
	int				y_;			///< Y座標
	int				dir_;		///< 方向
};

class GameSaveDataInventory
{
public:
	void save(GameField* gameField);
	void load(GameField* gameField);
private:
	int					money_;					///< 金
	char				items_[4096];			///< アイテム数
};

class GameSaveDataPlayers
{
public:
	struct PlayerInfo {
		GameCharaStatusBase		status;
	};
public:
	void save(GameField* gameField);
	void load(GameField* gameField);
private:
	int					playerNum_;				///< player number
	PlayerInfo			playerInfos_[32];		///< player infomations
};

class GameSaveData
{
public:
	void save(GameField* gameField);
	void load(GameField* gameField);

	const GameSaveDataHeader& getHeader() const { return header_; }
	const GameSaveDataLocation& getLocation() const { return location_; }
private:
	GameSaveDataHeader		header_;	///< data header
	GameSaveDataSystem		system_;	///< system infomation
	GameSaveDataLocation	location_;	///< Location
	GameSaveDataInventory	inventory_;	///< inventory
	GameSaveDataPlayers		players_;	///< players
};

