/**
 * @file
 * @brief Game Event Manager
 * @author project.kuto
 */

#include <utility>
#include <sstream>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_virtual_pad.h>
#include "game_event_manager.h"
#include "game_field.h"
#include "game_map.h"
#include "game_player.h"
#include "game_npc.h"
#include "game_message_window.h"
#include "game_collision.h"
#include "game_inventory.h"
#include "game_battle.h"
#include "game_select_window.h"
#include "game_event_picture.h"
#include "game_name_input_menu.h"
#include "game_bgm.h"
#include "game_shop_menu.h"
#include "game_event_map_chip.h"
#include "game_skill_anime.h"

// イベントコード(ツクール内で使用)
// 操作系
#define CODE_OPERATE_SWITCH		0x27e2	///< スイッチ
#define CODE_OPERATE_VAR		0x27ec	///< 変数
#define CODE_OPERATE_ITEM		0x2850	///< アイテム
#define CODE_OPERATE_KEY		0x2d5a	///< キー
#define CODE_OPERATE_INPUT		0x27A6	///< 数値入力
#define CODE_OPERATE_TIMER		0x27F6	///< タイマー
#define CODE_OPERATE_MONEY		0x2846	///< 所持金
// パーティ
#define CODE_PARTY_CHANGE		0x285a	///< パーティ変更
#define CODE_PARTY_HP			0x28dc	///< HP
#define CODE_PARTY_MP			0x28E6	///< MP
#define CODE_PARTY_STATE		0x28f0	///< 状態変更
#define CODE_PARTY_SKILL		0x28c8	///< 特技
#define CODE_PARTY_REFRESH		0x28fa	///< 回復
#define CODE_PARTY_EXP			0x28AA	///< 経験値
#define CODE_PARTY_LV			0x28B4	///< レベル
#define CODE_PARTY_POWER		0x28BE	///< 能力値
#define CODE_PARTY_SOUBI		0x28D2	///< 装備
#define CODE_PARTY_DAMAGE		0x2904	///< ダメージ処理
#define CODE_PARTY_NAME			0x2972	///< 名前
#define CODE_PARTY_TITLE		0x297C	///< 肩書き
#define CODE_PARTY_WALK			0x2986	///< 歩行絵
#define CODE_PARTY_FACE			0x2990	///< 顔絵
// システム
#define CODE_SYSTEM_VEHICLE		0x299A	///< 乗り物絵変更
#define CODE_SYSTEM_BGM			0x29a4	///< システムBGM
#define CODE_SYSTEM_SOUND		0x29AE	///< システム効果音
#define CODE_SYSTEM_GRAPHIC		0x29B8	///< システムグラフィック
#define CODE_SYSTEM_SCREEN		0x29C2	///< 画面切り替え方式
// 乗り物
#define CODE_VEH_RIDE			0x2A58	///< 乗降
#define CODE_VEH_LOCATE			0x2A62	///< 位置設定
// 構文
#define CODE_BLOCK_END			0x000a	///< Block
#define CODE_IF_START			0x2eea	///< if
#define CODE_IF_ELSE			0x55fa	///< else
#define CODE_IF_END				0x55fb	///< end if
#define CODE_LOOP_START			0x2fb2	///< loop
#define CODE_LOOP_BREAK			0x2fbc	///< break
#define CODE_LOOP_END			0x56c2	///< loop end
#define CODE_GOTO_MOVE			0x2f58	///< goto
#define CODE_GOTO_LABEL			0x2f4e	///< goto label
#define CODE_SELECT_START		0x279c	///< select case
#define CODE_SELECT_CASE		0x4eac	///< case n
#define CODE_SELECT_END			0x4ead	///< end select
#define CODE_SHOP				0x29E0	///< お店
#define CODE_SHOP_IF_START		0x50F0	///< お店(売買した)
#define CODE_SHOP_IF_ELSE		0x50F1	///< お店(売買しない)
#define CODE_SHOP_IF_END		0x50F2	///< お店(End If)
#define CODE_INN				0x29EA	///< 宿屋
#define CODE_INN_IF_START		0x50FA	///< 宿屋(宿泊した)
#define CODE_INN_IF_ELSE		0x50FB	///< 宿屋(宿泊しない)
#define CODE_INN_IF_END			0x50FC	///< 宿屋(End IF)
// イベント
#define CODE_EVENT_BREAK		0x3016	///< イベント中断
#define CODE_EVENT_CLEAR		0x3020	///< 一時消去
#define CODE_EVENT_GOSUB		0x302a	///< サブルーチン(イベントのよびだし)
// マルチメディア
#define CODE_MM_SOUND			0x2d1e	///< 効果音
#define CODE_MM_BGM_PLAY		0x2cf6	///< BGM再生
#define CODE_MM_BGM_FADEOUT		0x2d00	///< BGMフェードアウト
#define CODE_MM_BGM_SAVE		0x2D0A	///< BGM記憶
#define CODE_MM_BGM_LOAD		0x2D14	///< 記憶したBGMを再生
#define CODE_MM_MOVIE			0x2D28	///< ムービー
// 画面
#define CODE_SCREEN_CLEAR		0x2b02	///< 消去
#define CODE_SCREEN_SHOW		0x2b0c	///< 表示
#define CODE_SCREEN_COLOR		0x2b16	///< 色調変更
#define CODE_SCREEN_SHAKE		0x2b2a	///< シェイク
#define CODE_SCREEN_FLASH		0x2b20	///< フラッシュ
#define CODE_SCREEN_SCROLL		0x2B34	///< スクロール
#define CODE_SCREEN_WEATHER		0x2B3E	///< 天気
// ピクチャ
#define CODE_PICT_SHOW			0x2b66	///< 表示
#define CODE_PICT_MOVE			0x2b70	///< 移動
#define CODE_PICT_CLEAR			0x2b7a	///< 消去
// キャラ
#define CODE_CHARA_TRANS		0x2c2e	///< 透明状態変更
#define CODE_CHARA_MOVE			0x2c42	///< 移動
#define CODE_CHARA_FLASH		0x2C38	///< フラッシュ
#define CODE_MOVEALL_START		0x2C4C	///< 指定動作の全実行
#define CODE_MOVEALL_CANSEL		0x2C56	///< 指定動作の全実行をキャンセル
// 位置
#define CODE_LOCATE_MOVE		0x2a3a	///< 場所移動(固定値)
#define CODE_LOCATE_SAVE		0x2a44	///< 現在地保存
#define CODE_LOCATE_LOAD		0x2a4e	///< 記憶した場所に移動(変数)
// テキスト・注釈
#define CODE_TXT_REM			0x307a	///< 注釈
#define CODE_TXT_REM_ADD		0x578a	///< 注釈追加
#define CODE_TXT_SHOW			0x277e	///< 本文
#define CODE_TXT_SHOW_ADD		0x4e8e	///< 本文追加
#define CODE_TXT_OPTION			0x2788	///< 文章表示オプション
#define CODE_TXT_FACE			0x2792	///< 顔CG変更
// その他
#define CODE_NAME_INPUT			0x29F4	///< 名前入力
#define CODE_EVENT_LOCATE		0x2A6C	///< イベント位置設定
#define CODE_EVENT_SWAP			0x2A76	///< イベント位置交換
#define CODE_LAND_ID			0x2A9E	///< 地形ID取得
#define CODE_EVENT_ID			0x2AA8	///< イベントID取得
#define CODE_WAIT				0x2c92	///< ウェイト
#define CODE_CHIPSET			0x2DBE	///< チップセット
#define CODE_PANORAMA			0x2DC8	///< 遠景
#define CODE_ENCOUNT			0x2DDC	///< エンカウント
#define CODE_CHIP_CONVERT		0x2DE6	///< チップ置換
#define CODE_TELEPORT			0x2E22	///< テレポート増減
#define CODE_TELEPORT_PERM		0x2E2C	///< テレポート禁止
#define CODE_ESCAPE				0x2E36	///< エスケープ位置
#define CODE_ESCAPE_PERM		0x2E40	///< エスケープ禁止
#define CODE_SAVE_SHOW			0x2E86	///< セーブ画面
#define CODE_SAVE_PERM			0x2E9A	///< セーブ禁止
#define CODE_MENU_SHOW			0x2eae	///< メニュー表示
#define CODE_MENU_PERM			0x2EB8	///< メニュー禁止
#define CODE_LABEL				0x2F4E	///< ラベルの設定
#define CODE_GAMEOVER			0x3084	///< ゲームオーバー
#define CODE_TITLE				0x30DE	///< タイトルに戻る
#define CODE_BTLANIME			0x2BCA	///< 戦闘アニメ(非戦闘)
// 戦闘系
#define CODE_BTL_GO_START		0x29d6	///< 戦闘分岐
#define CODE_BTL_GO_WIN			0x50e6	///< 勝った時
#define CODE_BTL_GO_ESCAPE		0x50e7	///< 逃げた時
#define CODE_BTL_GO_LOSE		0x50e8	///< 負けた時
#define CODE_BTL_GO_END			0x50e9	///< 分岐終了
#define CODE_BTL_ANIME			0x33cc	///< 戦闘アニメ
// ツクール2003追加分
#define CODE_2003_JOB			0x03F0	///< 職業変更
#define CODE_2003_BTL_CMD		0x03F1	///< 戦闘コマンド
#define CODE_2003_ATK_REPEAT	0x03EF	///< 連続攻撃
#define CODE_2003_ESCAPE100		0x03EE	///< 100%脱出
#define CODE_2003_BTL_COMMON	0x03ED	///< バトルイベントからコモンよびだし

