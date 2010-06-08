/**
	@file
	@brief		RPG_RT.ldb(LcfDataBase)を管理するクラス
	@author		sue445
*/
#include "CRpgLdb.h"
#include "CRpgEventStream.h"

//=============================================================================
/**
	初期化

	@param	szDir			[in] 読み込み先のディレクトリ

	@retval					データベースの読み込みに成功
	@retval					データベースの読み込みに失敗
*/
//=============================================================================
bool CRpgLdb::Init(const char* szDir)
{
	int type;
	sueLib::smart_buffer	buf;
	std::string strFile;
	if(strlen(szDir)){
		rootFolder_ = szDir;
		strFile += szDir;
		strFile += "/";
	}
	strFile += "RPG_RT.ldb";

	bInit = false;

	// セーブデータじゃない
	if(!OpenFile(strFile.c_str()))		return false;


	// データを読み込む
	while(!IsEof()){
		type = ReadBerNumber();
		buf = ReadData();

		switch(type){
		case 11:	// 主人公
			AnalyzePlayer(buf);
			break;
		case 12:	// 特殊技能
			AnalyzeSkill(buf);
			break;
		case 13:	// アイテム
			AnalyzeItem(buf);
			break;
		case 14:	// 敵キャラ
			AnalyzeEnemy(buf);
			break;
		case 15:	// 敵グループ
			AnalyzeEnemyGroup(buf);
			break;
		case 16:	// 地形
			AnalyzeTerrain(buf);
			break;
		case 17:	// 属性
			AnalyzeAttribute(buf);
			break;
		case 18:	// 状態
			AnalyzeCondition(buf);
			break;
		case 19:	// 戦闘アニメ
			AnalyzeBattleAnime(buf);
			break;
		case 20:	// チップセット
			AnalyzeChipSet(buf);
			break;
		case 21:	// 用語
			AnalyzeTerm(buf);
			break;
		case 22:	// システム
			AnalyzeSystem(buf);
			break;
		case 23:	// スイッチ
			AnalyzeSwitch(buf);
			break;
		case 24:	// 変数
			AnalyzeVariable(buf);
			break;
		case 25:	// コモンイベント
			AnalyzeCommonEvent(buf);
			break;
		}
	}

	bInit = true;
	return true;
}

void CRpgLdb::AnalyzePlayer(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saPlayer.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saPlayer[id].name = array2.GetString(id, 0x01);
		saPlayer[id].title = array2.GetString(id, 0x02);
		saPlayer[id].walkGraphicName = array2.GetString(id, 0x03);
		saPlayer[id].walkGraphicPos = array2.GetNumber(id, 0x04);
		saPlayer[id].walkGraphicSemi = array2.GetFlag(id, 0x05);
		saPlayer[id].startLevel = array2.GetNumber(id, 0x07, 1);
		saPlayer[id].maxLevel = array2.GetNumber(id, 0x08, 50);
		saPlayer[id].criticalEnable = array2.GetFlag(id, 0x09);
		saPlayer[id].criticalRatio = array2.GetNumber(id, 0x0A);
		saPlayer[id].faceGraphicName = array2.GetString(id, 0x0F);
		saPlayer[id].faceGraphicPos = array2.GetNumber(id, 0x10);
		saPlayer[id].twinSword = array2.GetFlag(id, 0x15);
		saPlayer[id].fixEquip = array2.GetFlag(id, 0x16);
		saPlayer[id].execAI = array2.GetFlag(id, 0x17);
		saPlayer[id].strongGuard = array2.GetFlag(id, 0x18);
		saPlayer[id].status.resize(saPlayer[id].maxLevel);

		for (int i = 0; i < saPlayer[id].maxLevel; i++) {
			const uint16_t* tmp = reinterpret_cast< const uint16_t* >(array2.GetData(id, 0x1F).GetPtr());
			saPlayer[id].status[i].maxHP   = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*0) );
			saPlayer[id].status[i].maxMP   = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*1) );
			saPlayer[id].status[i].attack  = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*2) );
			saPlayer[id].status[i].defence = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*3) );
			saPlayer[id].status[i].magic   = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*4) );
			saPlayer[id].status[i].speed   = sueLib::getLE( tmp + (i + saPlayer[id].maxLevel*5) );
		}

		saPlayer[id].expBase = array2.GetNumber(id, 0x29, 30);
		saPlayer[id].expGain = array2.GetNumber(id, 0x2A, 30);
		saPlayer[id].expOffset = array2.GetNumber(id, 0x2B);
		{
			const uint16_t* temp = reinterpret_cast< const uint16_t* >(array2.GetData(id, 0x33).GetPtr());
			saPlayer[id].initEquip.weapon = sueLib::getLE(temp+0);
			saPlayer[id].initEquip.shield = sueLib::getLE(temp+1);
			saPlayer[id].initEquip.protector = sueLib::getLE(temp+2);
			saPlayer[id].initEquip.helmet = sueLib::getLE(temp+3);
			saPlayer[id].initEquip.accessory = sueLib::getLE(temp+4);
		}
		saPlayer[id].bareHandsAnime = array2.GetNumber(id, 0x38);
		CRpgArray2 array2Sub = CRpgUtil::GetArray2(array2.GetData(id, 0x3F));
		int learnMax = array2Sub.GetMaxRow();
		saPlayer[id].learnSkill.resize(learnMax+1);
		for (int i = 1; i <= learnMax; i++) {
			saPlayer[id].learnSkill[i].level = array2Sub.GetNumber(i, 0x01);
			saPlayer[id].learnSkill[i].skill = array2Sub.GetNumber(i, 0x02);
		}
		saPlayer[id].useUserCommandName = array2.GetFlag(id, 0x42);
		saPlayer[id].userCommandName = array2.GetString(id, 0x43);
		CRpgUtil::AnalyzeDataArray(saPlayer[id].conditionAvailability, array2, id, 0x47);
		CRpgUtil::AnalyzeDataArray(saPlayer[id].attributeAvailavility, array2, id, 0x49);
	}
}

