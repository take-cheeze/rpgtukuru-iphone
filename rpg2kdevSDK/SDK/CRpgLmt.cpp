/**
 * @file
 * @brief RPG_RT.lmt(LcfMapTree)を管理するクラス
 * @author project.kuto
 */
#include <kuto/kuto_utility.h>
#include "CRpgLmt.h"

//=============================================================================
/**
	初期化

	@param	szDir			[in] 読み込み先のディレクトリ

	@retval					データベースの読み込みに成功
	@retval					データベースの読み込みに失敗
*/
//=============================================================================
bool CRpgLmt::Init(const char* szDir)
{
	int max;
	sueLib::smart_buffer	buf;
	CRpgArray2		array2;
	std::string strFile;
	if(strlen(szDir)){
		strFile += szDir;
		strFile += "/";
	}
	strFile += "RPG_RT.lmt";

	bInit = false;

	// セーブデータじゃない
	if(!OpenFile(strFile.c_str()))		return false;

	
	// データを読み込む
	int numMap = ReadBerNumber();
	int mapIdMax = 0;
	int mapIndex = 0;
	while(!IsEof()){
		if (mapIndex == numMap) {
			break;
		}
		int row = ReadBerNumber();
		mapIdMax = kuto::max(mapIdMax, row);
		//kuto_printf("row=%d, %d¥n", row, mapIdMax);
		for(;;){
			int col = ReadBerNumber();
			if(col==0)	break;
			array2.SetData(row, col, ReadData());
		}
		mapIndex++;
	}
	m_saMapInfo.Resize(mapIdMax+1);
	// MapIDの羅列
	//int mapNum = ReadBerNumber();
	ReadBerNumber();
	int mapIdNum = 0;
	for (int i = 0; i < numMap+1; i++) {
		int mapId = ReadBerNumber();	// とりあえず進めるだけ
		mapIdNum++;
		//kuto_printf("id=%d, %d¥n", mapId, mapIdNum);
	}
	// 初期位置
	CRpgArray1		array1;
	while(!IsEof()){
		int col = ReadBerNumber();
		if(col==0)	break;
		array1.SetData(col, ReadData());
	}
	for (int row = 0; row <= mapIdMax; row++) {
		{
			m_saMapInfo[row].m_MapName = array2.GetString(row, 0x01);
			m_saMapInfo[row].m_ParentMapID = array2.GetNumber(row, 0x02);
			m_saMapInfo[row].m_MapCategory = array2.GetNumber(row, 0x04);
			m_saMapInfo[row].m_BgmType = array2.GetNumber(row, 0x0B);
			{
				// なぜか１次元配列
				CRpgArray1 tempArray = CRpgUtil::GetArray1(array2.GetData(row, 0x0C));
				m_saMapInfo[row].m_BgmName = tempArray.GetString(0x01);
			}
			m_saMapInfo[row].m_BattleMapType = array2.GetNumber(row, 0x15);
			m_saMapInfo[row].m_BattleMapName = array2.GetString(row, 0x16);
			m_saMapInfo[row].m_Teleport = array2.GetNumber(row, 0x1F);
			m_saMapInfo[row].m_Escape = array2.GetNumber(row, 0x20);
			m_saMapInfo[row].m_Save = array2.GetNumber(row, 0x21);
			buf = array2.GetData(row, 0x29);
			CRpgArray2 enemyArray2 = CRpgUtil::GetArray2(buf);
			int enemyMax = enemyArray2.GetMaxRow();
			m_saMapInfo[row].m_saEnemyGroup.Resize(enemyMax);
			for(int eneRow = 0; eneRow < enemyMax; eneRow++){
				m_saMapInfo[row].m_saEnemyGroup[eneRow].enemyGroupID = enemyArray2.GetNumber(eneRow + 1, 0x01);
			}
			m_saMapInfo[row].m_EnemyEncounter = array2.GetNumber(row, 0x2C, 25);
		}
	}
	
	m_PartyPosition.mapId = array1.GetNumber(0x01);
	m_PartyPosition.x = array1.GetNumber(0x02);
	m_PartyPosition.y = array1.GetNumber(0x03);
	m_BoatPosition.mapId = array1.GetNumber(0x0B);
	m_BoatPosition.x = array1.GetNumber(0x0C);
	m_BoatPosition.y = array1.GetNumber(0x0D);
	m_VesselPosition.mapId = array1.GetNumber(0x15);
	m_VesselPosition.x = array1.GetNumber(0x16);
	m_VesselPosition.y = array1.GetNumber(0x17);
	m_AirshipPosition.mapId = array1.GetNumber(0x1F);
	m_AirshipPosition.x = array1.GetNumber(0x20);
	m_AirshipPosition.y = array1.GetNumber(0x21);
	
	bInit = true;
	return true;
}