////////////////////////////////////////// バトルイベント
#define CODE_BTL_BACKGROUND		0x339a	///< 背景変更
#define CODE_BTL_STOP			0x3462	///< 戦闘中断
// 敵
#define CODE_BTL_ENEMY_HP		0x3336	///< HP操作
#define CODE_BTL_ENEMY_MP		0x3340	///< MP操作
#define CODE_BTL_ENEMY_STATE	0x334a	///< 状態変更
#define CODE_BTL_ENEMY_APPEAR	0x335e	///< 出現
// 戦闘構文
#define CODE_BTL_IF_START		0x33fe	///< if
#define CODE_BTL_IF_ELSE		0x5b0e	///< ※一緒
#define CODE_BTL_IF_END			0x5b0f	///< ※一緒


GameEventManager::GameEventManager(kuto::Task* parent, GameField* field)
: kuto::Task(parent)
, gameField_(field)
, currentEventPage_(NULL), executeChildCommands_(true), encountStep_(0), routeSetChara_(NULL)
, skillAnime_(NULL)
{
	bgm_ = GameBgm::createTask(this, "");	// temp
	// const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	initEventPageInfos();
	updateEventAppear();
	gameMessageWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	gameMessageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	gameMessageWindow_->setSize(kuto::Vector2(320.f, 80.f));
	gameMessageWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	gameMessageWindow_->pauseUpdate(true);
	selectWindow_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	selectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindow_->setSize(kuto::Vector2(320.f, 80.f));
	selectWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	selectWindow_->pauseUpdate(true);
	nameInputMenu_ = GameNameInputMenu::createTask(this, gameField_->getGameSystem());
	nameInputMenu_->pauseUpdate(true);
	shopMenu_ = GameShopMenu::createTask(this, gameField_->getGameSystem());
	shopMenu_->pauseUpdate(true);

/*
	comFuncMap_[CODE_OPERATE_SWITCH] = &GameEventManager::comOperateSwitch;
	comFuncMap_[CODE_OPERATE_VAR] = &GameEventManager::comOperateVar;
	comFuncMap_[CODE_OPERATE_ITEM] = &GameEventManager::comOperateItem;
	comFuncMap_[CODE_OPERATE_MONEY] = &GameEventManager::comOperateMoney;
	comFuncMap_[CODE_OPERATE_TIMER] = &GameEventManager::comOperateTimer;
	comFuncMap_[CODE_GOTO_LABEL] = &GameEventManager::comOperateJumpLabel;
	comFuncMap_[CODE_PARTY_CHANGE] = &GameEventManager::comOperatePartyChange;
	comFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::comOperateLocateMove;
	comFuncMap_[CODE_LOCATE_SAVE] = &GameEventManager::comOperateLocateSave;
	comFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::comOperateLocateLoad;
	comFuncMap_[CODE_TXT_SHOW] = &GameEventManager::comOperateTextShow;
	comFuncMap_[CODE_TXT_OPTION] = &GameEventManager::comOperateTextOption;
	comFuncMap_[CODE_TXT_FACE] = &GameEventManager::comOperateTextFace;
	comFuncMap_[CODE_BTL_GO_START] = &GameEventManager::comOperateBattleStart;
	comFuncMap_[CODE_BTL_GO_WIN] = &GameEventManager::comOperateBattleWin;
	comFuncMap_[CODE_BTL_GO_ESCAPE] = &GameEventManager::comOperateBattleEscape;
	comFuncMap_[CODE_BTL_GO_LOSE] = &GameEventManager::comOperateBattleLose;
	comFuncMap_[CODE_BTL_GO_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_IF_START] = &GameEventManager::comOperateIfStart;
	comFuncMap_[CODE_IF_ELSE] = &GameEventManager::comOperateIfElse;
	comFuncMap_[CODE_IF_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_SELECT_START] = &GameEventManager::comOperateSelectStart;
	comFuncMap_[CODE_SELECT_CASE] = &GameEventManager::comOperateSelectCase;
	comFuncMap_[CODE_SELECT_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_GAMEOVER] = &GameEventManager::comOperateGameOver;
	comFuncMap_[CODE_TITLE] = &GameEventManager::comOperateReturnTitle;
	comFuncMap_[CODE_EVENT_BREAK] = &GameEventManager::comOperateEventBreak;
	comFuncMap_[CODE_EVENT_CLEAR] = &GameEventManager::comOperateEventClear;
	comFuncMap_[CODE_LOOP_START] = &GameEventManager::comOperateLoopStart;
	comFuncMap_[CODE_LOOP_BREAK] = &GameEventManager::comOperateLoopBreak;
	comFuncMap_[CODE_LOOP_END] = &GameEventManager::comOperateLoopEnd;
	comFuncMap_[CODE_WAIT] = &GameEventManager::comOperateWait;
	comFuncMap_[CODE_PICT_SHOW] = &GameEventManager::comOperatePictureShow;
	comFuncMap_[CODE_PICT_MOVE] = &GameEventManager::comOperatePictureMove;
	comFuncMap_[CODE_PICT_CLEAR] = &GameEventManager::comOperatePictureClear;
	comFuncMap_[CODE_SYSTEM_SCREEN] = &GameEventManager::comOperateFadeType;
	comFuncMap_[CODE_SCREEN_CLEAR] = &GameEventManager::comOperateFadeOut;
	comFuncMap_[CODE_SCREEN_SHOW] = &GameEventManager::comOperateFadeIn;
	comFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::comOperateMapScroll;
	comFuncMap_[CODE_CHARA_TRANS] = &GameEventManager::comOperatePlayerVisible;
	comFuncMap_[CODE_PARTY_REFRESH] = &GameEventManager::comOperatePlayerCure;
	comFuncMap_[CODE_PARTY_EXP] = &GameEventManager::comOperateAddExp;
	comFuncMap_[CODE_PARTY_LV] = &GameEventManager::comOperateAddLevel;
	comFuncMap_[CODE_PARTY_POWER] = &GameEventManager::comOperateAddStatus;
	comFuncMap_[CODE_PARTY_SKILL] = &GameEventManager::comOperateAddSkill;
	comFuncMap_[CODE_EVENT_GOSUB] = &GameEventManager::comOperateCallEvent;
	comFuncMap_[CODE_CHARA_MOVE] = &GameEventManager::comOperateRoute;
	comFuncMap_[CODE_MOVEALL_START] = &GameEventManager::comOperateRouteStart;
	comFuncMap_[CODE_MOVEALL_CANSEL] = &GameEventManager::comOperateRouteEnd;
	comFuncMap_[CODE_NAME_INPUT] = &GameEventManager::comOperateNameInput;
	comFuncMap_[CODE_PARTY_NAME] = &GameEventManager::comOperatePlayerNameChange;
	comFuncMap_[CODE_PARTY_TITLE] = &GameEventManager::comOperatePlayerTitleChange;
	comFuncMap_[CODE_PARTY_WALK] = &GameEventManager::comOperatePlayerWalkChange;
	comFuncMap_[CODE_PARTY_FACE] = &GameEventManager::comOperatePlayerFaceChange;
	comFuncMap_[CODE_SYSTEM_BGM] = &GameEventManager::comOperateBgm;
	comFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::comOperateKey;
	comFuncMap_[CODE_PANORAMA] = &GameEventManager::comOperatePanorama;
	comFuncMap_[CODE_INN] = &GameEventManager::comOperateInnStart;
	comFuncMap_[CODE_INN_IF_START] = &GameEventManager::comOperateInnOk;
	comFuncMap_[CODE_INN_IF_ELSE] = &GameEventManager::comOperateInnCancel;
	comFuncMap_[CODE_INN_IF_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_SHOP] = &GameEventManager::comOperateShopStart;
	comFuncMap_[CODE_SHOP_IF_START] = &GameEventManager::comOperateInnOk;
	comFuncMap_[CODE_SHOP_IF_ELSE] = &GameEventManager::comOperateInnCancel;
	comFuncMap_[CODE_SHOP_IF_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_MM_SOUND] = &GameEventManager::comOperatePlaySound;
	comFuncMap_[CODE_SCREEN_COLOR] = &GameEventManager::comOperateScreenColor;
	comFuncMap_[CODE_BTLANIME] = &GameEventManager::comOperateBattleAnime;
	comFuncMap_[CODE_PARTY_SOUBI] = &GameEventManager::comOperateEquip;

	comWaitFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::comWaitLocateMove;
	comWaitFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::comWaitLocateMove;
	comWaitFuncMap_[CODE_TXT_SHOW] = &GameEventManager::comWaitTextShow;
	comWaitFuncMap_[CODE_BTL_GO_START] = &GameEventManager::comWaitBattleStart;
	comWaitFuncMap_[CODE_SELECT_START] = &GameEventManager::comWaitSelectStart;
	comWaitFuncMap_[CODE_WAIT] = &GameEventManager::comWaitWait;
	comWaitFuncMap_[CODE_PICT_MOVE] = &GameEventManager::comWaitPictureMove;
	comWaitFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::comWaitMapScroll;
	comWaitFuncMap_[CODE_PARTY_EXP] = &GameEventManager::comWaitTextShow;
	comWaitFuncMap_[CODE_PARTY_LV] = &GameEventManager::comWaitTextShow;
	comWaitFuncMap_[CODE_NAME_INPUT] = &GameEventManager::comWaitNameInput;
	comWaitFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::comWaitKey;
	comWaitFuncMap_[CODE_INN] = &GameEventManager::comWaitInnStart;
	comWaitFuncMap_[CODE_SHOP] = &GameEventManager::comWaitShopStart;
	comWaitFuncMap_[CODE_SCREEN_COLOR] = &GameEventManager::comWaitScreenColor;
	comWaitFuncMap_[CODE_BTLANIME] = &GameEventManager::comWaitBattleAnime;
 */
	pictures_.zeromemory();
}

bool GameEventManager::initialize()
{
	if (isInitializedChildren()) {
		gameMessageWindow_->pauseUpdate(false);
		gameMessageWindow_->freeze(true);
		selectWindow_->pauseUpdate(false);
		selectWindow_->freeze(true);
		nameInputMenu_->pauseUpdate(false);
		nameInputMenu_->freeze(true);
		shopMenu_->pauseUpdate(false);
		shopMenu_->freeze(true);

		bgm_->play();		// temp
		return true;
	}
	return false;
}