void CRpgLdb::AnalyzeSkill(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saSkill.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saSkill[id].name = array2.GetString(id, 0x01);
		saSkill[id].explain = array2.GetString(id, 0x02);
		saSkill[id].message1 = array2.GetString(id, 0x03);
		saSkill[id].message2 = array2.GetString(id, 0x04);
		saSkill[id].messageMiss = array2.GetNumber(id, 0x07);
		saSkill[id].type = array2.GetNumber(id, 0x08);
		saSkill[id].consumeMPFix = array2.GetNumber(id, 0x0B);
		saSkill[id].scope = array2.GetNumber(id, 0x0C);
		saSkill[id].anime = array2.GetNumber(id, 0x0E);
		saSkill[id].attackRatio = array2.GetNumber(id, 0x15);
		saSkill[id].magicRatio = array2.GetNumber(id, 0x16, 3);
		saSkill[id].variance = array2.GetNumber(id, 0x17, 4);
		saSkill[id].baseEffect = array2.GetNumber(id, 0x18);
		saSkill[id].baseSuccess = array2.GetNumber(id, 0x19, 100);
		saSkill[id].statusDownHP = array2.GetFlag(id, 0x1F);
		saSkill[id].statusDownMP = array2.GetFlag(id, 0x20);
		saSkill[id].statusDownAttack = array2.GetFlag(id, 0x21);
		saSkill[id].statusDownDefence = array2.GetFlag(id, 0x22);
		saSkill[id].statusDownMagic = array2.GetFlag(id, 0x23);
		saSkill[id].statusDownSpeed = array2.GetFlag(id, 0x24);
		saSkill[id].statusDownAbsorption = array2.GetFlag(id, 0x25);
		saSkill[id].statusDownIgnoreDefence = array2.GetFlag(id, 0x26);
		CRpgUtil::AnalyzeDataArray(saSkill[id].conditionChange, array2, id, 0x29);
		CRpgUtil::AnalyzeDataArray(saSkill[id].attribute, array2, id, 0x2B);
		saSkill[id].attributeDefenceDown = array2.GetFlag(id, 0x2D);
		saSkill[id].onSwitch = array2.GetNumber(id, 0x0D);
		saSkill[id].useField = array2.GetFlag(id, 0x12, 1);
		saSkill[id].useBattle = array2.GetFlag(id, 0x13);
	}
}

