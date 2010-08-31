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
		int8_t				scrollSpeedHorizontal;		///< 0x24:遠景/オプション/横方向にループ/自動スクロール/速度
		int8_t				scrollSpeedVertical;		///< 0x26:遠景/オプション/縦方向にループ/自動スクロール/速度
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
	std::string		m_BaseDir;

	// マップチップ(内部データの関係上、[y][x]と参照する)
	smart_array< smart_array< uint16_t > >	m_saUpper;		///< 0x47:上層マップ
	smart_array< smart_array< uint16_t > >	m_saLower;		///< 0x47:下層マップ

	kuto::Texture imgChipSet;	///< チップセット画像
	kuto::Texture imgPanorama;	///< 遠景画像
	
	kuto::Texture						chipCacheTexture_;
	kuto::SimpleArray<ChipCache>		chipCache_;

public:
	enum {
		CHIP_SIZE = 16,	///< チップサイズ
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
	
	smart_array< CRpgMapEvent >	saMapEvent;			///< マップイベントを一括格納
	
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
	bool Init(int nMapNum, std::string const& chipSet, const char* szDir="");	///< 初期化
	bool GetUpperChip(int x, int y, TextureInfo& texInfo) const;				///< 上層チップを取得
	bool GetUpperChip(int chipId, TextureInfo& texInfo) const;				///< 上層チップを取得
	bool GetLowerChip(int x, int y, int anime, TextureInfoSet& texInfoSet);	///< 下層チップを取得
	const kuto::Texture* GetChipSetTexture() const { return &imgChipSet; }
	const kuto::Texture* GetPanoramaTexture() const { return m_PanoramaInfo.enable? &imgPanorama : NULL; }
	const PanoramaInfo& GetPanoramaInfo() const { return m_PanoramaInfo; }
	void SetPanoramaInfo(const PanoramaInfo& info);
	int getLowerChipId(int x, int y) const;
	int getUpperChipId(int x, int y) const;

	int GetChipSet()	const{ return m_nChipSet; }						///< チップセット番号を取得
	int GetWidth()		const{ return m_nWidth;   }						///< マップの幅を取得
	int GetHeight()		const{ return m_nHeight;  }						///< マップの高さを取得

private:
	void LoadPanoramaTexture();
	void GetMapEvent(smart_buffer& bufSrc);								///< マップイベントを取得
	void GetOceanType(uint16_t chip, bool bOcean[4]) const;		///< チップ番号から海タイプを判別
	void GetChipPos(int nChip, bool bUpper, int* lpX, int* lpY) const;	///< チップ番号からChipSet内の座標を取得
	int GetChipCacheIndex(int chip, int animeCount) const;
	int GetEmptyChipCacheIndex() const;
};

#endif
