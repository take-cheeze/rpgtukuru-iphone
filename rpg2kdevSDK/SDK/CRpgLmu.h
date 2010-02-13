/**
	@file
	@brief		Map????.lmu(LcfMapUnit)を管理するクラス
	@author		sue445
*/
#ifndef _INC_CRPGLMU
#define _INC_CRPGLMU

#include "CRpgIOBase.h"
#include "CRpgEvent.h"
#include "CRpgLdb.h"
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_simple_array.h>


/// Map????.lmu(LcfMapUnit)を管理するクラス
class CRpgLmu : public CRpgIOBase
{
public:
	/// Map Scroll Type
	enum SCROLL_TYPE {
		SCROLL_NONE,
		SCROLL_LOOP_VERTICAL,
		SCROLL_LOOP_HORIZONTAL,
		SCROLL_LOOP_BOTH,
	};
	/// Panorama Image Infomation
	struct PanoramaInfo {
		bool			enable;						///< 0x1F:遠景/遠景ファイルを使用する
		bool			loopHorizontal;				///< 0x21:遠景/オプション/横方向にループ
		bool			loopVertical;				///< 0x22:遠景/オプション/縦方向にループ
		bool			scrollHorizontal;			///< 0x23:遠景/オプション/横方向にループ/自動スクロール
		bool			scrollVertical;				///< 0x25:遠景/オプション/縦方向にループ/自動スクロール
		s8				scrollSpeedHorizontal;		///< 0x24:遠景/オプション/横方向にループ/自動スクロール/速度
		s8				scrollSpeedVertical;		///< 0x26:遠景/オプション/縦方向にループ/自動スクロール/速度
		std::string		name;						///< 0x20:遠景ファイル名(初期値:"")
	};
	struct ChipCache {
		int		chipID;
		int		animeCount;
		ChipCache() : chipID(-1), animeCount(0) {}
	};
	
private:
	const char* GetHeader(){ return "LcfMapUnit"; }	///< ファイルごとの固有ヘッダ(CRpgIOBaseからオーバーライドして使います)

	int m_nChipSet;			///< 0x01:チップセットの番号(初期値:1)
	int m_nWidth;			///< 0x02:マップの幅(初期値:20)
	int m_nHeight;			///< 0x03:マップの高さ(初期値:15)
	SCROLL_TYPE		m_ScrollType;	///< 0x0B:スクロールタイプ
	PanoramaInfo	m_PanoramaInfo;	///< 遠景情報

	// マップチップ(内部データの関係上、[y][x]と参照する)
	smart_array< smart_array< unsigned short > >	m_saUpper;		///< 0x47:上層マップ
	smart_array< smart_array< unsigned short > >	m_saLower;		///< 0x47:下層マップ

	kuto::Texture imgChipSet;	///< チップセット画像
	kuto::Texture imgPanorama;	///< 遠景画像
	
	kuto::Texture						chipCacheTexture_;
	kuto::SimpleArray<ChipCache>		chipCache_;

public:
	enum {
		CHIP_SIZE = 16,	///< チップサイズ
	};
	
	enum Priority {
		kPriorityLow,		///< 0:通常キャラの下
		kPriorityNormal,	///< 1:通常キャラと重ならない
		kPriorityHigh,		///< 2:通常キャラの上
	};
	enum AnimationType {
		kAnimationTypeNormal,			///< 0:通常／足踏みなし
		kAnimationTypeNormalStepping,	///< 1:通常／足踏みあり
		kAnimationTypeDirFix,			///< 2:向き固定／足踏みなし
		kAnimationTypeDirFixStepping,	///< 3:向き固定／足踏みあり
		kAnimationTypeFix,				///< 4:グラフィック完全固定
		kAnimationTypePattern,			///< 5:4枚アニメ(右回転)
	};
	enum MoveSpeed {
		kMoveSpeed1per16,			///< 0:1/16倍速
		kMoveSpeed1per8,			///< 1:1/8倍速
		kMoveSpeed1per4,			///< 2:1/4倍速
		kMoveSpeed1per2,			///< 3:1/2倍速
		kMoveSpeedNormal,			///< 4:標準速
		kMoveSpeed2times,			///< 5:2倍速
		kMoveSpeed4times,			///< 6:4倍速
	};
	
