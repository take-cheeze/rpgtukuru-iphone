/**
	@file
	@brief		Map????.lmu(LcfMapUnit)を管理するクラス
	@author		sue445
*/
#include "CRpgLmu.h"
#include "CRpgEventStream.h"
#include <kuto/kuto_utility.h>


//=============================================================================
/**
	初期化

	@param	nMapNum			[in] マップファイル番号(1〜)
	@param	ldb				[in] あらかじめ読み込んだデータベース
	@param	szDir			[in] 読み込み先のディレクトリ

	@retval	true			マップファイルの読み込みに成功
	@retval	false			マップファイルの読み込みに失敗
*/
//=============================================================================
bool CRpgLmu::Init(int nMapNum, const CRpgLdb& ldb, const char* szDir)
{
	int type;
	char file[256];
	smart_buffer buf;

	// マップの番号がおかしい
	if(nMapNum<1)			return false;

	// ldbが初期化されていない
	if(!ldb.IsInit())		return false;

	sprintf(file, "Map%04d.lmu", nMapNum);

	bInit = false;
	if(strlen(szDir)){
		m_BaseDir = szDir;
		m_BaseDir += "/";
	}
	std::string strFile = m_BaseDir + file;

	// セーブデータじゃない
	if(!OpenFile(strFile.c_str()))		return false;

	// 初期値(lmu内で定義されてない場合はこれが採用されます)
	m_nChipSet = 1;
	m_nWidth   = 20;
	m_nHeight  = 15;
	m_ScrollType = SCROLL_LOOP_BOTH;
	m_PanoramaInfo.enable = false;
	m_PanoramaInfo.name = "";
	m_PanoramaInfo.loopHorizontal = false;
	m_PanoramaInfo.loopVertical = false;
	m_PanoramaInfo.scrollHorizontal = false;
	m_PanoramaInfo.scrollVertical = false;
	m_PanoramaInfo.scrollSpeedHorizontal = false;
	m_PanoramaInfo.scrollSpeedVertical = false;

	// データを読み込む
	while(!IsEof()){
		type = ReadBerNumber();
		buf = ReadData();

		switch(type){
		case 0x01:		// チップセット
			m_nChipSet = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x02:		// 幅
			m_nWidth = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x03:		// 高さ
			m_nHeight = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x0B:		// Scroll
			m_ScrollType = (SCROLL_TYPE)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x1F:		// 遠景/遠景ファイルを使用する
			m_PanoramaInfo.enable = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x20:		// 遠景ファイル名
			m_PanoramaInfo.name = kuto::sjis2utf8(std::string(buf.GetPtr(), buf.GetSize()));
			break;
		case 0x21:		// 遠景/オプション/横方向にループ
			m_PanoramaInfo.loopHorizontal = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x22:		// 遠景/オプション/縦方向にループ
			m_PanoramaInfo.loopVertical = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x23:		// 遠景/オプション/横方向にループ/自動スクロール
			m_PanoramaInfo.scrollHorizontal = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x24:		// 遠景/オプション/横方向にループ/自動スクロール/速度
			m_PanoramaInfo.scrollSpeedHorizontal = (int8_t)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x25:		// 遠景/オプション/縦方向にループ/自動スクロール
			m_PanoramaInfo.scrollVertical = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x26:		// 遠景/オプション/縦方向にループ/自動スクロール/速度
			m_PanoramaInfo.scrollSpeedVertical = (int8_t)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x47:		// 下層マップ
			BufferToArray2(m_saLower, buf, m_nWidth, m_nHeight);
			break;
		case 0x48:		// 上層マップ
			BufferToArray2(m_saUpper, buf, m_nWidth, m_nHeight);
			break;
		case 0x51:		// マップイベント部
			GetMapEvent(buf);
			break;
		}
	}

	// チップセットをロード
	strFile = m_BaseDir + "/ChipSet/";
	strFile += ldb.saChipSet[m_nChipSet].strFile;
	CRpgUtil::LoadImage(imgChipSet, strFile, true);
	if (imgChipSet.getWidth() != imgChipSet.getOrgWidth()) {
		const int CACHE_IMAGE_WIDTH = 256;
		const int CACHE_IMAGE_HEIGHT = 256;
		const int CACHE_SIZE = (CACHE_IMAGE_WIDTH / 16) * (CACHE_IMAGE_HEIGHT / 16);
		
		char* data = new char[CACHE_IMAGE_WIDTH * CACHE_IMAGE_HEIGHT * 4];
		chipCacheTexture_.loadFromMemory(data, CACHE_IMAGE_WIDTH, CACHE_IMAGE_HEIGHT, CACHE_IMAGE_WIDTH, CACHE_IMAGE_HEIGHT, GL_RGBA);
		chipCache_.allocate(CACHE_SIZE);
	}

	// 遠景をロード
	LoadPanoramaTexture();

	bInit = true;
	return true;
}
bool CRpgLmu::Init(int nMapNum, std::string const& chipSet, const char* szDir)
{
	int type;
	char file[256];
	smart_buffer buf;

	// マップの番号がおかしい
	if(nMapNum<1)			return false;

	sprintf(file, "Map%04d.lmu", nMapNum);

	bInit = false;
	if(strlen(szDir)){
		m_BaseDir = szDir;
		m_BaseDir += "/";
	}
	std::string strFile = m_BaseDir + file;

	// セーブデータじゃない
	if(!OpenFile(strFile.c_str()))		return false;

	// 初期値(lmu内で定義されてない場合はこれが採用されます)
	m_nChipSet = 1;
	m_nWidth   = 20;
	m_nHeight  = 15;
	m_ScrollType = SCROLL_LOOP_BOTH;
	m_PanoramaInfo.enable = false;
	m_PanoramaInfo.name = "";
	m_PanoramaInfo.loopHorizontal = false;
	m_PanoramaInfo.loopVertical = false;
	m_PanoramaInfo.scrollHorizontal = false;
	m_PanoramaInfo.scrollVertical = false;
	m_PanoramaInfo.scrollSpeedHorizontal = false;
	m_PanoramaInfo.scrollSpeedVertical = false;

	// データを読み込む
	while(!IsEof()){
		type = ReadBerNumber();
		buf = ReadData();

		switch(type){
		case 0x01:		// チップセット
			m_nChipSet = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x02:		// 幅
			m_nWidth = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x03:		// 高さ
			m_nHeight = CRpgUtil::GetBerNumber(buf);
			break;
		case 0x0B:		// Scroll
			m_ScrollType = (SCROLL_TYPE)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x1F:		// 遠景/遠景ファイルを使用する
			m_PanoramaInfo.enable = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x20:		// 遠景ファイル名
			m_PanoramaInfo.name = kuto::sjis2utf8(std::string(buf.GetPtr(), buf.GetSize()));
			break;
		case 0x21:		// 遠景/オプション/横方向にループ
			m_PanoramaInfo.loopHorizontal = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x22:		// 遠景/オプション/縦方向にループ
			m_PanoramaInfo.loopVertical = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x23:		// 遠景/オプション/横方向にループ/自動スクロール
			m_PanoramaInfo.scrollHorizontal = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x24:		// 遠景/オプション/横方向にループ/自動スクロール/速度
			m_PanoramaInfo.scrollSpeedHorizontal = (int8_t)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x25:		// 遠景/オプション/縦方向にループ/自動スクロール
			m_PanoramaInfo.scrollVertical = (bool)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x26:		// 遠景/オプション/縦方向にループ/自動スクロール/速度
			m_PanoramaInfo.scrollSpeedVertical = (int8_t)CRpgUtil::GetBerNumber(buf);
			break;
		case 0x47:		// 下層マップ
			BufferToArray2(m_saLower, buf, m_nWidth, m_nHeight);
			break;
		case 0x48:		// 上層マップ
			BufferToArray2(m_saUpper, buf, m_nWidth, m_nHeight);
			break;
		case 0x51:		// マップイベント部
			GetMapEvent(buf);
			break;
		}
	}

	// チップセットをロード
	strFile.assign(m_BaseDir).append("/ChipSet/").append(chipSet);
	if( !CRpgUtil::LoadImage(imgChipSet, strFile, true) ) kuto_assert(false);
	if (imgChipSet.getWidth() != imgChipSet.getOrgWidth()) {
		const int CACHE_IMAGE_WIDTH = 256;
		const int CACHE_IMAGE_HEIGHT = 256;
		const int CACHE_SIZE = (CACHE_IMAGE_WIDTH / 16) * (CACHE_IMAGE_HEIGHT / 16);
		
		char* data = new char[CACHE_IMAGE_WIDTH * CACHE_IMAGE_HEIGHT * 4];
		chipCacheTexture_.loadFromMemory(data, CACHE_IMAGE_WIDTH, CACHE_IMAGE_HEIGHT, CACHE_IMAGE_WIDTH, CACHE_IMAGE_HEIGHT, GL_RGBA);
		chipCache_.allocate(CACHE_SIZE);
	}

	// 遠景をロード
	LoadPanoramaTexture();

	bInit = true;
	return true;
}

