/**
 * @file
 * @brief RPG_RT.lmt(LcfMapTree)を管理するクラス
 * @author project.kuto
*/
#ifndef _INC_CRPGLMT
#define _INC_CRPGLMT

#include "CRpgIOBase.h"
#include <string>

/// RPG_RT.lmt(LcfMapTree)を管理するクラス
class CRpgLmt : public CRpgIOBase{
private:
	const char* GetHeader(){ return "LcfMapTree"; }	///< ファイルごとの固有ヘッダ(CRpgIOBaseからオーバーライドして使います)

public:
	/// エンカウントする敵グループ
	struct EnemyGroup
	{
		int		enemyGroupID;	///< 0x01	敵グループのID
	};
	/// マップデータ部分
	struct MapInfo {
		int							m_MapId;			///< 0x00:map ID
		std::string					m_MapName;			///< 0x01:マップ名
		int							m_ParentMapID;		///< 0x02:親マップのID
		int							m_MapCategory;		///< 0x04:マップかエリアかのフラグ
		int							m_BgmType;			///< 0x0B:BGM
		std::string					m_BgmName;			///< 0x0C:BGM/演奏するBGMのファイル名
		int							m_BattleMapType;	///< 0x15:戦闘背景
		std::string					m_BattleMapName;	///< 0x16:戦闘背景/ファイル名
		int							m_Teleport;			///< 0x1F:テレポート
		int							m_Escape;			///< 0x20:エスケープ
		int							m_Save;				///< 0x21:セーブ
		smart_array< EnemyGroup >	m_saEnemyGroup;		///< 0x29:エンカウントする敵グループ
		int							m_EnemyEncounter;	///< 0x2C:敵出現歩数
	};
	/// 初期位置
	struct InitPosition {
		int			mapId;		///< 0x01:初期マップID
		int			x;			///< 0x02:初期位置
		int			y;			///< 0x03:初期位置
	};
	smart_array< MapInfo >		m_saMapInfo;		///< グローバル配列
	InitPosition				m_PartyPosition;	///< 0x01:パーティー初期位置
	InitPosition				m_BoatPosition;		///< 0x0B:小型船初期位置
	InitPosition				m_VesselPosition;	///< 0x15:大型船初期位置
	InitPosition				m_AirshipPosition;	///< 0x1F:飛行船初期位置
	

public:
	CRpgLmt(){}								///< コンストラクタ
	~CRpgLmt(){}							///< デストラクタ

	bool Init(const char* szDir="");		///< 初期化
};


#endif
