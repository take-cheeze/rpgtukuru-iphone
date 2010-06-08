/**
	@file
	@brief		Save??.lsd(LcfSaveData)を管理するクラス
	@author		sue445
*/
#ifndef _INC_CRPGLSD
#define _INC_CRPGLSD

#include <string>
#include <vector>
// #include <kuto/kuto_types.h>
#include "CRpgIOBase.h"



/// Save??.lsd(LcfSaveData)を管理するクラス
class CRpgLsd : public CRpgIOBase
{
public:
	enum {
		MAX_SAVEDATA = 15,		///< セーブデータの最大番号
	};
	struct DataSelectInfo
	{
		uint64_t		time;				///< 0x01	タイムスタンプ
		std::string		leaderName;			///< 0x0B	先頭のキャラの名前	
		int				leaderLevel;		///< 0x0C	先頭のキャラのLv
		int				leaderHP;			///< 0x0D	先頭のキャラのHP
		std::string		partyImage[4];		///< 0x15	1人目顔絵ファイル
		int				partyImageNo[4];	///< 0x16	1人目顔絵番号
	};
	struct SystemInfo
	{
		std::string			systemTexture;	///< 0x15	システムグラフィック
		std::vector<bool>	switches;		///< 0x20	スイッチ
		std::vector<int>	vars;			///< 0x22	変数
		bool				enableTeleport;	///< 0x51	テレポート可能か
		bool				enableEscape;	///< 0x52	エスケープ可能か
		bool				enableSave;		///< 0x53	セーブ可能か
		bool				enableMenu;		///< 0x54	メニューの呼び出し可能か
		std::string			battleMap;		///< 0x55	戦闘背景
		int					saveNum;		///< 0x59	セーブ回数
	};
	struct Location
	{
		int				mapId;		///< 0x0B	マップ番号
		int				x;			///< 0x0C	X座標
		int				y;			///< 0x0D	Y座標
	};
	
private:
	const char* GetHeader(){ return "LcfSaveData"; }	///< ファイルごとの固有ヘッダ(CRpgIOBaseからオーバーライドして使います)
	
	void analyzeDataSelectInfo(sueLib::smart_buffer& buf);
	void analyzeSystemInfo(sueLib::smart_buffer& buf);
	void analyzeLocation(sueLib::smart_buffer& buf);
public:
	DataSelectInfo		dataSelectInfo_;
	SystemInfo			systemInfo_;
	Location			location_;

public:
	CRpgLsd(){}											///< コンストラクタ
	~CRpgLsd(){}										///< デストラクタ

	bool Init(int nSaveNum, const char* szDir="");		///< 初期化
	void Save(int nSaveNum, const char* szDir="");		///< Save to file
};

#endif