void GameEventManager::preMapChange()
{
/*
	for (uint i = 0; i < pictures_.size(); i++) {
		if (pictures_[i]) {
			pictures_[i]->release();
			pictures_[i] = NULL;
		}
	}
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	for (uint i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		if (eventPageInfos_[i].npc) {
			eventPageInfos_[i].npc->release();
			eventPageInfos_[i].npc = NULL;
		}
		if (eventPageInfos_[i].mapChip) {
			eventPageInfos_[i].mapChip->release();
			eventPageInfos_[i].mapChip = NULL;
		}
	}

	restEventInfo_.enable = true;
	restEventInfo_.eventIndex = waitEventInfo_.eventIndex;
	restEventInfo_.eventListCopy = *waitEventInfo_.page;	// deep copyしとく
	restEventInfo_.pos = waitEventInfo_.pos;
	restEventInfo_.nextPos = waitEventInfo_.nextPos;
	restEventInfo_.count = waitEventInfo_.count;
	restEventInfo_.executeChildCommands = waitEventInfo_.executeChildCommands;
	restEventInfo_.conditionStack = waitEventInfo_.conditionStack;
	restEventInfo_.loopStack = waitEventInfo_.loopStack;

	waitEventInfo_.page = &restEventInfo_.eventListCopy;	// pointerを差し替え
	callStack_.clear();		// これも消滅ってことで
 */
}

void GameEventManager::initEventPageInfos()
{
/*
	const rpg2k::model::DataBase& rpgLdb = gameField_->getGameSystem().getLDB();
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	eventPageInfos_.deallocate();
	eventPageInfos_.allocate(rpgLmu.saMapEvent.GetSize() + rpgLdb.saCommonEvent.GetSize());
 */
}

void GameEventManager::postMapChange()
{
	initEventPageInfos();
	updateEventAppear();
}

void GameEventManager::update()
{
	if (timer_.enable && timer_.count) timer_.count--;

	updateEventAppear();
	if (waitEventInfo_.enable)
		updateWaitEvent();
	else
		updateEvent();
	if (!waitEventInfo_.enable) {
		kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
		bool pressMenu = virtualPad->press(kuto::VirtualPad::KEY_B);
		pressMenu = pressMenu && !gameField_->getPlayerLeader()->isMoving();
		// Undefined cannot open menu flag
		if (pressMenu) gameField_->startSystemMenu();
		else updateEncount();
	}
}

/*
std::string GameEventManager::getEncountBattleMap(const rpg2k::model::MapTree::MapInfo& mapInfo, int terrainId)
{
	if (mapInfo.m_BattleMapType == 0) {
		const rpg2k::model::MapTree::MapInfo& parentInfo = gameField_->getGameSystem().getLMT().m_saMapInfo[mapInfo.m_ParentMapID];
		return getEncountBattleMap(parentInfo, terrainId);
	} else if (mapInfo.m_BattleMapType == 1) {
		const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
		return ldb.saTerrain[terrainId].battleGraphic;
	} else {
		return mapInfo.m_BattleMapName;
	}
}
 */

void GameEventManager::updateEncount()
{
/*
	if (gameField_->getGameSystem().getConfig().noEncount)
		return;

	GamePlayer* player = gameField_->getPlayerLeader();
	if (player->getMoveResult() == GameChara::kMoveResultDone) {
		encountStep_++;
		const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
		const rpg2k::model::MapTree::MapInfo& mapInfo = gameField_->getGameSystem().getLMT().m_saMapInfo[gameField_->getMap()->getMapId()];
		float encountRatio = mapInfo.m_EnemyEncounter > 0? (float)(encountStep_) / ((float)mapInfo.m_EnemyEncounter * 2.f + 1.f) : -1.f;
		if (encountStep_ > 0 && encountRatio >= kuto::random(1.f) && mapInfo.m_saEnemyGroup.GetSize() > 0) {
			int terrainId = gameField_->getMap()->getTerrainId(player->getPosition().x, player->getPosition().y);
			std::string terrain = getEncountBattleMap(mapInfo, terrainId);
			std::vector<int> enableEnemyIds;
			for (uint i = 0; i < mapInfo.m_saEnemyGroup.GetSize(); i++) {
				int enemyGroupId = mapInfo.m_saEnemyGroup[i].enemyGroupID;
				if (mapInfo.m_BattleMapType != 2 ||
				 (terrainId > 0 && ((int)ldb.saEnemyGroup[enemyGroupId].appearTerrain.size() <= terrainId ||
				 ldb.saEnemyGroup[enemyGroupId].appearTerrain[terrainId]))) {
					enableEnemyIds.push_back(enemyGroupId);
				}
			}
			if (!enableEnemyIds.empty()) {
				gameField_->startBattle(terrain, enableEnemyIds[kuto::random((int)enableEnemyIds.size())], false, true, true);
				encountStep_ = -1;
			}
		}
	}
 */
}

/*
bool GameEventManager::isEventConditionOk(const rpg2k::structure::InstructionCondition& condition)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	bool appear = true;
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagSwitch1) {
		if (!system.getSwitch(condition.nSw1))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagSwitch2) {
		if (!system.getSwitch(condition.nSw2))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagVar) {
		if (system.getVar(condition.nVarNum) < condition.nVarOver)
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagItem) {
		if (system.getInventory()->getItemNum(condition.nItem) == 0)
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagPlayer) {
		if (!gameField_->getPlayerFromId(condition.nChara))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagTimer) {
		if (!timer_.enable || ((timer_.count + 59) / 60) > condition.nTimer)
			appear = false;
	}
	return appear;
}
 */

void GameEventManager::updateEventAppear()
{
/*
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	rpg2k::model::Project& system = gameField_->getGameSystem();
	for (uint i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		const CRpgMapEvent& mapEvent = rpgLmu.saMapEvent[i];
		if (eventPageInfos_[i].cleared)
			continue;
		int pageIndex = 0;
		for (int iPage = mapEvent.saPage.GetSize() - 1; iPage >= 1; iPage--) {
			const EventPage& eventPage = mapEvent.saPage[iPage];
			bool appear = isEventConditionOk(eventPage.eventList.condition);
			if (appear) {
				pageIndex = iPage;
				break;
			}
		}
		// change page
		if (eventPageInfos_[i].index != pageIndex) {
			if (eventPageInfos_[i].mapChip != NULL) {
				if (mapEvent.saPage[eventPageInfos_[i].index].priority == CRpgMapEvent::kDrawPriorityNormal)
					gameField_->getCollision()->removeEventObject(eventPageInfos_[i].mapChip);
				eventPageInfos_[i].mapChip->release();
				eventPageInfos_[i].mapChip = NULL;
			}
			if (pageIndex > 0) {
				const EventPage& eventPage = mapEvent.saPage[pageIndex];
				kuto::u32 npcCrc = kuto::crc32(eventPage.strWalk);
				if (eventPageInfos_[i].npc != NULL && eventPageInfos_[i].npcCrc != npcCrc) {
					gameField_->getCollision()->removeChara(eventPageInfos_[i].npc);
					eventPageInfos_[i].npc->release();
					eventPageInfos_[i].npc = NULL;
				}
				if (!eventPage.strWalk.empty()) {
					if (eventPageInfos_[i].npc) {
						eventPageInfos_[i].npc->setEventPage(eventPage);
						eventPageInfos_[i].npc->setDirection((GameChara::DirType)eventPage.nWalkMuki);
					} else {
						GameNpc* npc = GameNpc::createTask(this, gameField_, eventPage);
						npc->setPosition(kuto::Point2(mapEvent.x, mapEvent.y));
						npc->loadWalkTexture(eventPage.strWalk, eventPage.nWalkPos);
						npc->setDirection((GameChara::DirType)eventPage.nWalkMuki);

						gameField_->getCollision()->addChara(npc);
						eventPageInfos_[i].npc = npc;
					}
				} else {
					eventPageInfos_[i].mapChip = GameEventMapChip::createTask(this, system.getLDB(), gameField_->getMap());
					eventPageInfos_[i].mapChip->setPosition(kuto::Point2(mapEvent.x, mapEvent.y));
					eventPageInfos_[i].mapChip->setPriority((CRpgMapEvent::DrawPriority)eventPage.priority);
					eventPageInfos_[i].mapChip->setPartsIndex(eventPage.nWalkPos);
					if (eventPage.priority == CRpgMapEvent::kDrawPriorityNormal)
						gameField_->getCollision()->addEventObject(eventPageInfos_[i].mapChip);
				}
				eventPageInfos_[i].npcCrc = npcCrc;
			} else {
				if (eventPageInfos_[i].npc != NULL) {
					gameField_->getCollision()->removeChara(eventPageInfos_[i].npc);
					eventPageInfos_[i].npc->release();
					eventPageInfos_[i].npc = NULL;
				}
				eventPageInfos_[i].npcCrc = 0;
			}
			eventPageInfos_[i].index = pageIndex;
		}
		// update position
		if (eventPageInfos_[i].index > 0) {
			if (eventPageInfos_[i].npc != NULL) {
				eventPageInfos_[i].x = eventPageInfos_[i].npc->getPosition().x;
				eventPageInfos_[i].y = eventPageInfos_[i].npc->getPosition().y;
			} else {
				eventPageInfos_[i].x = mapEvent.x;
				eventPageInfos_[i].y = mapEvent.y;
			}
		}
	}

	for (uint i = 1; i < system.getLDB().saCommonEvent.GetSize(); i++) {
		int pageInfoIndex = i + rpgLmu.saMapEvent.GetSize();
		const rpg2k::model::DataBase::CommonEvent& commonEvent = system.getLDB().saCommonEvent[i];
		int pageIndex = 0;
		bool appear = isEventConditionOk(commonEvent.eventList.condition);
		if (appear) {
			pageIndex = 1;
		}
		eventPageInfos_[pageInfoIndex].index = pageIndex;
	}
 */
}