void CRpgLdb::AnalyzeItem(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saItem.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saItem[id].name = array2.GetString(id, 0x01);
		saItem[id].explain = array2.GetString(id, 0x02);
		saItem[id].type = array2.GetNumber(id, 0x03);
		saItem[id].price = array2.GetNumber(id, 0x05);
		saItem[id].useCount = array2.GetNumber(id, 0x06, 1);
		if (saItem[id].type == kItemTypeWeapon
		 || saItem[id].type == kItemTypeShield
		 || saItem[id].type == kItemTypeProtector
		 || saItem[id].type == kItemTypeHelmet
		 || saItem[id].type == kItemTypeAccessory
		) {
			saItem[id].statusUpAttack = array2.GetNumber(id, 0x0B);
			saItem[id].statusUpDefence = array2.GetNumber(id, 0x0C);
			saItem[id].statusUpMagic = array2.GetNumber(id, 0x0D);
			saItem[id].statusUpSpeed = array2.GetNumber(id, 0x0E);
		}
		if (saItem[id].type == kItemTypeWeapon)
			saItem[id].handle = array2.GetNumber(id, 0x0F);
		saItem[id].consumeMP = array2.GetNumber(id, 0x10);
		saItem[id].baseHitRatio = array2.GetNumber(id, 0x11, 90);
		saItem[id].criticalUp = array2.GetNumber(id, 0x12);
		saItem[id].anime = array2.GetNumber(id, 0x14, 1);
		saItem[id].firstAttack = array2.GetFlag(id, 0x15);
		saItem[id].doubleAttack = array2.GetFlag(id, 0x16);
		saItem[id].wholeAttack = array2.GetFlag(id, 0x17);
		saItem[id].ignoreEnemyAvoidance = array2.GetFlag(id, 0x18);
		saItem[id].protectCritical = array2.GetFlag(id, 0x19);
		saItem[id].attackAvoidanceUp = array2.GetFlag(id, 0x1A);
		saItem[id].halfConsumeMP = array2.GetFlag(id, 0x1B);
		saItem[id].protectDamageTerrain = array2.GetFlag(id, 0x1C);
		saItem[id].scope = array2.GetNumber(id, 0x1F);
		saItem[id].cureHPRatio = array2.GetNumber(id, 0x20);
		saItem[id].cureHPValue = array2.GetNumber(id, 0x21);
		saItem[id].cureMPRatio = array2.GetNumber(id, 0x22);
		saItem[id].cureMPValue = array2.GetNumber(id, 0x23);
		saItem[id].fieldOnly = array2.GetFlag(id, 0x25);
		saItem[id].deadOnly = array2.GetFlag(id, 0x26);
		saItem[id].statusChangeHP = array2.GetNumber(id, 0x29);
		saItem[id].statusChangeMP = array2.GetNumber(id, 0x2A);
		saItem[id].statusChangeAttack = array2.GetNumber(id, 0x2B);
		saItem[id].statusChangeDefence = array2.GetNumber(id, 0x2C);
		saItem[id].statusChangeMagic = array2.GetNumber(id, 0x2D);
		saItem[id].statusChangeSpeed = array2.GetNumber(id, 0x2E);
		saItem[id].messageType = array2.GetNumber(id, 0x33);
		saItem[id].skill = array2.GetNumber(id, 0x35, 1);
		if (saItem[id].type == kItemTypeSwitch) {
			saItem[id].onSwitch = array2.GetNumber(id, 0x37, 1);
			saItem[id].useField = array2.GetFlag(id, 0x39, 1);
			saItem[id].useBattle = array2.GetFlag(id, 0x3A);
		}
		if (saItem[id].type == kItemTypeWeapon
		 || saItem[id].type == kItemTypeShield
		 || saItem[id].type == kItemTypeProtector
		 || saItem[id].type == kItemTypeHelmet
		 || saItem[id].type == kItemTypeAccessory
		 || saItem[id].type == kItemTypeMedicine
		 || saItem[id].type == kItemTypeBook
		 || saItem[id].type == kItemTypeSeed
		 || saItem[id].type == kItemTypeSpecial
		) {
			CRpgUtil::AnalyzeDataArray(saItem[id].equipPlayer, array2, id, 0x3D);
		}
		if (saItem[id].type == kItemTypeWeapon
		 || saItem[id].type == kItemTypeShield
		 || saItem[id].type == kItemTypeProtector
		 || saItem[id].type == kItemTypeHelmet
		 || saItem[id].type == kItemTypeAccessory
		 || saItem[id].type == kItemTypeMedicine
		) {
			CRpgUtil::AnalyzeDataArray(saItem[id].conditionChange, array2, id, 0x3F);
		}
		if (saItem[id].type == kItemTypeWeapon
		 || saItem[id].type == kItemTypeShield
		 || saItem[id].type == kItemTypeProtector
		 || saItem[id].type == kItemTypeHelmet
		 || saItem[id].type == kItemTypeAccessory
		) {
			CRpgUtil::AnalyzeDataArray(saItem[id].attribute, array2, id, 0x41);
			saItem[id].conditionChangeRatio = array2.GetNumber(id, 0x43);
			saItem[id].revertCondition = array2.GetFlag(id, 0x44);
		}
	}
}