void CRpgLmu::SetPanoramaInfo(const PanoramaInfo& info)
{
	m_PanoramaInfo = info;
	LoadPanoramaTexture();
}

void CRpgLmu::LoadPanoramaTexture()
{
	if (m_PanoramaInfo.enable) {
		std::string strFile = m_BaseDir + "Panorama/";
		strFile += m_PanoramaInfo.name;
		CRpgUtil::LoadImage(imgPanorama, strFile, false);
	} else {
		imgPanorama.destroy();
	}
}


//=============================================================================
/**
	マップイベントデータを取得

	@param	bufSrc		[in] 元データ
*/
//=============================================================================
void CRpgLmu::GetMapEvent(smart_buffer& bufSrc)
{
	CRpgArray2	array;
	smart_buffer buf;


	array = CRpgUtil::GetArray2(bufSrc);
	int max_event = array.GetMaxRow();


	// 開始番号が1なので配列要素数も1増やしておいた方がやりやすい
	saMapEvent.Resize(max_event+1);
	for(int i = 1; i <= max_event; i++){
		saMapEvent[i].strName	= array.GetString(i, 0x01);
		saMapEvent[i].x			= array.GetNumber(i, 0x02);
		saMapEvent[i].y			= array.GetNumber(i, 0x03);
		saMapEvent[i].data		= array.GetNumber(i, 0x04);

		// マップイベントをページごとに分割
		CRpgArray2	array2a = CRpgUtil::GetArray2(array.GetData(i, 0x05));
		int max_page = array2a.GetMaxRow();
		saMapEvent[i].saPage.Resize(max_page+1);

		for(int j = 1; j <= max_page; j++){
			// イベント出現条件
			CRpgArray1 array1a = CRpgUtil::GetArray1(array2a.GetData(j, 0x02));
			saMapEvent[i].saPage[j].eventList.condition.nFlag		= array1a.GetNumber(0x01);
			saMapEvent[i].saPage[j].eventList.condition.nSw1		= array1a.GetNumber(0x02, 1);
			saMapEvent[i].saPage[j].eventList.condition.nSw2		= array1a.GetNumber(0x03, 1);
			saMapEvent[i].saPage[j].eventList.condition.nVarNum		= array1a.GetNumber(0x04, 1);
			saMapEvent[i].saPage[j].eventList.condition.nVarOver	= array1a.GetNumber(0x05);
			saMapEvent[i].saPage[j].eventList.condition.nItem		= array1a.GetNumber(0x06, 1);
			saMapEvent[i].saPage[j].eventList.condition.nChara		= array1a.GetNumber(0x07, 1);
			saMapEvent[i].saPage[j].eventList.condition.nTimer		= array1a.GetNumber(0x08);

			saMapEvent[i].saPage[j].strWalk			= array2a.GetString(j, 0x15, "");
			saMapEvent[i].saPage[j].nWalkPos		= array2a.GetNumber(j, 0x16);
			saMapEvent[i].saPage[j].nWalkMuki		= array2a.GetNumber(j, 0x17);
			saMapEvent[i].saPage[j].nWalkPattern	= array2a.GetNumber(j, 0x18, 1);
			saMapEvent[i].saPage[j].bWalkHalf		= array2a.GetFlag(  j, 0x19, 1);
			saMapEvent[i].saPage[j].moveType		= array2a.GetNumber(j, 0x1F, 0);
			saMapEvent[i].saPage[j].moveFrequency	= array2a.GetNumber(j, 0x20, 3);
			saMapEvent[i].saPage[j].eventList.condition.nStart			= array2a.GetNumber(j, 0x21, 1);

			saMapEvent[i].saPage[j].priority		= array2a.GetNumber(j, 0x22);
			saMapEvent[i].saPage[j].notCrossover	= array2a.GetFlag(j, 0x23);
			saMapEvent[i].saPage[j].animationType	= array2a.GetNumber(j, 0x24);
			saMapEvent[i].saPage[j].moveSpeed		= array2a.GetNumber(j, 0x25, kMoveSpeed1per2);
			
			array1a = CRpgUtil::GetArray1(array2a.GetData(j, 0x29));
			saMapEvent[i].saPage[j].route.commands.resize(array1a.GetNumber(0x0B));
			buf = array1a.GetData(0x0C);
			CRpgIOBase comSt;
			if (comSt.OpenFromMemory(buf, buf.GetSize())){
				for (unsigned int iRoute = 0; iRoute < saMapEvent[i].saPage[j].route.commands.size(); iRoute++) {
					saMapEvent[i].saPage[j].route.commands[iRoute] = comSt.ReadBerNumber();
				}
			}
			saMapEvent[i].saPage[j].route.repeat = array1a.GetFlag(0x15, 1);
			saMapEvent[i].saPage[j].route.ignore = array1a.GetFlag(0x16);			
			
			// イベントデータを1行ずつvectorに入れる
			buf = array2a.GetData(j, 0x34);
			CRpgEventStream st;
			if(st.OpenFromMemory(buf, buf.GetSize())){
				CRpgEvent event;
				while(st.ReadEvent(event)){
					saMapEvent[i].saPage[j].eventList.events.push_back(event);
				}
			}
		}
	}
}