void GameEventManager::updateEvent()
{
/*
	const rpg2k::moedel::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	rpg2k::model::Project& system = gameField_->getGameSystem();

	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	bool pressOk = virtualPad->press(kuto::VirtualPad::KEY_A);
	GamePlayer* player = gameField_->getPlayerLeader();
	const kuto::Point2& playerPos = player->getPosition();
	GameChara::DirType playerDir = player->getDirection();
	kuto::Point2 playerFrontPos = playerPos;
	switch (playerDir) {
	case GameChara::kDirLeft: 	playerFrontPos.x--; break;
	case GameChara::kDirRight: 	playerFrontPos.x++; break;
	case GameChara::kDirUp: 	playerFrontPos.y--; break;
	case GameChara::kDirDown: 	playerFrontPos.y++; break;
	}

	for (uint i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		const CRpgMapEvent& mapEvent = rpgLmu.saMapEvent[i];
		if (eventPageInfos_[i].cleared)
			continue;
		currentEventIndex_ = i;
		if (eventPageInfos_[i].index > 0) {
			const EventPage& eventPage = mapEvent.saPage[eventPageInfos_[i].index];
			bool isStart = false;
			switch (eventPage.eventList.condition.nStart) {
			case rpg2k::structure::InstructionCondition::kStartTypeButton:
				if (!waitEventInfo_.enable && pressOk) {
					if (eventPage.priority == CRpgMapEvent::kDrawPriorityNormal) {
						isStart = (playerFrontPos.x - eventPageInfos_[i].x == 0 && playerFrontPos.y - eventPageInfos_[i].y == 0);
						if (!isStart && gameField_->getMap()->isCounter(playerFrontPos.x, playerFrontPos.y)) {
							kuto::Point2 playerFrontFrontPos = playerFrontPos;
							switch (playerDir) {
							case GameChara::kDirLeft: 	playerFrontFrontPos.x--; break;
							case GameChara::kDirRight: 	playerFrontFrontPos.x++; break;
							case GameChara::kDirUp: 	playerFrontFrontPos.y--; break;
							case GameChara::kDirDown: 	playerFrontFrontPos.y++; break;
							}
							isStart = (playerFrontFrontPos.x - eventPageInfos_[i].x == 0 && playerFrontFrontPos.y - eventPageInfos_[i].y == 0);
						}
					} else {
						isStart = (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0);
					}
				}
				break;
			case rpg2k::structure::InstructionCondition::kStartTypeTouchPlayer:
				if (!waitEventInfo_.enable) {
					if (eventPage.priority == CRpgMapEvent::kDrawPriorityNormal) {
						isStart = (playerFrontPos.x - eventPageInfos_[i].x == 0 && playerFrontPos.y - eventPageInfos_[i].y == 0);
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0;
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultDone;
					}
					isStart = isStart && !player->isEnableRoute();
				}
				break;
			case rpg2k::structure::InstructionCondition::kStartTypeTouchEvent:
				if (!waitEventInfo_.enable && eventPageInfos_[i].npc) {
					if (eventPage.priority == CRpgMapEvent::kDrawPriorityNormal) {
						GameChara::DirType npcDir = eventPageInfos_[i].npc->getDirection();
						isStart =
							(playerPos.x - eventPageInfos_[i].x == 1 && playerPos.y - eventPageInfos_[i].y == 0 && npcDir == GameChara::kDirRight)
						||  (playerPos.x - eventPageInfos_[i].x == -1 && playerPos.y - eventPageInfos_[i].y == 0 && npcDir == GameChara::kDirLeft)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 1 && npcDir == GameChara::kDirDown)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == -1 && npcDir == GameChara::kDirUp);
						isStart = isStart && eventPageInfos_[i].npc->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0;
						isStart = isStart && eventPageInfos_[i].npc->getMoveResult() == GameChara::kMoveResultDone;
					}
					isStart = isStart && !player->isEnableRoute();
				}
				break;
			case rpg2k::structure::InstructionCondition::kStartTypeAuto:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case rpg2k::structure::InstructionCondition::kStartTypeParallel:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;

			startDecideButton_ = eventPage.eventList.condition.nStart == rpg2k::structure::InstructionCondition::kStartTypeButton;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage.eventList, 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
	for (uint i = 1; i < system.getLDB().saCommonEvent.GetSize(); i++) {
		currentEventIndex_ = i + rpgLmu.saMapEvent.GetSize();
		if (eventPageInfos_[currentEventIndex_].index > 0) {
			const rpg2k::structure::Array1D& eventPage = system.getLDB().commonEvent()[i];
			bool isStart = false;
			switch (eventPage.eventList.condition.nStart) {
			case rpg2k::structure::InstructionCondition::kStartTypeAuto:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case rpg2k::structure::InstructionCondition::kStartTypeParallel:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;

			startDecideButton_ = eventPage.eventList.condition.nStart == rpg2k::structure::InstructionCondition::kStartTypeButton;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage.eventList, 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
 */
}

void GameEventManager::executeCommands(const rpg2k::structure::Event& eventPage, int start)
{
	currentEventPage_ = &eventPage;
	// create label
	std::memset(labels_.get(), 0, labels_.size() * sizeof(int));
	for (currentCommandIndex_ = 0; currentCommandIndex_ < (int)eventPage.size(); currentCommandIndex_++) {
		const rpg2k::structure::Instruction& com = eventPage[currentCommandIndex_];
		if (com.code() == CODE_LABEL) {
			labels_[com.at(0) - 1] = currentCommandIndex_;
		}
	}
	// execute command
	for (currentCommandIndex_ = start; currentCommandIndex_ < (int)eventPage.size(); currentCommandIndex_++) {
		const rpg2k::structure::Instruction& com = eventPage[currentCommandIndex_];
		if (!conditionStack_.empty() && uint(conditionStack_.top().nest) < com.nest() && !executeChildCommands_) {
			continue;
		}
		//executeChildCommands_ = false;

		ComFuncMap::iterator it;
		if ( ( it = comFuncMap_.find(com.code()) ) != comFuncMap_.end()) {
			(this->*(it->second))(com);
			if (waitEventInfo_.enable) {
				if (backupWaitInfoEnable_) {
					// need to push to call stack??
				} else {
					waitEventInfo_.count = 0;
					waitEventInfo_.eventIndex = currentEventIndex_;
					waitEventInfo_.page = &eventPage;
					waitEventInfo_.pos = currentCommandIndex_;
					waitEventInfo_.nextPos = waitEventInfo_.pos + 1;
					waitEventInfo_.conditionStack = conditionStack_;
					waitEventInfo_.loopStack = loopStack_;
					waitEventInfo_.executeChildCommands = executeChildCommands_;

					GamePlayer* player = gameField_->getPlayerLeader();
					player->startTalking(player->getDirection());
					if (&restEventInfo_.eventListCopy != &eventPage && eventPageInfos_[currentEventIndex_].npc) {
						const kuto::Point2& playerPos = player->getPosition();
						const kuto::Point2& npcPos = eventPageInfos_[currentEventIndex_].npc->getPosition();
						rpg2k::EventDir::Type dir =
							(playerPos.x < npcPos.x) ? rpg2k::EventDir::LEFT  :
							(playerPos.x > npcPos.x) ? rpg2k::EventDir::RIGHT :
							(playerPos.y > npcPos.y) ? rpg2k::EventDir::DOWN  :
							(playerPos.y < npcPos.y) ? rpg2k::EventDir::UP    :
							rpg2k::EventDir::DOWN;
						eventPageInfos_[currentEventIndex_].npc->startTalking(dir);
					}
				}
				break;
			}
		} else {
			//kuto_printf("unknown command %x¥n", com.getEventCode());
		}
		//kuto_printf("event command %x¥n", com.getEventCode());
	}
	if (!waitEventInfo_.enable && !backupWaitInfoEnable_) {
		gameMessageWindow_->setFaceTexture("", 0, false, false);	// reset face?
	}
}

void GameEventManager::updateWaitEvent()
{
	const rpg2k::structure::Event& eventPage = *waitEventInfo_.page;
	int comStartPos = waitEventInfo_.pos;
	executeChildCommands_ = waitEventInfo_.executeChildCommands;
	conditionStack_ = waitEventInfo_.conditionStack;
	loopStack_ = waitEventInfo_.loopStack;
	routeSetChara_ = NULL;
	// execute command
	const rpg2k::structure::Instruction& com = eventPage[comStartPos];
	kuto_assert(comWaitFuncMap_.find(com.code()) != comWaitFuncMap_.end());
	(this->*(comWaitFuncMap_.find(com.code())->second))(com);

	if (!waitEventInfo_.enable) {
		gameField_->getPlayerLeader()->endTalking();
		if (&restEventInfo_.eventListCopy != &eventPage && eventPageInfos_[waitEventInfo_.eventIndex].npc)
			eventPageInfos_[waitEventInfo_.eventIndex].npc->endTalking();

		restEventInfo_.enable = false;
		currentEventIndex_ = waitEventInfo_.eventIndex;
		backupWaitInfoEnable_ = false;

		executeCommands(eventPage, waitEventInfo_.nextPos);
		while (!waitEventInfo_.enable && !callStack_.empty()) {
			restoreCallStack();
			executeCommands(*currentEventPage_, currentCommandIndex_ + 1);
		}
	}
}

GameChara* GameEventManager::getCharaFromEventId(int eventId)
{
	GameChara* chara = NULL;
	switch (eventId) {
	case 10001:		// 主人公
		chara = gameField_->getPlayerLeader();
		break;
	case 10002:		// 小型船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10003:		// 大型船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10004:		// 飛行船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10005:		// このイベント
		chara = eventPageInfos_[currentEventIndex_].npc;
		break;
	default:		// 呼び出しているマップ中のこのIDをもつマップイベント
		chara = eventPageInfos_[eventId].npc;
		break;
	}
	return chara;
}