void CRpgLdb::AnalyzeEnemy(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saEnemy.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saEnemy[id].name = array2.GetString(id, 0x01);
		saEnemy[id].graphicName = array2.GetString(id, 0x02);
		saEnemy[id].graphicColor = array2.GetNumber(id, 0x03);
		saEnemy[id].status.maxHP = array2.GetNumber(id, 0x04, 10);
		saEnemy[id].status.maxMP = array2.GetNumber(id, 0x05, 10);
		saEnemy[id].status.attack = array2.GetNumber(id, 0x06, 10);
		saEnemy[id].status.defence = array2.GetNumber(id, 0x07, 10);
		saEnemy[id].status.magic = array2.GetNumber(id, 0x08, 10);
		saEnemy[id].status.speed = array2.GetNumber(id, 0x09, 10);
		saEnemy[id].graphicSemi = array2.GetFlag(id, 0x0A);
		saEnemy[id].resultExp = array2.GetNumber(id, 0x0B);
		saEnemy[id].resultGold = array2.GetNumber(id, 0x0C);
		saEnemy[id].resultItem = array2.GetNumber(id, 0x0D);
		saEnemy[id].resultItemDropRatio = array2.GetNumber(id, 0x0E, 100);
		saEnemy[id].criticalEnable = array2.GetFlag(id, 0x15);
		saEnemy[id].criticalRatio = array2.GetNumber(id, 0x16, 30);
		saEnemy[id].missRush = array2.GetFlag(id, 0x1A);
		saEnemy[id].fly = array2.GetFlag(id, 0x1C);
		CRpgUtil::AnalyzeDataArray(saEnemy[id].conditionAvailability, array2, id, 0x1F);
		CRpgUtil::AnalyzeDataArray(saEnemy[id].attributeAvailavility, array2, id, 0x21);
		CRpgArray2 array2Sub = CRpgUtil::GetArray2(array2.GetData(id, 0x2A));
		int patternMax = array2Sub.GetMaxRow();
		saEnemy[id].attackPattern.resize(patternMax+1);
		for (int i = 1; i <= patternMax; i++) {
			saEnemy[id].attackPattern[i].type = array2Sub.GetNumber(i, 0x01);
			saEnemy[id].attackPattern[i].base = array2Sub.GetNumber(i, 0x02, 1);
			saEnemy[id].attackPattern[i].skill = array2Sub.GetNumber(i, 0x03, 1);
			saEnemy[id].attackPattern[i].transform = array2Sub.GetNumber(i, 0x04, 1);
			saEnemy[id].attackPattern[i].conditionType = array2Sub.GetNumber(i, 0x05);
			saEnemy[id].attackPattern[i].conditionNumberA = array2Sub.GetNumber(i, 0x06);
			saEnemy[id].attackPattern[i].conditionNumberB = array2Sub.GetNumber(i, 0x07);
			saEnemy[id].attackPattern[i].conditionSwitch = array2Sub.GetNumber(i, 0x08, 1);
			saEnemy[id].attackPattern[i].onSwitchEnable = array2Sub.GetFlag(i, 0x09);
			saEnemy[id].attackPattern[i].onSwitchID = array2Sub.GetNumber(i, 0x0A, 1);
			saEnemy[id].attackPattern[i].offSwitchEnable = array2Sub.GetFlag(i, 0x0B);
			saEnemy[id].attackPattern[i].offSwitchID = array2Sub.GetNumber(i, 0x0C, 1);
			saEnemy[id].attackPattern[i].priority = array2Sub.GetNumber(i, 0x0D, 50);
		}
	}
}