//=============================================================================
/**
	(x,y)座標の上層チップのマップチップを取得する

	@param	x				[in] x座標
	@param	y				[in] y座標
	@param	img				[out] 取得した画像

	@retval	true			チップの取得に成功
	@retval	false			チップの取得に失敗
*/
//=============================================================================
bool CRpgLmu::GetUpperChip(int x, int y, TextureInfo& texInfo) const
{
	if(x<0 || x >= m_nWidth || y<0 || y>= m_nHeight)	return false;
	if (!m_saUpper.GetPtr()) return false;
	
	return GetUpperChip(m_saUpper[y][x], texInfo);
}

bool CRpgLmu::GetUpperChip(int chipId, TextureInfo& texInfo) const
{
	if(chipId>=10000 && chipId<10144){
		int cx, cy;
		GetChipPos(chipId, true, &cx, &cy);
		kuto::Vector2 size((float)CHIP_SIZE / imgChipSet.getWidth(), (float)CHIP_SIZE / imgChipSet.getHeight());
		texInfo.texcoord[0].set((float)(cx << 4) / imgChipSet.getWidth(), (float)(cy << 4) / imgChipSet.getHeight());
		texInfo.texcoord[1] = texInfo.texcoord[0] + size;
		texInfo.texture = &imgChipSet;
		return true;
	}
	return false;
}

