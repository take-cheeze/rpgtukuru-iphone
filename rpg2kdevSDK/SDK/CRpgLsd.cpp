/**
	@file
	@brief		Save??.lsd(LcfSaveData)を管理するクラス
	@author		sue445
*/
#include "CRpgLsd.h"


//=============================================================================
/**
	初期化

	@param	nSaveNum		[in] セーブデータ番号(1〜15)
	@param	szDir			[in] 読み込み先のディレクトリ

	@retval					セーブデータの読み込みに成功
	@retval					セーブデータの読み込みに失敗
*/
//=============================================================================
bool CRpgLsd::Init(int nSaveNum, const char* szDir)
{
	// セーブデータの番号がおかしい
	if (nSaveNum < 1 || nSaveNum > MAX_SAVEDATA)
		return false;
	
	int type;
	sueLib::smart_buffer buf;
	char file[256];
	std::string strFile;

	bInit = false;
	sprintf(file, "Save%02d.lsd", nSaveNum);
	if(strlen(szDir)){
		strFile += szDir;
		strFile += "/";
	}
	strFile += file;

	// セーブデータじゃない
	if (!OpenFile(strFile.c_str()))
		return false;

	// データを読み込む
	while(!IsEof()){
		type = ReadBerNumber();
		buf = ReadData();

		switch(type){
		case 0x64:	// セーブデータ選択画面の情報
			analyzeDataSelectInfo(buf);
			break;
		case 0x65:	// フラグ情報など
			analyzeSystemInfo(buf);
			break;
		case 0x66:
			break;
		case 0x67:
			break;
		case 0x68:	// セーブ地点
			analyzeLocation(buf);
			break;
		case 0x69:
			break;
		case 0x6A:
			break;
		case 0x6B:
			break;
		case 0x6C:	// パーティ[キャラ番号][属性]
			break;
		case 0x6D:	// アイテム情報など
			break;
		case 0x6E:
			break;
		case 0x6F:
			break;
		case 0x70:
			break;
		case 0x71:
			break;
		case 0x72:
			break;
		}
	}

	bInit = true;
	return true;
}

void CRpgLsd::analyzeDataSelectInfo(sueLib::smart_buffer& buf)
{
	CRpgArray1 array1 = CRpgUtil::GetArray1(buf);
	dataSelectInfo_.time = *(s64*)array1.GetData(0x01).GetPtr();
	dataSelectInfo_.leaderName = array1.GetString(0x0B);
	dataSelectInfo_.leaderLevel = array1.GetNumber(0x0C);
	dataSelectInfo_.leaderHP = array1.GetNumber(0x0D);
	for (int i = 0; i < 4; i++) {
		dataSelectInfo_.partyImage[i] = array1.GetString(0x15 + i*2);
		dataSelectInfo_.partyImageNo[i] = array1.GetNumber(0x16 + i*2);
	}
}

void CRpgLsd::analyzeSystemInfo(sueLib::smart_buffer& buf)
{
	CRpgArray1 array1 = CRpgUtil::GetArray1(buf);
	systemInfo_.systemTexture = array1.GetString(0x15);
	CRpgUtil::AnalyzeDataArray(systemInfo_.switches, array1, 0x19);
	CRpgUtil::AnalyzeDataArray(systemInfo_.vars, array1, 0x21);
	systemInfo_.enableTeleport = array1.GetFlag(0x51);
	systemInfo_.enableEscape = array1.GetFlag(0x52);
	systemInfo_.enableSave = array1.GetFlag(0x53);
	systemInfo_.enableMenu = array1.GetFlag(0x54);
	systemInfo_.battleMap = array1.GetString(0x55);
	systemInfo_.saveNum = array1.GetNumber(0x59);
}

void CRpgLsd::analyzeLocation(sueLib::smart_buffer& buf)
{
	CRpgArray1 array1 = CRpgUtil::GetArray1(buf);
	location_.mapId = array1.GetNumber(0x0B);
	location_.x = array1.GetNumber(0x0C);
	location_.y = array1.GetNumber(0x0D);
}