/*
void GameEventManager::comOperateSwitch(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.at(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.at(1);
		rangeB = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getVar(com.at(1));
		rangeB = rangeA;
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.at(3) == 0) {			// 0:ONにする
			system.setSwitch(iSwitch, true);
		} else if (com.at(3) == 1) {	// 1:OFFにする
			system.setSwitch(iSwitch, false);
		} else if (com.at(3) == 2) {	// 2:ON/OFFを逆転
			system.setSwitch(iSwitch, !system.getSwitch(iSwitch));
		}
	}
}

void GameEventManager::comOperateVar(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.at(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.at(1);
		rangeB = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getVar(com.at(1));
		rangeB = rangeA;
		break;
	}
	int value = 0;
	switch (com.at(4)) {
	case 0:		// [定数] Cの値を直接適用
		value = com.at(5);
		break;
	case 1:		// [変数] V[C]
		value = system.getVar(com.at(5));
		break;
	case 2:		// [変数(参照)] V[V[C]]
		value = system.getVar(system.getVar(com.at(5)));
		break;
	case 3:		// [乱数] C〜Dの範囲の乱数
		value = kuto::random(com.at(6) + 1) + com.at(5);
		break;
	case 4:		// [アイテム]
		if (com.at(6) == 0)	// 所持数
			value = system.getInventory()->getItemNum(com.at(5));
		else {							// 装備数
			value = 0;
			for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
				const GameCharaStatus& status = gameField_->getPlayers()[i]->getStatus();
				if (status.getEquip().weapon == com.at(5) ||
				status.getEquip().shield == com.at(5) ||
				status.getEquip().protector == com.at(5) ||
				status.getEquip().helmet == com.at(5) ||
				status.getEquip().accessory == com.at(5))
					value++;
			}
		}
		break;
	case 5:		// [主人公]
		{
			const GameCharaStatus& status = gameField_->getGameSystem().getPlayerStatus(com.at(5));
			switch (com.at(6)) {
			case 0:		value = status.getLevel();		break;		// レベル
			case 1:		value = status.getExp();		break;		// 経験値
			case 2:		value = status.getHp();		break;		// HP
			case 3:		value = status.getMp();		break;		// MP
			case 4:		value = status.getBaseStatus().maxHP;		break;		// 最大HP
			case 5:		value = status.getBaseStatus().maxMP;		break;		// 最大MP
			case 6:		value = status.getBaseStatus().attack;		break;		// 攻撃力
			case 7:		value = status.getBaseStatus().defence;		break;		// 防御力
			case 8:		value = status.getBaseStatus().magic;		break;		// 精神力
			case 9:		value = status.getBaseStatus().speed;		break;		// 敏捷力
			case 10:	value = status.getEquip().weapon;		break;		// 武器No.
			case 11:	value = status.getEquip().shield;		break;		// 盾No.
			case 12:	value = status.getEquip().protector;		break;		// 鎧No.
			case 13:	value = status.getEquip().helmet;		break;		// 兜No.
			case 14:	value = status.getEquip().accessory;		break;		// 装飾品No.
			}
		}
		break;
	case 6:		// [キャラ]
		{
			GameChara* chara = getCharaFromEventId(com.at(5));
			switch (com.at(6)) {
			case 0:		// マップID
				switch (com.at(5)) {
				case 10002:		// 小型船 Undefined
				case 10003:		// 大型船 Undefined
				case 10004:		// 飛行船 Undefined
					value = 0;
					break;
				case 10001:		// 主人公
				case 10005:		// このイベント
				default:		// 呼び出しているマップ中のこのIDをもつマップイベント
					value = gameField_->getMap()->getMapId();
					break;
				}
				break;
			case 1:		// 	X座標
				if (chara)
					value = chara->getPosition().x;
				else
					value = 0;
				break;
			case 2:		// 	Y座標
				if (chara)
					value = chara->getPosition().y;
				else
					value = 0;
				break;
			case 3:		// 	向き
				if (chara) {
					switch (chara->getDirection()) {
					case GameChara::kDirUp:		value = 8;	break;
					case GameChara::kDirRight:	value = 6;	break;
					case GameChara::kDirDown:	value = 2;	break;
					case GameChara::kDirLeft:	value = 4;	break;
					}
				} else
					value = 0;
				break;
			case 4:		// 	画面X(画面上の水平座標)
				if (chara)
					value = chara->getPosition().x - gameField_->getMap()->getStartX();
				else
					value = 0;
				break;
			case 5:		// 	画面Y(画面上の垂直座標)
				if (chara)
					value = chara->getPosition().y - gameField_->getMap()->getStartY();
				else
					value = 0;
				break;
			}
		}
		break;
	case 7:		// [その他]
		if (com.at(5) == 0)		// 所持金
			value = system.getInventory()->getMoney();
		else if (com.at(5) == 1)	// タイマー1の残り秒数
			value = (timer_.count + 59) / 60;
		else if (com.at(5) == 2)	// パーティ人数
			value = gameField_->getPlayers().size();
		else if (com.at(5) == 3)	// セーブ回数
			value = system.getSaveCount();
		else if (com.at(5) == 4)	// 戦闘回数
			value = system.getBattleCount();
		else if (com.at(5) == 5)	// 勝利回数
			value = system.getWinCount();
		else if (com.at(5) == 6)	// 敗北回数
			value = system.getLoseCount();
		else if (com.at(5) == 7)	// 逃走回数
			value = system.getEscapeCount();
		else if (com.at(5) == 8)	// MIDIの演奏位置(Tick)
			value = 0;		// Undefined
		break;
	case 8:		// [敵キャラ]
		// Undefined
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.at(3) == 0) {			// 0:代入
			system.setVar(iSwitch, value);
		} else if (com.at(3) == 1) {	// 1:加算
			system.setVar(iSwitch, system.getVar(iSwitch) + value);
		} else if (com.at(3) == 2) {	// 2:減算
			system.setVar(iSwitch, system.getVar(iSwitch) - value);
		} else if (com.at(3) == 3) {	// 3:乗算
			system.setVar(iSwitch, system.getVar(iSwitch) * value);
		} else if (com.at(3) == 4) {	// 4:除算
			system.setVar(iSwitch, system.getVar(iSwitch) / value);
		} else if (com.at(3) == 5) {	// 5:剰余
			system.setVar(iSwitch, system.getVar(iSwitch) % value);
		}
	}
}

void GameEventManager::comOperateItem(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int itemId = com.at(1) == 0? com.at(2) : system.getVar(com.at(2));
	int num = com.at(3) == 0? com.at(4) : system.getVar(com.at(4));
	system.getInventory()->addItemNum(itemId, com.at(0) == 0? num : -num);
}

void GameEventManager::comOperateMoney(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int num = com.at(1) == 0? com.at(2) : system.getVar(com.at(2));
	system.getInventory()->addMoney(com.at(0) == 0? num : -num);
}

void GameEventManager::comOperateTimer(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	switch (com.at(0)) {
	case 0:		// 値の設定
		timer_.count = com.at(1) == 0? com.at(2) : system.getVar(com.at(2));
		timer_.count *= 60;		// sec to frame
		break;
	case 1:		// 作動開始
		timer_.enable = true;
		timer_.draw = (com.at(3) == 1);
		timer_.enableBattle = (com.at(4) == 1);
		break;
	case 2:		// 作動停止
		timer_.enable = false;
		break;
	}
}

void GameEventManager::comOperateJumpLabel(const rpg2k::structure::Instruction& com)
{
	currentCommandIndex_ = labels_[com.at(0) - 1];		// 戻り先で+1されるのでちょうどLabelの次になる
}

void GameEventManager::comOperatePartyChange(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int playerId = com.at(1) == 0? com.at(2) : system.getVar(com.at(2));
	if (com.at(0) == 0) {		// メンバーを加える
		gameField_->addPlayer(playerId);
	} else {							// メンバーを外す
		gameField_->removePlayer(playerId);
	}
}

void GameEventManager::comOperateLocateMove(const rpg2k::structure::Instruction& com)
{
	int dir = com.atNum() >= 4? com.at(3) : 0;
	gameField_->changeMap(com.at(0), com.at(1), com.at(2), dir);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitLocateMove(const rpg2k::structure::Instruction& com)
{
	waitEventInfo_.enable = false;
}

void GameEventManager::comOperateLocateSave(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.setVar(com.at(0), gameField_->getMap()->getMapId());
	system.setVar(com.at(1), gameField_->getPlayerLeader()->getPosition().x);
	system.setVar(com.at(2), gameField_->getPlayerLeader()->getPosition().y);
}

void GameEventManager::comOperateLocateLoad(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.setVar(com.at(0), gameField_->getMap()->getMapId());
	system.setVar(com.at(1), gameField_->getPlayerLeader()->getPosition().x);
	system.setVar(com.at(2), gameField_->getPlayerLeader()->getPosition().y);
	gameField_->changeMap(system.getVar(com.at(0)), system.getVar(com.at(1)), system.getVar(com.at(2)), 0);
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateTextShow(const rpg2k::structure::Instruction& com)
{
	openGameMassageWindow();
	gameMessageWindow_->addLine(com.getString());
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->events.size(); i++) {
		const rpg2k::structure::Instruction& comNext = currentEventPage_->events[i];
		if (comNext.getEventCode() == CODE_TXT_SHOW_ADD) {
			gameMessageWindow_->addLine(comNext.getString());
		} else {
			break;
		}
	}

	waitEventInfo_.enable = true;
}

void GameEventManager::openGameMassageWindow()
{
	gameMessageWindow_->freeze(false);
	gameMessageWindow_->open();
	gameMessageWindow_->reset();
	gameMessageWindow_->clearMessages();
	gameMessageWindow_->setShowFrame(messageWindowSetting_.showFrame);
	switch (messageWindowSetting_.pos) {
	case MessageWindowSetting::kPosTypeUp:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 0.f));
		break;
	case MessageWindowSetting::kPosTypeCenter:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 80.f));
		break;
	default:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
		break;
	}
	if (messageWindowSetting_.autoMove) {
		// Undefined
	}
}

void GameEventManager::comWaitTextShow(const rpg2k::structure::Instruction& com)
{
	if (gameMessageWindow_->closed()) {
		waitEventInfo_.enable = false;
		gameMessageWindow_->freeze(true);
	}
}

void GameEventManager::comOperateTextOption(const rpg2k::structure::Instruction& com)
{
	messageWindowSetting_.showFrame = !com.at(0);
	messageWindowSetting_.pos = (MessageWindowSetting::PosType)com.at(1);
	messageWindowSetting_.autoMove = com.at(2);
	messageWindowSetting_.enableOtherEvent = com.at(3);
}

void GameEventManager::comOperateTextFace(const rpg2k::structure::Instruction& com)
{
	gameMessageWindow_->setFaceTexture(com.getString(), com.at(0), (bool)com.at(1), (bool)com.at(2));
	selectWindow_->setFaceTexture(com.getString(), com.at(0), (bool)com.at(1), (bool)com.at(2));
}

void GameEventManager::comOperateBattleStart(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	std::string terrain;
	if (com.at(2) == 0) {
		int terrainId = gameField_->getMap()->getTerrainId(eventPageInfos_[currentEventIndex_].x, eventPageInfos_[currentEventIndex_].y);
		terrain = system.getLDB().saTerrain[terrainId].battleGraphic;
	} else {
		terrain = com.getString();
	}
	int enemyId = com.at(0) == 0? com.at(1) : system.getVar(com.at(1));
	gameField_->startBattle(terrain, enemyId, (bool)com.at(5), com.at(3) != 0, com.at(4) == 0);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitBattleStart(const rpg2k::structure::Instruction& com)
{
	waitEventInfo_.enable = false;
	int result = gameField_->getBattleResult();
	if (com.at(3) == 1 && result == GameBattle::kResultEscape) {
		comOperateEventBreak(com);		// Escape -> Break
	} else if (com.at(3) == 2 || com.at(4) == 1) {
		conditionStack_.push(ConditionInfo(com.getNest(), result));
	}
}

void GameEventManager::comOperateBattleWin(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultWin;
}

void GameEventManager::comOperateBattleEscape(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultEscape;
}

void GameEventManager::comOperateBattleLose(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultLose;
}

void GameEventManager::comOperateBranchEnd(const rpg2k::structure::Instruction& com)
{
	conditionStack_.pop();
	executeChildCommands_ = true;
}

void GameEventManager::comOperateIfStart(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int condValue = 0;
	switch (com.at(0)) {
	case 0:		// 0:スイッチ
		condValue = system.getSwitch(com.at(1)) == (com.at(2) == 0? true : false);
		break;
	case 1:		// 1:変数
		{
			int value = com.at(2) == 0? com.at(3) : system.getVar(com.at(3));
			switch (com.at(4)) {
			case 0:		// と同値
				condValue = (system.getVar(com.at(1)) == value);
				break;
			case 1:		// 	以上
				condValue = (system.getVar(com.at(1)) >= value);
				break;
			case 2:		// 	以下
				condValue = (system.getVar(com.at(1)) <= value);
				break;
			case 3:		// より大きい
				condValue = (system.getVar(com.at(1)) > value);
				break;
			case 4:		// より小さい
				condValue = (system.getVar(com.at(1)) < value);
				break;
			case 5:		// 以外
				condValue = (system.getVar(com.at(1)) != value);
				break;
			}
		}
		break;
	case 2:		// 2:タイマー1
		switch (com.at(2)) {
		case 0:		// 以上
			condValue = (timer_.enable? timer_.count >= com.at(1) * 60 : false);
			break;
		case 1:		// 以下
			condValue = (timer_.enable? timer_.count <= com.at(1) * 60 : false);
			break;
		}
		break;
	case 3:		// 3:所持金
		switch (com.at(2)) {
		case 0:		// 以上
			condValue = (system.getInventory()->getMoney() >= com.at(1));
			break;
		case 1:		// 以下
			condValue = (system.getInventory()->getMoney() <= com.at(1));
			break;
		}
		break;
	case 4:		// 4:アイテム
		switch (com.at(2)) {
		case 0:		// 持っている
			condValue = (system.getInventory()->getItemNum(com.at(1)) > 0);
			break;
		case 1:		// 持っていない
			condValue = (system.getInventory()->getItemNum(com.at(1)) == 0);
			break;
		}
		break;
	case 5:		// 5:主人公
		switch (com.at(2)) {
		case 0:		// パーティにいる
			condValue = gameField_->getPlayerFromId(com.at(1)) != NULL;
			break;
		case 1:		// 主人公の名前が文字列引数と等しい
			condValue = com.getString() == gameField_->getGameSystem().getPlayerInfo(com.at(1)).name;
			break;
		case 2:		// レベルがCの値以上
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getLevel() >= com.at(3);
			break;
		case 3:		// HPがCの値以上
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getHp() >= com.at(3);
			break;
		case 4:		// 特殊技能IDがCの値の特殊技能を使用できる
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).isLearnedSkill(com.at(3));
			break;
		case 5:		// アイテムIDがCの値のアイテムを装備している
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip().weapon == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip().shield == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip().protector == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip().helmet == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip().accessory == com.at(3);
			break;
		case 6:		// 状態IDがCの状態になっている
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getBadConditionIndex(com.at(3)) >= 0;
			break;
		}
		break;
	case 6:		// 6:キャラの向き
		{
			GameChara* chara = getCharaFromEventId(com.at(1));
			if (chara) {
				condValue = chara->getDirection() == com.at(2);
			} else
				condValue = false;
		}
		break;
	case 7:		// 7:乗り物
		condValue = (false);	// Undefined
		break;
	case 8:		// 8:決定キーでこのイベントを開始した
		condValue = startDecideButton_;
		break;
	case 9:		// 9:演奏中のBGMが一周した
		condValue = bgm_->isLooped();	// Undefined
		break;
	}
	conditionStack_.push(ConditionInfo(com.getNest(), condValue));
	executeChildCommands_ = conditionStack_.top().value == true;
}

void GameEventManager::comOperateIfElse(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

void GameEventManager::openGameSelectWindow()
{
	selectWindow_->freeze(false);
	selectWindow_->open();
	selectWindow_->reset();
	selectWindow_->resetCursor();
	selectWindow_->clearMessages();
	selectWindow_->setShowFrame(messageWindowSetting_.showFrame);
	switch (messageWindowSetting_.pos) {
	case MessageWindowSetting::kPosTypeUp:
		selectWindow_->setPosition(kuto::Vector2(0.f, 0.f));
		break;
	case MessageWindowSetting::kPosTypeCenter:
		selectWindow_->setPosition(kuto::Vector2(0.f, 80.f));
		break;
	default:
		selectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
		break;
	}
	if (messageWindowSetting_.autoMove) {
		// Undefined
	}
}

void GameEventManager::comOperateSelectStart(const rpg2k::structure::Instruction& com)
{
	openGameSelectWindow();
	std::string::size_type oldPos = 0;
	for (int i = 0; i < 4; i++) {
		std::string::size_type pos = com.getString().find('/', oldPos);
		if (pos == std::string::npos) {
			selectWindow_->addLine(com.getString().substr(oldPos));
			break;
		} else {
			selectWindow_->addLine(com.getString().substr(oldPos, pos - oldPos));
			oldPos = pos + 1;
		}
	}
	selectWindow_->setEnableCancel(com.at(0) != 0);

	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitSelectStart(const rpg2k::structure::Instruction& com)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled()) {
			selectIndex = com.at(0) - 1;
		}
		int value = selectIndex < (int)selectWindow_->getMessageSize()? kuto::crc32(selectWindow_->getMessage(selectIndex).str) : 0;
		conditionStack_.push(ConditionInfo(com.getNest(), value));
	}
}

void GameEventManager::comOperateSelectCase(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = (uint)conditionStack_.top().value == kuto::crc32(com.getString());
}

void GameEventManager::comOperateGameOver(const rpg2k::structure::Instruction& com)
{
	gameField_->gameOver();
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateReturnTitle(const rpg2k::structure::Instruction& com)
{
	gameField_->returnTitle();
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateEventBreak(const rpg2k::structure::Instruction& com)
{
	currentCommandIndex_ = 100000;		// 100000行もないだろ
}

void GameEventManager::comOperateEventClear(const rpg2k::structure::Instruction& com)
{
	eventPageInfos_[currentEventIndex_].cleared = true;
	if (eventPageInfos_[currentEventIndex_].npc) {
		eventPageInfos_[currentEventIndex_].npc->freeze(true);
	}
}

void GameEventManager::comOperateLoopStart(const rpg2k::structure::Instruction& com)
{
	LoopInfo info;
	info.startIndex = currentCommandIndex_;
	info.conditionSize = conditionStack_.size();
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->events.size(); i++) {
		const rpg2k::structure::Instruction& comNext = currentEventPage_->events[i];
		if (com.getNest() != comNext.getNest()) {
			continue;
		}
		if (comNext.getEventCode() == CODE_LOOP_END) {
			info.endIndex = i;
			break;
		}
	}
	loopStack_.push(info);
}

void GameEventManager::comOperateLoopBreak(const rpg2k::structure::Instruction& com)
{
	while (loopStack_.top().conditionSize < (int)conditionStack_.size()) {
		conditionStack_.pop();
	}
	currentCommandIndex_ = loopStack_.top().endIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
	loopStack_.pop();
}

void GameEventManager::comOperateLoopEnd(const rpg2k::structure::Instruction& com)
{
	currentCommandIndex_ = loopStack_.top().startIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
}

void GameEventManager::comOperateWait(const rpg2k::structure::Instruction& com)
{
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitWait(const rpg2k::structure::Instruction& com)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(0) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

static void setPictureInfo(GameEventPicture::Info& info, const rpg2k::structure::Instruction& com, GameField* field)
{
	rpg2k::model::Project& system = field->getGameSystem();
	info.map = field->getMap();
	info.position.x = com.at(1) == 0? com.at(2) : system.getVar(com.at(2));
	info.position.y = com.at(1) == 0? com.at(3) : system.getVar(com.at(3));
	info.scroll = (bool)com.at(4);
	info.scale = (float)com.at(5) / 100.f;
	info.color.a = 1.f - (float)com.at(6) / 100.f;
	info.useAlpha = (bool)com.at(7);
	info.color.r = (float)com.at(8) / 100.f;
	info.color.g = (float)com.at(9) / 100.f;
	info.color.b = (float)com.at(10) / 100.f;
	info.saturation = (float)com.at(11) / 100.f;
	info.effect = (GameEventPicture::EffectType)com.at(12);
	info.effectSpeed = com.at(13);
}

void GameEventManager::comOperatePictureShow(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
	GameEventPicture::Info info;
	setPictureInfo(info, com, gameField_);

	std::string filename = system.getGameDir();
	filename += "/Picture/" + com.getString();
	pictures_[picIndex] = GameEventPicture::createTask(this, filename, info);
	pictures_[picIndex]->setPriority(1.f + (float)picIndex * -0.0001f);
}

void GameEventManager::comOperatePictureMove(const rpg2k::structure::Instruction& com)
{
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]) {
		return;
	}
	GameEventPicture::Info info;
	setPictureInfo(info, com, gameField_);

	pictures_[picIndex]->move(info, com.at(14) * 60 / 10);
	waitEventInfo_.enable = (bool)com.at(15);
}

void GameEventManager::comWaitPictureMove(const rpg2k::structure::Instruction& com)
{
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]->isMoving()) {
		waitEventInfo_.enable = false;
	}
}

void GameEventManager::comOperatePictureClear(const rpg2k::structure::Instruction& com)
{
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
}

void GameEventManager::comOperateFadeType(const rpg2k::structure::Instruction& com)
{
	gameField_->setFadeInfo(com.at(0), com.at(1));
}

void GameEventManager::comOperateFadeOut(const rpg2k::structure::Instruction& com)
{
	gameField_->fadeOut(com.at(0));
}

void GameEventManager::comOperateFadeIn(const rpg2k::structure::Instruction& com)
{
	gameField_->fadeIn(com.at(0));
}

void GameEventManager::comOperateMapScroll(const rpg2k::structure::Instruction& com)
{
	GameMap* map = gameField_->getMap();
	if (com.at(0) < 2)
		map->setEnableScroll((bool)com.at(0));
	else {
		if (com.at(0) == 2) {
			int x = 0;
			int y = 0;
			if (com.at(1) == 0)	// 上
				y = -com.at(2);
			else if (com.at(1) == 1)	// 右
				x = com.at(2);
			else if (com.at(1) == 2)	// 下
				y = com.at(2);
			else if (com.at(1) == 3)	// 左
				x = -com.at(2);
			map->scroll(x, y, (float)(1 << com.at(3)));
		} else
			map->scrollBack((float)(1 << com.at(3)));
		if (com.at(4) == 1);
			waitEventInfo_.enable = true;
	}
}

void GameEventManager::comWaitMapScroll(const rpg2k::structure::Instruction& com)
{
	if (!gameField_->getMap()->isScrolling())
		waitEventInfo_.enable = false;
}

void GameEventManager::comOperatePlayerVisible(const rpg2k::structure::Instruction& com)
{
	gameField_->getPlayerLeader()->setVisible((bool)com.at(0));
}

void GameEventManager::addLevelUpMessage(const GameCharaStatus& status, int oldLevel)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	const rpg2k::structure::Array1D& term = system.getLDB().vocabulary();
	const GamePlayerInfo& player = system.getPlayerInfo(status.getCharaId());
	char temp[256];
	sprintf(temp, "%sは%s%d%s", player.name.c_str(), term.param.level.c_str(),
		status.getLevel(), term.battle.levelUp.c_str());
	gameMessageWindow_->addLine(temp);
	for (uint iLearn = 1; iLearn < player.baseInfo->learnSkill.size(); iLearn++) {
		const rpg2k::model::DataBase::LearnSkill& learnSkill = player.baseInfo->learnSkill[iLearn];
		if (learnSkill.level > oldLevel && learnSkill.level <= status.getLevel()) {
			gameMessageWindow_->addLine(system.getLDB().skill[learnSkill.skill][1].get_string() + ldb.vocabulary(37));
		}
	}
}

void GameEventManager::comOperatePlayerCure(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	for (uint i = 0; i < statusList.size(); i++) {
		statusList[i]->fullCure();
	}
}

void GameEventManager::comOperateAddExp(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	int exp = com.at(3) == 0? com.at(4) : system.getVar(com.at(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (uint i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addExp(com.at(2) == 0? exp : -exp);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.at(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

void GameEventManager::comOperateAddLevel(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	int level = com.at(3) == 0? com.at(4) : system.getVar(com.at(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (uint i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addLevel(com.at(2) == 0? level : -level);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.at(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

void GameEventManager::comOperateAddStatus(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	int upParam = com.at(4) == 0? com.at(5) : system.getVar(com.at(5));
	for (uint i = 0; i < statusList.size(); i++) {
		rpg2k::model::DataBase::Status itemUp;
		std::memset(&itemUp, 0, sizeof(itemUp));
		switch (com.at(3)) {
		case 0:		// 0:最大HP
			itemUp.maxHP = com.at(2) == 0? upParam : -upParam;
			break;
		case 1:		// 1:最大MP
			itemUp.maxMP = com.at(2) == 0? upParam : -upParam;
			break;
		case 2:		// 2:攻撃力
			itemUp[rpg2k::Param::ATTACK] = com.at(2) == 0? upParam : -upParam;
			break;
		case 3:		// 3:防御力
			itemUp.defence = com.at(2) == 0? upParam : -upParam;
			break;
		case 4:		// 4:精神力
			itemUp.magic = com.at(2) == 0? upParam : -upParam;
			break;
		case 5:		// 5:敏捷性
			itemUp.speed = com.at(2) == 0? upParam : -upParam;
			break;
		}
		statusList[i]->addItemUp(itemUp);
	}
}

void GameEventManager::comOperateAddSkill(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	int skillId = com.at(3) == 0? com.at(4) : system.getVar(com.at(4));
	for (uint i = 0; i < statusList.size(); i++) {
		if (com.at(2) == 0)
			statusList[i]->learnSkill(skillId);
		else
			statusList[i]->forgetSkill(skillId);
	}
}

void GameEventManager::comOperateCallEvent(const rpg2k::structure::Instruction& com)
{
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	rpg2k::model::Project& system = gameField_->getGameSystem();
	// backup
	{
		CallEventInfo info;
		info.eventIndex = currentEventIndex_;
		info.executeChildCommands = executeChildCommands_;
		info.page = currentEventPage_;
		std::memcpy(info.labels.get(), labels_.get(), labels_.size() * sizeof(int));
		info.pos = currentCommandIndex_;
		info.conditionStack = conditionStack_;
		info.loopStack = loopStack_;
		callStack_.push(info);
	}
	// call
	executeChildCommands_ = false;
	conditionStack_.clear();
	loopStack_.clear();
	if (com.at(0) == 0) {
		// common event
		int eventId = com.at(1);
		currentEventIndex_ = eventId + rpgLmu.saMapEvent.GetSize();
		executeCommands(system.getLDB().saCommonEvent[eventId].eventList, 0);
	} else {
		// map event
		int eventId = (com.at(0) == 1)? com.at(1) : system.getVar(com.at(1));
		int eventPage = (com.at(0) == 1)? com.at(2) : system.getVar(com.at(2));
		if (eventId == 10005)
			eventId = currentEventIndex_;
		currentEventIndex_ = eventId;
		executeCommands(rpgLmu.saMapEvent[eventId].saPage[eventPage].eventList, 0);
	}
	if (waitEventInfo_.enable) {
		backupWaitInfoEnable_ = true;
	} else {
		// restore
		restoreCallStack();
	}
}
 */

