/**
 * @file
 * @brief Game Config
 * @author takuto
 */
#pragma once

#include <string>


/// Gameのコンフィグ　というよりデバッグ機能か
/// or for cheating
struct GameConfig
{
	GameConfig(std::string const& projName)
	: noEncount(false), alwaysEscape(false), playerDash(false), throughCollision(false)
	, difficulty(kDifficultyNormal)
	, projectName_(projName)
	{
	}

	bool			noEncount;			///< エンカウントしない
	bool			alwaysEscape;		///< 逃げられないバトル以外は必ず逃げられる
	bool			playerDash;			///< プレイヤーの移動速度アップ
	bool			throughCollision;	///< マップで衝突判定しない
	enum Difficulty {
		kDifficultyEasy,
		kDifficultyNormal,
		kDifficultyHard,
		kDifficultyMax
	}				difficulty;			///< 難易度

	std::string const& projectName() const { return projectName_; }
private:
	std::string projectName_;

	/*
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int) 
	{
		ar & BOOST_SERIALIZATION_NVP(noEncount);
		ar & BOOST_SERIALIZATION_NVP(alwaysEscape);
		ar & BOOST_SERIALIZATION_NVP(playerDash);
		ar & BOOST_SERIALIZATION_NVP(throughCollision);
		ar & BOOST_SERIALIZATION_NVP(difficulty);
		ar & boost::serialization::make_nvp("projectName", projectName_);
	}
	 */
};