//=============================================================================
/**
	(x,y)座標の下層チップのマップチップを取得する

	@param	x				[in] x座標
	@param	y				[in] y座標
	@param	anime			[in] アニメーション番号
	@param	img				[out] 取得した画像


	@retval	true			チップの取得に成功
	@retval	false			チップの取得に失敗
*/
//=============================================================================
bool CRpgLmu::GetLowerChip(int x, int y, int anime, TextureInfoSet& texInfoSet)
{
	if(x<0 || x >= m_nWidth || y<0 || y>= m_nHeight)	return false;
	if (!m_saLower.GetPtr()) return false;

	int i, cx = 18, cy = 8, base_cx = 0, base_cy = 0;
	const uint16_t chip = m_saLower[y][x];
	const int nOffset[4][2] = {
		{0, 0},		// 左上
		{8, 0},		// 右上
		{0, 8},		// 左下
		{8, 8},		// 右下
	};
	//img.Create(CHIP_SIZE, CHIP_SIZE);
	//img.CopyPalette(imgChipSet);

	//if(x==2 && y==6){
	//	printf("%02X\n", chip);
	//}

	kuto::Vector2 size((float)CHIP_SIZE / imgChipSet.getWidth(), (float)CHIP_SIZE / imgChipSet.getHeight());
	kuto::Vector2 cacheSize((float)CHIP_SIZE / chipCacheTexture_.getWidth(), (float)CHIP_SIZE / chipCacheTexture_.getHeight());
	texInfoSet.size = 0;
	if(chip<3000){
		int cacheIndex = GetChipCacheIndex(chip, anime);
		if (cacheIndex >= 0) {
			cx = cacheIndex % (chipCacheTexture_.getWidth() / CHIP_SIZE);
			cy = cacheIndex / (chipCacheTexture_.getWidth() / CHIP_SIZE);
			texInfoSet.info[0].texcoord[0].set((float)(cx << 4) / chipCacheTexture_.getWidth(), (float)(cy << 4) / chipCacheTexture_.getHeight());
			texInfoSet.info[0].texcoord[1] = texInfoSet.info[0].texcoord[0] + cacheSize;
			texInfoSet.info[0].texture = &chipCacheTexture_;
			texInfoSet.size++;
			return true;
		}
		cacheIndex = GetEmptyChipCacheIndex();
		int destX = 0, destY = 0;
		if (cacheIndex >= 0) {
			destX = cacheIndex % (chipCacheTexture_.getWidth() / CHIP_SIZE) * CHIP_SIZE;
			destY = cacheIndex / (chipCacheTexture_.getWidth() / CHIP_SIZE) * CHIP_SIZE;
		}
		
		bool ocean_flag[4];

		/*
			0:浅瀬
			1:四隅に深海と境界を持つ浅瀬
			2:四隅に浅瀬と境界を持つ深海
			3:深海
		*/
		int  ocean_type[4];

		GetOceanType(chip, ocean_flag);

		// 海のタイプを調べる
		// 0〜
		if(chip<1000){		// 	海岸線Ａ１(普通の陸)
			base_cx = 0;
			base_cy = 0;
			for(i = 0; i < 4; i++){
				if(!ocean_flag[i])	ocean_type[i] = 0;
				else				ocean_type[i] = 1;
			}
		}
		// 1000〜
		else if(chip<2000){	// 海岸線Ａ２(雪)
			base_cx = 3;
			base_cy = 0;
			for(i = 0; i < 4; i++){
				if(!ocean_flag[i])	ocean_type[i] = 0;
				else				ocean_type[i] = 1;
			}
		}
		// 2000〜
		else if(chip<3000){	// 海岸線Ａ１(普通の陸)
			base_cx = 0;
			base_cy = 0;
			for(i = 0; i < 4; i++){
				if(!ocean_flag[i])	ocean_type[i] = 3;
				else				ocean_type[i] = 2;
			}
		}

		// ベースとなる海をセット
		for(i = 0; i < 4; i++){
			if (cacheIndex >= 0) {
				int x = ((anime % 3)      <<4) + nOffset[i][0];
				int y = ((ocean_type[i]+4)<<4) + nOffset[i][1];
				for (int j = 0; j < 8; j++) {
					char* destPtr = chipCacheTexture_.getData() + (destX + nOffset[i][0]) * 4 + ((j + destY + nOffset[i][1]) * chipCacheTexture_.getWidth() * 4);
					const char* srcPtr = imgChipSet.getData() + x * 4 + ((j + y) * imgChipSet.getWidth() * 4);
					std::memcpy(destPtr, srcPtr, 8 * 4);
				}
			} else {
				int x = ((anime % 3)      <<4) + nOffset[i][0];
				int y = ((ocean_type[i]+4)<<4) + nOffset[i][1];
				//img.Blt(imgChipSet, nOffset[i][0], nOffset[i][1], x, y, CHIP_SIZE>>1, CHIP_SIZE>>1, false);
				texInfoSet.info[i].texcoord[0].set((float)(x) / imgChipSet.getWidth(), (float)(y) / imgChipSet.getHeight());
				texInfoSet.info[i].texcoord[1] = texInfoSet.info[i].texcoord[0] + size * 0.5f;
				texInfoSet.info[i].texture = &imgChipSet;
				texInfoSet.size++;
			}
		}

		/*
			00:海・海岸線無し

			01:海岸線・左上
			02:海岸線・右上
			04:海岸線・右下
			08:海岸線・左下

			10:海岸線・左
			11:海岸線・左＋右上
			12:海岸線・左　　　＋右下
			13:海岸線・左＋右上＋右下

			14:海岸線・上
			15:海岸線・上＋右下
			16:海岸線・上　　　＋左下
			17:海岸線・上＋右下＋左下

			18:海岸線・右
			19:海岸線・右＋左下
			1A:海岸線・右　　　＋左上
			1B:海岸線・右＋左下＋左上

			1C:海岸線・下
			1D:海岸線・下＋左上
			1E:海岸線・下　　　＋右上
			1F:海岸線・下＋左上＋右上

			20:海岸線・左＋右
			21:海岸線・上＋下

			ＡＢ
			ＣＤ
			22:海岸線・ＡＢＣ
			23:海岸線・ＡＢＣ　＋右下
			24:海岸線・ＡＢ　Ｄ
			25:海岸線・ＡＢ　Ｄ＋左下
			26:海岸線・　ＢＣＤ
			27:海岸線・　ＢＣＤ＋左上
			28:海岸線・Ａ　ＣＤ
			29:海岸線・Ａ　ＣＤ＋右上
			2A:海岸線・ＣＡＢＤ
			2B:海岸線・ＢＡＣＤ
			2C:海岸線・ＡＣＤＢ
			2D:海岸線・ＡＢＤＣ
			2E:海岸線・丸(ＡＢＣＤ)
		*/
		int chip2 = chip%50;

		enum{
			A,	// 四辺
			B,	// 上下
			C,	// 左右
			D,	// 四隅

			OCEAN,	// 海岸線なし(下の海チップをそのまま採用)
		};
		int nChip[4] = {OCEAN, OCEAN, OCEAN, OCEAN};

		if(chip2<0x10){
			bool flag[8];
			ByteToBool((unsigned char)chip2, flag);
			if(flag[0])		nChip[0] = D;	// 左上
			if(flag[1])		nChip[1] = D;	// 右上
			if(flag[2])		nChip[3] = D;	// 右下
			if(flag[3])		nChip[2] = D;	// 左下
		}
		// 0x10〜
		else if(chip2<0x14){
			nChip[0] = nChip[2] = B;
			if(chip2 & 0x01)	nChip[1] = D;
			if(chip2 & 0x02)	nChip[3] = D;
		}
		// 0x14〜
		else if(chip2<0x18){
			nChip[0] = nChip[1] = C;
			if(chip2 & 0x01)	nChip[3] = D;
			if(chip2 & 0x02)	nChip[2] = D;
		}
		// 0x18〜
		else if(chip2<0x1C){
			nChip[1] = nChip[3] = B;
			if(chip2 & 0x01)	nChip[2] = D;
			if(chip2 & 0x02)	nChip[0] = D;
		}
		// 0x1C〜
		else if(chip2<0x20){
			nChip[2] = nChip[3] = C;
			if(chip2 & 0x01)	nChip[0] = D;
			if(chip2 & 0x02)	nChip[1] = D;
		}
		else if(chip2==0x20){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = B;
		}
		else if(chip2==0x21){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = C;
		}
		else if(chip2<=0x23){
			nChip[0] = A;
			nChip[2] = B;
			nChip[1] = C;
			if(chip2 & 0x01)	nChip[3] = D;
		}
		else if(chip2<=0x25){
			nChip[1] = A;
			nChip[3] = B;
			nChip[0] = C;
			if(chip2 & 0x01)	nChip[2] = D;
		}
		else if(chip2<=0x27){
			nChip[3] = A;
			nChip[1] = B;
			nChip[2] = C;
			if(chip2 & 0x01)	nChip[0] = D;
		}
		else if(chip2<=0x29){
			nChip[2] = A;
			nChip[0] = B;
			nChip[3] = C;
			if(chip2 & 0x01)	nChip[1] = D;
		}
		else if(chip2==0x2A){
			nChip[0] = nChip[1] = A;
			nChip[2] = nChip[3] = B;
		}
		else if(chip2==0x2B){
			nChip[0] = nChip[2] = A;
			nChip[1] = nChip[3] = C;
		}
		else if(chip2==0x2C){
			nChip[2] = nChip[3] = A;
			nChip[0] = nChip[1] = B;
		}
		else if(chip2==0x2D){
			nChip[1] = nChip[3] = A;
			nChip[0] = nChip[2] = C;
		}
	//	else if(chip2==0x2E){
		else{
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = A;
		}

		// 海岸線を描画
		for(i = 0; i < 4; i++){
			if (cacheIndex >= 0) {
				if(nChip[i] != OCEAN){
					int x = ((base_cx + anime % 3)<<4) + nOffset[i][0];
					int y = ((base_cy + nChip[i]) <<4) + nOffset[i][1];
					for (int j = 0; j < 8; j++) {
						char* destPtr = chipCacheTexture_.getData() + (destX + nOffset[i][0]) * 4 + ((j + destY + nOffset[i][1]) * chipCacheTexture_.getWidth() * 4);
						const char* srcPtr = imgChipSet.getData() + x * 4 + ((j + y) * imgChipSet.getWidth() * 4);
						std::memcpy(destPtr, srcPtr, 8 * 4);
					}
				}
			} else {
				if(nChip[i] != OCEAN){
					int x = ((base_cx + anime % 3)<<4) + nOffset[i][0];
					int y = ((base_cy + nChip[i]) <<4) + nOffset[i][1];
					//img.Blt(imgChipSet, nOffset[i][0], nOffset[i][1], x, y, CHIP_SIZE>>1, CHIP_SIZE>>1, false);
					texInfoSet.info[i+4].texcoord[0].set((float)(x) / imgChipSet.getWidth(), (float)(y) / imgChipSet.getHeight());
					texInfoSet.info[i+4].texcoord[1] = texInfoSet.info[i+4].texcoord[0] + size * 0.5f;
					texInfoSet.info[i+4].texture = &imgChipSet;
				} else {
					texInfoSet.info[i+4].texture = NULL;
				}
				texInfoSet.size++;
			}
		}

		if (cacheIndex >= 0) {
			chipCacheTexture_.updateImage();
			texInfoSet.info[0].texcoord[0].set((float)(destX) / chipCacheTexture_.getWidth(), (float)(destY) / chipCacheTexture_.getHeight());
			texInfoSet.info[0].texcoord[1] = texInfoSet.info[0].texcoord[0] + cacheSize;
			texInfoSet.info[0].texture = &chipCacheTexture_;
			texInfoSet.size++;
			chipCache_[cacheIndex].chipID = chip;
			chipCache_[cacheIndex].animeCount = anime;
			return true;
		}
		return true;
	}
	// 3000〜(3028)
	else if(chip<3050){
		// Ｃ１
		cx = 3;
		cy = 4;
		cy += anime % 4;
	}
	// 3050〜(3078)
	else if(chip<3100){
		// Ｃ２
		cx = 4;
		cy = 4;
		cy += anime % 4;
	}
	// 3100〜(3128)
	else if(chip<3150){
		// Ｃ３
		cx = 5;
		cy = 4;
		cy += anime % 4;
	}
	// 3150〜
	else if(chip<3200){
		// Ｄ１
		cx = 0;
		cy = 8;
	}
	// 3200〜
	else if(chip<3250){
		// Ｄ２
		cx = 3;
		cy = 8;
	}
	// 3250〜
	else if(chip<3300){
		// Ｄ３
		cx = 0;
		cy = 12;
	}
	// 3300〜
	else if(chip<3350){
		// Ｄ４
		cx = 3;
		cy = 12;
	}
	// 3350〜
	else if(chip<3400){
		// Ｄ５
		cx = 6;
		cy = 0;
	}
	// 3400〜
	else if(chip<3450){
		// Ｄ６
		cx = 9;
		cy = 0;
	}
	// 3450〜
	else if(chip<3500){
		// Ｄ７
		cx = 6;
		cy = 4;
	}
	// 3500〜
	else if(chip<3550){
		// Ｄ８
		cx = 9;
		cy = 4;
	}
	// 3550〜
	else if(chip<3600){
		// Ｄ９
		cx = 6;
		cy = 8;
	}
	// 3600〜
	else if(chip<3650){
		// Ｄ１０
		cx = 9;
		cy = 8;
	}
	// 3650〜
	else if(chip<3700){
		// Ｄ１１
		cx = 6;
		cy = 12;
	}
	// 3700〜
	else if(chip<3750){
		// Ｄ１２
		cx = 9;
		cy = 12;
	}
	// 3750〜
	else if(chip<4000);		// たぶん存在しない
	// 4000〜
	else if(chip<5000){
		// 相対位置(ア〜エ→A〜D)
		enum{
			A,
			B,
			C,
			D7,	// Dはテンキーと対応
			D8,
			D9,
			D4,
			D5,
			D6,
			D1,
			D2,
			D3,
		};
		// 相対位置→相対座標
		static const int nOffset2[12][2] = {
			{0, 0},
			{1, 0},
			{2, 0},
			{0, 1},
			{1, 1},
			{2, 1},
			{0, 2},
			{1, 2},
			{2, 2},
			{0, 3},
			{1, 3},
			{2, 3},
		};

		// 4000〜
		if(chip<4050){
			// Ｄ１
			base_cx = 0;
			base_cy = 8;
		}
		// 4050〜
		else if(chip<4100){
			// Ｄ２
			base_cx = 3;
			base_cy = 8;
		}
		// 4100〜
		else if(chip<4150){
			// Ｄ３
			base_cx = 0;
			base_cy = 12;
		}
		// 4150〜
		else if(chip<4200){
			// Ｄ４
			base_cx = 3;
			base_cy = 12;
		}
		// 4200〜
		else if(chip<4250){
			// Ｄ５
			base_cx = 6;
			base_cy = 0;
		}
		// 4250〜
		else if(chip<4300){
			// Ｄ６
			base_cx = 9;
			base_cy = 0;
		}
		// 4300〜
		else if(chip<4350){
			// Ｄ７
			base_cx = 6;
			base_cy = 4;
		}
		// 4350〜
		else if(chip<4400){
			// Ｄ８
			base_cx = 9;
			base_cy = 4;
		}
		// 4400〜
		else if(chip<4450){
			// Ｄ９
			base_cx = 6;
			base_cy = 8;
		}
		// 4450〜
		else if(chip<4500){
			// Ｄ１０
			base_cx = 9;
			base_cy = 8;
		}
		// 4500〜
		else if(chip<4550){
			// Ｄ１１
			base_cx = 6;
			base_cy = 12;
		}
		// 4550〜
		else if(chip<4600){
			// Ｄ１２
			base_cx = 9;
			base_cy = 12;
		}

		/*
			00:中央部

			01:四隅・左上
			02:四隅・右上
			04:四隅・右下
			08:四隅・左下

			10:集合・左
			11:集合・左＋右上
			12:集合・左　　　＋右下
			13:集合・左＋右上＋右下

			14:集合・上
			15:集合・上＋右下
			16:集合・上　　　＋左下
			17:集合・上＋右下＋左下

			18:集合・右
			19:集合・右＋左下
			1A:集合・右　　　＋左上
			1B:集合・右＋左下＋左上

			1C:集合・下
			1D:集合・下＋左上
			1E:集合・下　　　＋右上
			1F:集合・下＋左上＋右上

			20:集合・左＋右
			21:集合・上＋下

			// ここから地形チップ独自規格∧||∧
			22:集合・左上
			23:集合・左上＋四隅・右下

			24:集合・右上
			25:集合・右上＋四隅・左下

			26:集合・右下
			27:集合・右下＋四隅・左上

			28:集合・左下
			29:集合・左下＋四隅・右上

			2A:集合(左上＋右上)
			2B:集合(左上＋左下)
			2C:集合(左下＋右下)
			2D:集合(右上＋右下)
			2E:集合(左上＋右上＋左下＋右下)
		*/
		int nChip[4] = {D5, D5, D5, D5};

		int chip2 = chip % 50;
		// 0x00〜
		if(chip2<0x10){
			//bool flag[8];
			//ByteToBool((unsigned char)chip2, flag);
			//for(i = 0; i < 4; i++){
			//	if(flag[i])		nChip[i] = C;
			//}
			if (chip2 & 0x01) nChip[0] = C;
			if (chip2 & 0x02) nChip[1] = C;
			if (chip2 & 0x04) nChip[3] = C;
			if (chip2 & 0x08) nChip[2] = C;
		}
		// 0x10〜
		else if(chip2<0x14){
			nChip[0] = nChip[2] = D4;
			if(chip2 & 0x01)	nChip[1] = C;
			if(chip2 & 0x02)	nChip[3] = C;
		}
		// 0x14〜
		else if(chip2<0x18){
			nChip[0] = nChip[1] = D8;
			if(chip2 & 0x01)	nChip[3] = C;
			if(chip2 & 0x02)	nChip[2] = C;
		}
		// 0x18〜
		else if(chip2<0x1C){
			nChip[1] = nChip[3] = D6;
			if(chip2 & 0x01)	nChip[2] = C;
			if(chip2 & 0x02)	nChip[0] = C;
		}
		// 0x1C〜
		else if(chip2<0x20){
			nChip[2] = nChip[3] = D2;
			if(chip2 & 0x01)	nChip[0] = C;
			if(chip2 & 0x02)	nChip[1] = C;
		}
		else if(chip2==0x20){
			nChip[0] = nChip[2] = D4;
			nChip[1] = nChip[3] = D6;
		}
		else if(chip2==0x21){
			nChip[0] = nChip[1] = D8;
			nChip[2] = nChip[3] = D2;
		}
		else if(chip2<=0x23){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = D7;
			if(chip2 & 0x01)	nChip[3] = C;
		}
		else if(chip2<=0x25){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = D9;
			if(chip2 & 0x01)	nChip[2] = C;
		}
		else if(chip2<=0x27){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = D3;
			if(chip2 & 0x01)	nChip[0] = C;
		}
		else if(chip2<=0x29){
			nChip[0] = nChip[1] = nChip[2] = nChip[3] = D1;
			if(chip2 & 0x01)	nChip[1] = C;
		}
		else if(chip2==0x2A){
			nChip[0] = nChip[2] = D7;
			nChip[1] = nChip[3] = D9;
		}
		else if(chip2==0x2B){
			nChip[0] = nChip[1] = D7;
			nChip[2] = nChip[3] = D1;
		}
		else if(chip2==0x2C){
			nChip[0] = nChip[2] = D1;
			nChip[1] = nChip[3] = D3;
		}
		else if(chip2==0x2D){
			nChip[0] = nChip[1] = D9;
			nChip[2] = nChip[3] = D3;
		}
	//	else if(chip2==0x2E){
		else{
			nChip[0] = D7;
			nChip[1] = D9;
			nChip[2] = D1;
			nChip[3] = D3;
		}

		// チップを描画
		int cacheIndex = GetChipCacheIndex(chip, 0);
		if (cacheIndex >= 0) {
			cx = cacheIndex % (chipCacheTexture_.getWidth() / CHIP_SIZE);
			cy = cacheIndex / (chipCacheTexture_.getWidth() / CHIP_SIZE);
			texInfoSet.info[0].texcoord[0].set((float)(cx << 4) / chipCacheTexture_.getWidth(), (float)(cy << 4) / chipCacheTexture_.getHeight());
			texInfoSet.info[0].texcoord[1] = texInfoSet.info[0].texcoord[0] + cacheSize;
			texInfoSet.info[0].texture = &chipCacheTexture_;
			texInfoSet.size++;
			return true;
		}
		cacheIndex = GetEmptyChipCacheIndex();
		int destX = 0, destY = 0;
		if (cacheIndex >= 0) {
			destX = cacheIndex % (chipCacheTexture_.getWidth() / CHIP_SIZE) * CHIP_SIZE;
			destY = cacheIndex / (chipCacheTexture_.getWidth() / CHIP_SIZE) * CHIP_SIZE;
		}
		
		for(i = 0; i < 4; i++){
			if (cacheIndex >= 0) {
				int x = ( (base_cx+nOffset2[nChip[i]][0])<<4 ) + nOffset[i][0];
				int y = ( (base_cy+nOffset2[nChip[i]][1])<<4 ) + nOffset[i][1];
				for (int j = 0; j < 8; j++) {
					char* destPtr = chipCacheTexture_.getData() + (destX + nOffset[i][0]) * 4 + ((j + destY + nOffset[i][1]) * chipCacheTexture_.getWidth() * 4);
					const char* srcPtr = imgChipSet.getData() + x * 4 + ((j + y) * imgChipSet.getWidth() * 4);
					std::memcpy(destPtr, srcPtr, 8 * 4);
				}
			} else {
				int x = ( (base_cx+nOffset2[nChip[i]][0])<<4 ) + nOffset[i][0];
				int y = ( (base_cy+nOffset2[nChip[i]][1])<<4 ) + nOffset[i][1];
				//img.Blt(imgChipSet, nOffset[i][0], nOffset[i][1], x, y, CHIP_SIZE>>1, CHIP_SIZE>>1, false);
				texInfoSet.info[i].texcoord[0].set((float)(x) / imgChipSet.getWidth(), (float)(y) / imgChipSet.getHeight());
				texInfoSet.info[i].texcoord[1] = texInfoSet.info[i].texcoord[0] + size * 0.5f;
				texInfoSet.info[i].texture = &imgChipSet;
				texInfoSet.size++;
			}
		}

		if (cacheIndex >= 0) {
			chipCacheTexture_.updateImage();
			texInfoSet.info[0].texcoord[0].set((float)(destX) / chipCacheTexture_.getWidth(), (float)(destY) / chipCacheTexture_.getHeight());
			texInfoSet.info[0].texcoord[1] = texInfoSet.info[0].texcoord[0] + cacheSize;
			texInfoSet.info[0].texture = &chipCacheTexture_;
			texInfoSet.size++;
			chipCache_[cacheIndex].chipID = chip;
			chipCache_[cacheIndex].animeCount = 0;
			return true;
		}

		return true;
	}
	// 5000〜
	else{				// ブロックＥ・標準チップ
		GetChipPos(chip, false, &cx, &cy);
	}


	// cx,cyを座標として採用する(4分割しないチップ)
	//img.Blt(imgChipSet, 0, 0, cx<<4, cy<<4, CHIP_SIZE, CHIP_SIZE, false);
	texInfoSet.info[0].texcoord[0].set((float)(cx << 4) / imgChipSet.getWidth(), (float)(cy << 4) / imgChipSet.getHeight());
	texInfoSet.info[0].texcoord[1] = texInfoSet.info[0].texcoord[0] + size;
	texInfoSet.info[0].texture = &imgChipSet;
	texInfoSet.size++;
	return true;
}