void CRpgLdb::AnalyzeEnemyGroup(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saEnemyGroup.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saEnemyGroup[id].name = array2.GetString(id, 0x01);
		CRpgArray2 array2Sub = CRpgUtil::GetArray2(array2.GetData(id, 0x02));
		int placementMax = array2Sub.GetMaxRow();
		saEnemyGroup[id].placement.resize(placementMax+1);
		for (int i = 1; i <= placementMax; i++) {
			saEnemyGroup[id].placement[i].enemyID = array2Sub.GetNumber(i, 0x01, 1);
			saEnemyGroup[id].placement[i].x = array2Sub.GetNumber(i, 0x02);
			saEnemyGroup[id].placement[i].y = array2Sub.GetNumber(i, 0x03);
			saEnemyGroup[id].placement[i].breakIn = array2Sub.GetFlag(i, 0x04);
		}
		CRpgUtil::AnalyzeDataArray(saEnemyGroup[id].appearTerrain, array2, id, 0x04);
	}
}

void CRpgLdb::AnalyzeTerrain(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saTerrain.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saTerrain[id].name = array2.GetString(id, 0x01);
		saTerrain[id].damage = array2.GetNumber(id, 0x02);
		saTerrain[id].encounterRatio = array2.GetNumber(id, 0x03);
		saTerrain[id].battleGraphic = array2.GetString(id, 0x04);
		saTerrain[id].passBoat = array2.GetFlag(id, 0x05);
		saTerrain[id].passVessel = array2.GetFlag(id, 0x06);
		saTerrain[id].passAirship = array2.GetFlag(id, 0x07);
		saTerrain[id].landAirship = array2.GetFlag(id, 0x09);
		saTerrain[id].charaDisplayMode = array2.GetNumber(id, 0x0B);
	}
}

void CRpgLdb::AnalyzeBattleAnime(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saBattleAnime.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saBattleAnime[id].name = array2.GetString(id, 0x01);
		saBattleAnime[id].filename = array2.GetString(id, 0x02);
		CRpgArray2 array2Sub = CRpgUtil::GetArray2(array2.GetData(id, 0x06));
		int array2SubMax = array2Sub.GetMaxRow();
		saBattleAnime[id].timingOfFlashes.resize(array2SubMax+1);
		for (int i = 1; i <= array2SubMax; i++) {
			saBattleAnime[id].timingOfFlashes[i].frame = array2Sub.GetNumber(i, 0x01);
			saBattleAnime[id].timingOfFlashes[i].scope = array2Sub.GetNumber(i, 0x03);
			saBattleAnime[id].timingOfFlashes[i].colorR = array2Sub.GetNumber(i, 0x04, 31);
			saBattleAnime[id].timingOfFlashes[i].colorG = array2Sub.GetNumber(i, 0x05, 31);
			saBattleAnime[id].timingOfFlashes[i].colorB = array2Sub.GetNumber(i, 0x06, 31);
			saBattleAnime[id].timingOfFlashes[i].strength = array2Sub.GetNumber(i, 0x07, 31);
		}
		saBattleAnime[id].scope = array2.GetNumber(id, 0x09);
		saBattleAnime[id].baseLine = array2.GetNumber(id, 0x0A);
		saBattleAnime[id].useGrid = array2.GetFlag(id, 0x0B, true);
		array2Sub = CRpgUtil::GetArray2(array2.GetData(id, 0x0C));
		array2SubMax = array2Sub.GetMaxRow();
		saBattleAnime[id].animeFrames.resize(array2SubMax+1);
		for (int i = 1; i <= array2SubMax; i++) {
			CRpgArray2 array2SubSub = CRpgUtil::GetArray2(array2Sub.GetData(i, 0x01));
			int array2SubSubMax = array2SubSub.GetMaxRow();
			saBattleAnime[id].animeFrames[i].cells.Resize(array2SubSubMax+1);
			for (int ii = 1; ii <= array2SubSubMax; ii++) {
				saBattleAnime[id].animeFrames[i].cells[ii].visible = array2SubSub.GetFlag(ii, 0x01, true);
				saBattleAnime[id].animeFrames[i].cells[ii].pattern = array2SubSub.GetNumber(ii, 0x02);
				saBattleAnime[id].animeFrames[i].cells[ii].x = array2SubSub.GetNumber(ii, 0x03);
				saBattleAnime[id].animeFrames[i].cells[ii].y = array2SubSub.GetNumber(ii, 0x04);
				saBattleAnime[id].animeFrames[i].cells[ii].scale = array2SubSub.GetNumber(ii, 0x05, 100);
				saBattleAnime[id].animeFrames[i].cells[ii].colorR = array2SubSub.GetNumber(ii, 0x06, 100);
				saBattleAnime[id].animeFrames[i].cells[ii].colorG = array2SubSub.GetNumber(ii, 0x07, 100);
				saBattleAnime[id].animeFrames[i].cells[ii].colorB = array2SubSub.GetNumber(ii, 0x08, 100);
				saBattleAnime[id].animeFrames[i].cells[ii].colorIntensity = array2SubSub.GetNumber(ii, 0x09, 100);
				saBattleAnime[id].animeFrames[i].cells[ii].colorA = array2SubSub.GetNumber(ii, 0x0A);
			}
		}
	}
}