	// 各マップイベント
	struct MAPEVENT{
		MAPEVENT(): x(0), y(0){}		///< デフォルトコンストラクタ

		std::string strName;			///< 0x01:名前
		int  x;							///< 0x02:X座標
		int  y;							///< 0x03:Y座標
		int  data;						///< 0x04:不明(´・ω・｀)

		/// ページ単位でのマップイベント
		struct MAPEVENT_PAGE
		{
			MAPEVENT_PAGE():
				nWalkPos(0), nWalkMuki(0), nWalkPattern(1), bWalkHalf(false) {}	///< コンストラクタ

			CRpgEventList	eventList;				///< イベントリスト
			
			// 歩行絵関連
			std::string	 strWalk;					///< 0x15:ファイル名

			/**
				歩行絵の時:左上から順に0〜7\n
				チップセットの時:上層チップの位置\n
				※歩行絵か上層チップかはファイル名があるかどうかで判断
			*/
			int nWalkPos;						///< 0x16:歩行絵(上層チップ)の位置

			/**
				0:上\n
				1:右\n
				2:下\n
				3:左
			*/
			int nWalkMuki;						///< 0x17:向き

			/**
				0:LEFT\n
				1:MIDDLE(初期値？)\n
				2:RIGHT
			*/
			int nWalkPattern;					///< 0x18:歩行パターン

			bool bWalkHalf;						///< 0x19:半透明かどうか
			
			/**
				0:移動しない
				1:ランダム移動
				2:上下に往復
				3:左右に往復
				4:主人公に近寄る
				5:主人公から逃げる
				6:移動ルート指定
			*/
			int	moveType;						///< 0x1F	移動タイプ
			int	moveFrequency;					///< 0x20	移動頻度	1〜8
			
			int 				priority;			///< 0x22:プライオリティタイプ (0:通常キャラの下 1:通常キャラと重ならない 2:通常キャラの上)
			bool				notCrossover;		///< 0x23:プライオリティタイプ・別のイベントと重ならない
			int					animationType;		///< 0x24:アニメーションタイプ
			int					moveSpeed;			///< 0x25:移動速度
			CRpgRoute			route;				///< 0x29:移動ルート
		};	// end of struct MAPEVENT_PAGE
		smart_array< MAPEVENT_PAGE > saPage;	///< 0x05:マップイベント
	};	// end of struct MAPEVENT
	smart_array< MAPEVENT >	saMapEvent;			///< マップイベントを一括格納
	
	/// ChipSet Info
	struct TextureInfo
	{
		kuto::Vector2			texcoord[2];
		const kuto::Texture*	texture;
	};
	struct TextureInfoSet
	{
		TextureInfo		info[8];
		int				size;
	};

public:
	CRpgLmu(){}															///< コンストラクタ
	~CRpgLmu(){}														///< デストラクタ

	bool Init(int nMapNum, const CRpgLdb& ldb, const char* szDir="");	///< 初期化
	bool GetUpperChip(int x, int y, TextureInfo& texInfo) const;				///< 上層チップを取得
	bool GetLowerChip(int x, int y, int anime, TextureInfoSet& texInfoSet);	///< 下層チップを取得
	const kuto::Texture* GetChipSetTexture() const { return &imgChipSet; }
	const kuto::Texture* GetPanoramaTexture() const { return m_PanoramaInfo.enable? &imgPanorama : NULL; }
	const PanoramaInfo& GetPanoramaInfo() const { return m_PanoramaInfo; }
	int getLowerChipId(int x, int y) const;
	int getUpperChipId(int x, int y) const;

	int GetChipSet()	const{ return m_nChipSet; }						///< チップセット番号を取得
	int GetWidth()		const{ return m_nWidth;   }						///< マップの幅を取得
	int GetHeight()		const{ return m_nHeight;  }						///< マップの高さを取得

private:
	void GetMapEvent(smart_buffer& bufSrc);								///< マップイベントを取得
	void GetOceanType(unsigned short chip, bool bOcean[4]) const;		///< チップ番号から海タイプを判別
	void GetChipPos(int nChip, bool bUpper, int* lpX, int* lpY) const;	///< チップ番号からChipSet内の座標を取得
	int GetChipCacheIndex(int chip, int animeCount) const;
	int GetEmptyChipCacheIndex() const;
};

#endif