//=============================================================================
/**
	海のタイプを取得(左上、右上、左下、右下の順)

	@note					
							

	@param	chip			[in] チップ番号
	@param	bOcean			[out]	浅瀬の時(false:浅瀬／true:四隅に深海と境界を持つ浅瀬)\n
									深海の時(false:深海／true:四隅に浅瀬と境界を持つ深海)
*/
//=============================================================================
void CRpgLmu::GetOceanType(uint16_t chip, bool bOcean[4]) const
{
	unsigned int mask = 1;
	const uint16_t chip2 = chip % 800 / 50;

	for(int i = 0; i < 4; i++){
		bOcean[i] = ((chip2 & mask)!=0);
		mask <<= 1;
	}
}

//=============================================================================
/**
	チップ番号から座標を取得

	@param	nChip			[in] チップ番号
	@param	bUpper;			[in] true:上層チップ／false:下層チップ
	@param	lpX				[out] x座標
	@param	lpY				[out] y座標
*/
//=============================================================================
void CRpgLmu::GetChipPos(int nChip, bool bUpper, int* lpX, int* lpY) const
{
	// チップIDをChipSet内の相対ID(?)にする
	int chip;
	if(bUpper)	chip = nChip - 10000 + 336;		// 48*7
	else		chip = nChip - 5000  + 192;		// 48*4;

	// 横6マスのチップに換算した時の座標
	int pos_x = chip % 6;
	int pos_y = chip / 6;

	// 6x8のブロックごとに分けた時のブロックの座標
	// 算出しようとするとどうしても乗算が入ってきて遅くなりそうなのでifで分岐(汗
	int block_x, block_y;
	if     (pos_y<8){	block_x = 0;  block_y = 0; }
	else if(pos_y<16){	block_x = 0;  block_y = 8; }
	else if(pos_y<24){	block_x = 6;  block_y = 0; }
	else if(pos_y<32){	block_x = 6;  block_y = 8; }
	else if(pos_y<40){	block_x = 12; block_y = 0; }
	else if(pos_y<48){	block_x = 12; block_y = 8; }
	else if(pos_y<56){	block_x = 18; block_y = 0; }
	else if(pos_y<64){	block_x = 18; block_y = 8; }
	else if(pos_y<72){	block_x = 24; block_y = 0; }
	else			{	block_x = 24; block_y = 8; }

	*lpX = block_x +  pos_x;
	*lpY = block_y + (pos_y & 0x07);
}

int CRpgLmu::GetChipCacheIndex(int chip, int animeCount) const
{
	for (unsigned int i = 0; i < chipCache_.size(); i++) {
		if (chipCache_[i].chipID == chip && chipCache_[i].animeCount % 3 == animeCount % 3)
			return i;
	}
	return -1;
}

int CRpgLmu::GetEmptyChipCacheIndex() const
{
	for (unsigned int i = 0; i < chipCache_.size(); i++) {
		if (chipCache_[i].chipID == -1)
			return i;
	}
	return -1;
}

int CRpgLmu::getLowerChipId(int x, int y) const
{
	const uint16_t chip = m_saLower[y][x];
	if (chip < 3000) {
		return chip / 1000;
	} else if (chip < 4000) {
		return (chip - 3000) / 50 + 3;
	} else if (chip < 5000) {
		return (chip - 4000) / 50 + 6;
	} else {
		return (chip - 5000) + 18;
	}
}

int CRpgLmu::getUpperChipId(int x, int y) const
{
	const uint16_t chip = m_saUpper[y][x];
	return chip - 10000;
}