void CRpgLdb::AnalyzeAttribute(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saAttribute.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saAttribute[id].name = array2.GetString(id, 0x01);
		saAttribute[id].type = array2.GetNumber(id, 0x02);
		saAttribute[id].effectRatioA = array2.GetNumber(id, 0x0B);
		saAttribute[id].effectRatioB = array2.GetNumber(id, 0x0C);
		saAttribute[id].effectRatioC = array2.GetNumber(id, 0x0D);
		saAttribute[id].effectRatioD = array2.GetNumber(id, 0x0E);
		saAttribute[id].effectRatioE = array2.GetNumber(id, 0x0F);
	}
}

void CRpgLdb::AnalyzeCondition(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saCondition.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saCondition[id].name = array2.GetString(id, 0x01);
		saCondition[id].type = array2.GetNumber(id, 0x02);
		saCondition[id].color = array2.GetNumber(id, 0x03, 6);
		saCondition[id].priority = array2.GetNumber(id, 0x04, 50);
		saCondition[id].limitAction = array2.GetNumber(id, 0x05);
		saCondition[id].applyRatioA = array2.GetNumber(id, 0x0B, 100);
		saCondition[id].applyRatioB = array2.GetNumber(id, 0x0C, 80);
		saCondition[id].applyRatioC = array2.GetNumber(id, 0x0D, 60);
		saCondition[id].applyRatioD = array2.GetNumber(id, 0x0E, 30);
		saCondition[id].applyRatioE = array2.GetNumber(id, 0x0F);
		saCondition[id].cureTurn = array2.GetNumber(id, 0x15);
		saCondition[id].cureRatioNatural = array2.GetNumber(id, 0x16);
		saCondition[id].cureRatioImpact = array2.GetNumber(id, 0x17);
		saCondition[id].changeAttack = array2.GetFlag(id, 0x1F);
		saCondition[id].changeDefence = array2.GetFlag(id, 0x20);
		saCondition[id].changeMagic = array2.GetFlag(id, 0x21);
		saCondition[id].changeSpeed = array2.GetFlag(id, 0x22);
		saCondition[id].changeHitRatio = array2.GetNumber(id, 0x23);
		saCondition[id].enableAttackRatio = array2.GetFlag(id, 0x29);
		saCondition[id].attackRatio = array2.GetNumber(id, 0x2A, 100);
		saCondition[id].enableMagicRatio = array2.GetFlag(id, 0x2B);
		saCondition[id].magicRatio = array2.GetNumber(id, 0x2C);
		saCondition[id].friendMessage = array2.GetString(id, 0x33);
		saCondition[id].enemyMessage = array2.GetString(id, 0x34);
		saCondition[id].alreadyMessage = array2.GetString(id, 0x35);
		saCondition[id].myTurnMessage = array2.GetString(id, 0x36);
		saCondition[id].cureMessage = array2.GetString(id, 0x37);
		saCondition[id].changeHPBattleRatio = array2.GetNumber(id, 0x3D);
		saCondition[id].changeHPBattleValue = array2.GetNumber(id, 0x3E);
		saCondition[id].changeHPMoveRatio = array2.GetNumber(id, 0x3F);
		saCondition[id].changeHPMoveValue = array2.GetNumber(id, 0x40);
		saCondition[id].changeMPBattleRatio = array2.GetNumber(id, 0x41);
		saCondition[id].changeMPBattleValue = array2.GetNumber(id, 0x42);
		saCondition[id].changeMPMoveRatio = array2.GetNumber(id, 0x43);
		saCondition[id].changeMPMoveValue = array2.GetNumber(id, 0x44);
	}
}

