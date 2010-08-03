/**
 * @file
 * @brief Game Config
 * @author takuto
 */
#pragma once


/// Gameのコンフィグ　というよりデバッグ機能か
struct GameConfig
{
	enum Difficulty {
		kDifficultyEasy,
		kDifficultyNormal,
		kDifficultyHard,
		kDifficultyMax
	};
	GameConfig() : noEncount(false), alwaysEscape(false), playerDash(false), throughCollision(false), difficulty(kDifficultyNormal) {}

	bool			noEncount;			///< エンカウントしない
	bool			alwaysEscape;		///< 逃げられないバトル以外は必ず逃げられる
	bool			playerDash;			///< プレイヤーの移動速度アップ
	bool			throughCollision;	///< コリジョン判定しない
	Difficulty		difficulty;			///< 難易度
};