void GameEventManager::restoreCallStack()
{
	const CallEventInfo& info = callStack_.top();
	currentEventIndex_ = info.eventIndex;
	executeChildCommands_ = info.executeChildCommands;
	currentEventPage_ = info.page;
	std::memcpy(labels_.get(), info.labels.get(), labels_.size() * sizeof(int));
	currentCommandIndex_ = info.pos;
	conditionStack_ = info.conditionStack;
	loopStack_ = info.loopStack;
	callStack_.pop();
}

/*
void GameEventManager::comOperateRoute(const rpg2k::structure::Instruction& com)
{
	GameChara* chara = getCharaFromEventId(com.at(0));
	if (!chara)
		return;
	// int frequency = com.at(1);
	CRpgRoute route;
	route.repeat = com.at(2) == 1;
	route.ignore = com.at(3) == 1;
	for (int i = 4; i < com.atNum(); i++) {
		route.commands.push_back(com.at(i));
		switch (com.at(i)) {
		case 32:	// スイッチON
		case 33:	// スイッチOFF
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 34:	// グラフィック変更
			route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 35:	// 効果音の演奏
			route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 1));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 2));
			break;
		}
	}
	chara->setRoute(route);
	routeSetChara_ = chara;
	routeSetChara_->startRoute();
}

void GameEventManager::comOperateRouteStart(const rpg2k::structure::Instruction& com)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->startRoute();
}

void GameEventManager::comOperateRouteEnd(const rpg2k::structure::Instruction& com)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->endRoute();
}

void GameEventManager::comOperateNameInput(const rpg2k::structure::Instruction& com)
{
	nameInputMenu_->freeze(false);
	nameInputMenu_->setPlayerInfo(com.at(0), (bool)com.at(1), (bool)com.at(2));
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitNameInput(const rpg2k::structure::Instruction& com)
{
	if (nameInputMenu_->closed()) {
		waitEventInfo_.enable = false;
		nameInputMenu_->freeze(true);
	}
}

void GameEventManager::comOperatePlayerNameChange(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.getPlayerInfo(com.at(0)).name = com.getString();
}

void GameEventManager::comOperatePlayerTitleChange(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.getPlayerInfo(com.at(0)).title = com.getString();
}

void GameEventManager::comOperatePlayerWalkChange(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int playerId = com.at(0);
	GamePlayerInfo& playerInfo = system.getPlayerInfo(playerId);
	playerInfo.walkGraphicName = com.getString();
	playerInfo.walkGraphicPos = com.at(1);
	playerInfo.walkGraphicSemi = (bool)com.at(2);

	GamePlayer* player = gameField_->getPlayerFromId(playerId);
	if (player) {
		player->loadWalkTexture(playerInfo.walkGraphicName, playerInfo.walkGraphicPos);
	}
}

void GameEventManager::comOperatePlayerFaceChange(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int playerId = com.at(0);
	GamePlayerInfo& playerInfo = system.getPlayerInfo(playerId);
	playerInfo.faceGraphicName = com.getString();
	playerInfo.faceGraphicPos = com.at(1);

	GamePlayer* player = gameField_->getPlayerFromId(playerId);
	if (player) {
		player->loadFaceTexture(playerInfo.faceGraphicName, playerInfo.faceGraphicPos);
	}
}

void GameEventManager::comOperateBgm(const rpg2k::structure::Instruction& com)
{
	// Undefined
}

int getInputKeyValue(const rpg2k::structure::Instruction& com)
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	int key = 0;
	if (com.at(2) == 1) {
		if (virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	} else {
		if (com.at(5) == 1 && virtualPad->press(kuto::VirtualPad::KEY_X))
			key = 1;
		if (com.at(6) == 1 && virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (com.at(7) == 1 && virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (com.at(8) == 1 && virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (com.at(9) == 1 && virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	}
	if (com.at(3) == 1 && virtualPad->press(kuto::VirtualPad::KEY_A))
		key = 5;
	if (com.at(4) == 1 && virtualPad->press(kuto::VirtualPad::KEY_B))
		key = 6;
	return key;
}

void GameEventManager::comOperateKey(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	waitEventInfo_.enable = com.at(1);
	if (!waitEventInfo_.enable)
		system.setVar(com.at(0), getInputKeyValue(com));
}

void GameEventManager::comWaitKey(const rpg2k::structure::Instruction& com)
{
	int key = getInputKeyValue(com);
	if (key != 0) {
		waitEventInfo_.enable = false;
		rpg2k::model::Project& system = gameField_->getGameSystem();
		system.setVar(com.at(0), key);
	}
}

void GameEventManager::comOperatePanorama(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::MapUnit::PanoramaInfo info;
	info.enable = true;
	info.name = com.getString();
	info.loopHorizontal = (com.at(0) == 1);
	info.loopVertical = (com.at(1) == 1);
	info.scrollHorizontal = (com.at(2) == 1);
	info.scrollSpeedHorizontal = com.at(3);
	info.scrollVertical = (com.at(4) == 1);
	info.scrollSpeedVertical = com.at(5);
	gameField_->getMap()->getRpgLmu().SetPanoramaInfo(info);
}

void GameEventManager::comOperateInnStart(const rpg2k::structure::Instruction& com)
{
	openGameSelectWindow();
	const rpg2k::model::Project& system = gameField_->getGameSystem();
	const rpg2k::model::DataBase& ldb = system.getLDB();
	const rpg2k::model::DataBase::InnTerm& innTerm = ldb.term.inn[com.at(0)];
	std::string mes = innTerm.what[0];
	std::ostringstream oss;
	oss << com.at(1);
	mes += oss.str();
	mes += ldb.term.shopParam.money;
	mes += innTerm.what[1];
	selectWindow_->addLine(mes);
	selectWindow_->addLine(innTerm.what[2]);
	selectWindow_->addLine(innTerm.ok, system.getInventory()->getMoney() >= com.at(1));
	selectWindow_->addLine(innTerm.cancel);
	selectWindow_->setCursorStart(2);
	selectWindow_->setEnableCancel(true);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitInnStart(const rpg2k::structure::Instruction& com)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled())
			selectIndex = 3;
		if (selectIndex == 2) {
			gameField_->getGameSystem().getInventory()->addMoney(-com.at(1));
			for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
				gameField_->getPlayers()[i]->getStatus().fullCure();
			}
		}
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.getNest(), selectIndex == 2));
	}
}

void GameEventManager::comOperateInnOk(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == true;
}

void GameEventManager::comOperateInnCancel(const rpg2k::structure::Instruction& com)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

void GameEventManager::comOperateShopStart(const rpg2k::structure::Instruction& com)
{
	shopMenu_->freeze(false);
	int shopType = com.at(0);
	int mesType = com.at(1);
	std::vector<int> items;
	for (int i = 4; i < com.atNum(); i++)
		items.push_back(com.at(i));
	shopMenu_->setShopData(shopType, mesType, items);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitShopStart(const rpg2k::structure::Instruction& com)
{
	if (shopMenu_->closed()) {
		shopMenu_->freeze(true);
		waitEventInfo_.enable = false;
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.getNest(), shopMenu_->buyOrSell()));
	}
}

void GameEventManager::comOperatePlaySound(const rpg2k::structure::Instruction& com)
{
	// Undefined
}

void GameEventManager::comOperateScreenColor(const rpg2k::structure::Instruction& com)
{
	// Undefined
	waitEventInfo_.enable = (com.at(5) == 1);
}

void GameEventManager::comWaitScreenColor(const rpg2k::structure::Instruction& com)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(4) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

void GameEventManager::comOperateBattleAnime(const rpg2k::structure::Instruction& com)
{
	GameSkillAnime* anime = GameSkillAnime::createTask(this, gameField_->getGameSystem(), com.at(0));
	int eventId = com.at(1);
	GameChara* chara = getCharaFromEventId(eventId);
	if (chara) {
		anime->setPlayPosition(kuto::Vector2(chara->getPosition().x * 16.f, chara->getPosition().y * 16.f));
	} else {
		anime->setPlayPosition(kuto::Vector2(eventPageInfos_[eventId].x * 16.f, eventPageInfos_[eventId].y * 16.f));
	}
	waitEventInfo_.enable = (com.at(2) == 1);
	if (waitEventInfo_.enable)
		skillAnime_ = anime;
	else
		anime->setDeleteFinished(true);
	anime->play();
}

void GameEventManager::comWaitBattleAnime(const rpg2k::structure::Instruction& com)
{
	if (skillAnime_->isFinished()) {
		skillAnime_->release();
		skillAnime_ = NULL;
		waitEventInfo_.enable = false;
	}
}

void GameEventManager::comOperateEquip(const rpg2k::structure::Instruction& com)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.at(1))));
		break;
	}
	for (uint i = 0; i < statusList.size(); i++) {
		rpg2k::model::DataBase::Equip equip = statusList[i]->getEquip();
		if (com.at(2) == 0) {
			// 装備変更
			int itemId = com.at(3) == 0? com.at(4) : system.getVar(com.at(4));
			switch (system.getLDB().saItem[itemId].type) {
			case rpg2k::model::DataBase::kItemTypeWeapon:
				equip.weapon = itemId;
				break;
			case rpg2k::model::DataBase::kItemTypeShield:
				equip.shield = itemId;
				break;
			case rpg2k::model::DataBase::kItemTypeProtector:
				equip.protector = itemId;
				break;
			case rpg2k::model::DataBase::kItemTypeHelmet:
				equip.helmet = itemId;
				break;
			case rpg2k::model::DataBase::kItemTypeAccessory:
				equip.accessory = itemId;
				break;
			}
		} else {
			// 装備外す
			switch (com.at(3)) {
			case 0:
				equip.weapon = 0;
				break;
			case 1:
				equip.shield = 0;
				break;
			case 2:
				equip.protector = 0;
				break;
			case 3:
				equip.helmet = 0;
				break;
			case 4:
				equip.accessory = 0;
				break;
			case 5:
				equip.weapon = 0;
				equip.shield = 0;
				equip.protector = 0;
				equip.helmet = 0;
				equip.accessory = 0;
				break;
			}
		}
		if (equip.weapon != statusList[i]->getEquip().weapon && statusList[i]->getEquip().weapon != 0)
			system.getInventory()->addItemNum(statusList[i]->getEquip().weapon, 1);
		if (equip.shield != statusList[i]->getEquip().shield && statusList[i]->getEquip().shield != 0)
			system.getInventory()->addItemNum(statusList[i]->getEquip().shield, 1);
		if (equip.protector != statusList[i]->getEquip().protector && statusList[i]->getEquip().protector != 0)
			system.getInventory()->addItemNum(statusList[i]->getEquip().protector, 1);
		if (equip.helmet != statusList[i]->getEquip().helmet && statusList[i]->getEquip().helmet != 0)
			system.getInventory()->addItemNum(statusList[i]->getEquip().helmet, 1);
		if (equip.accessory != statusList[i]->getEquip().accessory && statusList[i]->getEquip().accessory != 0)
			system.getInventory()->addItemNum(statusList[i]->getEquip().accessory, 1);

		statusList[i]->setEquip(equip);
	}
}
 */


void GameEventManager::draw()
{
	if (timer_.draw) {
		// Undefined
	}
}