void CRpgLdb::AnalyzeChipSet(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saChipSet.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saChipSet[id].strName = array2.GetString(id, 0x01);
		saChipSet[id].strFile = array2.GetString(id, 0x02);
		CRpgUtil::AnalyzeDataArray(saChipSet[id].randforms, array2, id, 0x03, LOWER_CHIP_NUM);
		CRpgUtil::AnalyzeDataArray(saChipSet[id].blockLower, array2, id, 0x04, LOWER_CHIP_NUM);
		CRpgUtil::AnalyzeDataArray(saChipSet[id].blockUpper, array2, id, 0x05, UPPER_CHIP_NUM);
		saChipSet[id].seaAnimeType = array2.GetNumber(id, 0x0B);
		saChipSet[id].seaAnimeSpeed = array2.GetNumber(id, 0x0C);
	}
}

void AnalyzeShopTerm(CRpgArray1& array1, CRpgLdb::ShopTerm& shop, int start)
{
	shop.what = array1.GetString(start + 0);
	shop.what2 = array1.GetString(start + 1);
	shop.buy = array1.GetString(start + 2);
	shop.sell = array1.GetString(start + 3);
	shop.cancel = array1.GetString(start + 4);
	shop.buyItem = array1.GetString(start + 5);
	shop.buyNum = array1.GetString(start + 6);
	shop.buyEnd = array1.GetString(start + 7);
	shop.sellItem = array1.GetString(start + 8);
	shop.sellNum = array1.GetString(start + 9);
	shop.sellEnd = array1.GetString(start + 10);
}

void AnalyzeInnTerm(CRpgArray1& array1, CRpgLdb::InnTerm& inn, int start)
{
	inn.what[0] = array1.GetString(start + 0);
	inn.what[1] = array1.GetString(start + 1);
	inn.what[2] = array1.GetString(start + 2);
	inn.ok = array1.GetString(start + 3);
	inn.cancel = array1.GetString(start + 4);
}

void CRpgLdb::AnalyzeTerm(sueLib::smart_buffer& buf)
{
	CRpgArray1 array1 = CRpgUtil::GetArray1(buf);
	term.battle.battleStart = array1.GetString(0x01);
	term.battle.firstAttack = array1.GetString(0x02);
	term.battle.escapeSuccess = array1.GetString(0x03);
	term.battle.escapeMiss = array1.GetString(0x04);
	term.battle.win = array1.GetString(0x05);
	term.battle.lose = array1.GetString(0x06);
	term.battle.getExp = array1.GetString(0x07);
	term.battle.getMoney[0] = array1.GetString(0x08);
	term.battle.getMoney[1] = array1.GetString(0x09);
	term.battle.getItem = array1.GetString(0x0A);
	term.battle.normalAttack = array1.GetString(0x0B);
	term.battle.criticalFriend = array1.GetString(0x0C);
	term.battle.criticalEnemy = array1.GetString(0x0D);
	term.battle.guard = array1.GetString(0x0E);
	term.battle.seek = array1.GetString(0x0F);
	term.battle.charge = array1.GetString(0x10);
	term.battle.suicideBombing = array1.GetString(0x11);
	term.battle.escapeEnemy = array1.GetString(0x12);
	term.battle.transform = array1.GetString(0x13);
	term.battle.enemyDamage = array1.GetString(0x14);
	term.battle.enemyNoDamage = array1.GetString(0x15);
	term.battle.friendDamage = array1.GetString(0x16);
	term.battle.friendNoDamage = array1.GetString(0x17);
	term.battle.skillMiss[0] = array1.GetString(0x18);
	term.battle.skillMiss[1] = array1.GetString(0x19);
	term.battle.skillMiss[2] = array1.GetString(0x1A);
	term.battle.attackMiss = array1.GetString(0x1B);
	term.battle.useItem = array1.GetString(0x1C);
	term.battle.paramCure = array1.GetString(0x1D);
	term.battle.paramUp = array1.GetString(0x1E);
	term.battle.paramDown = array1.GetString(0x1F);
	term.battle.friendAbsorb = array1.GetString(0x20);
	term.battle.enemyAbsorb = array1.GetString(0x21);
	term.battle.attributeUp = array1.GetString(0x22);
	term.battle.attributeDown = array1.GetString(0x23);
	term.battle.levelUp = array1.GetString(0x24);
	term.battle.getSkill = array1.GetString(0x25);
	
	AnalyzeShopTerm(array1, term.shop[0], 0x29);
	AnalyzeShopTerm(array1, term.shop[1], 0x36);
	AnalyzeShopTerm(array1, term.shop[2], 0x43);
	
	AnalyzeInnTerm(array1, term.inn[0], 0x50);
	AnalyzeInnTerm(array1, term.inn[1], 0x55);
	
	term.shopParam.itemGet = array1.GetString(0x5C);
	term.shopParam.itemEquiped = array1.GetString(0x5D);
	term.shopParam.money = array1.GetString(0x5F);
	
	term.param.level = array1.GetString(0x7B);
	term.param.hp = array1.GetString(0x7C);
	term.param.mp = array1.GetString(0x7D);
	term.param.condition = array1.GetString(0x7E);
	term.param.exp = array1.GetString(0x7F);
	term.param.levelShort = array1.GetString(0x80);
	term.param.hpShort = array1.GetString(0x81);
	term.param.mpShort = array1.GetString(0x82);
	term.param.consumeMp = array1.GetString(0x83);
	term.param.attack = array1.GetString(0x84);
	term.param.defence = array1.GetString(0x85);
	term.param.magic = array1.GetString(0x86);
	term.param.speed = array1.GetString(0x87);
	term.param.weapon = array1.GetString(0x88);
	term.param.shield = array1.GetString(0x89);
	term.param.protector = array1.GetString(0x8A);
	term.param.helmet = array1.GetString(0x8B);
	term.param.accessory = array1.GetString(0x8C);
	
	term.menu.battle = array1.GetString(0x65);
	term.menu.autoBattle = array1.GetString(0x66);
	term.menu.escape = array1.GetString(0x67);
	term.menu.attack = array1.GetString(0x68);
	term.menu.guard = array1.GetString(0x69);
	term.menu.item = array1.GetString(0x6A);
	term.menu.skill = array1.GetString(0x6B);
	term.menu.equip = array1.GetString(0x6C);
	term.menu.save = array1.GetString(0x6E);
	term.menu.endGame = array1.GetString(0x70);
	
	term.title.newGame = array1.GetString(0x72);
	term.title.loadGame = array1.GetString(0x73);
	term.title.endGame = array1.GetString(0x75);

	term.saveLoad.selectSave = array1.GetString(0x92);
	term.saveLoad.selectLoad = array1.GetString(0x93);
	term.saveLoad.checkEndGame = array1.GetString(0x94);
	term.saveLoad.file = array1.GetString(0x97);
	term.saveLoad.yes = array1.GetString(0x98);
	term.saveLoad.no = array1.GetString(0x99);
}

void CRpgLdb::AnalyzeSystem(sueLib::smart_buffer& buf)
{
	CRpgArray1 array1 = CRpgUtil::GetArray1(buf);
	system.boat = array1.GetString(0x0B);
	system.vessel = array1.GetString(0x0C);
	system.airship = array1.GetString(0x0D);
	system.title = array1.GetString(0x11);
	system.gameover = array1.GetString(0x12);
	system.system = array1.GetString(0x13);
	CRpgUtil::AnalyzeDataArray(system.startParty, array1, 0x15);
}

void CRpgLdb::AnalyzeSwitch(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saSwitch.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saSwitch[id].name = array2.GetString(id, 0x01);
	}
}

void CRpgLdb::AnalyzeVariable(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saVar.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saVar[id].name = array2.GetString(id, 0x01);
	}
}

void CRpgLdb::AnalyzeCommonEvent(sueLib::smart_buffer& buf)
{
	CRpgArray2 array2 = CRpgUtil::GetArray2(buf);
	int max = array2.GetMaxRow();
	saCommonEvent.Resize(max+1);
	for(int id = 1; id<=max; id++){
		saCommonEvent[id].name = array2.GetString(id, 0x01);
		saCommonEvent[id].eventList.condition.nStart = array2.GetNumber(id, 0x0B, 5);
		saCommonEvent[id].eventList.condition.nFlag = array2.GetFlag(id, 0x0C)? CRpgEventCondition::kFlagSwitch1 : 0;
		saCommonEvent[id].eventList.condition.nSw1 = array2.GetNumber(id, 0x0D, 1);
		int eventMax = array2.GetNumber(id, 0x15);
		saCommonEvent[id].eventList.events.resize(eventMax);
		sueLib::smart_buffer eventBuf = array2.GetData(id, 0x16);
		CRpgEventStream st;
		int eventIndex = 0;
		if(st.OpenFromMemory(eventBuf, eventBuf.GetSize())){
			CRpgEvent* event = &saCommonEvent[id].eventList.events[eventIndex];
			while(st.ReadEvent(*event)){
				eventIndex++;
				event = &saCommonEvent[id].eventList.events[eventIndex];
			}
		}
	}
}

