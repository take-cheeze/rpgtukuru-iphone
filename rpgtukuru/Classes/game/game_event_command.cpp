#include <deque>
#include <sstream>
#include <utility>

#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_virtual_pad.h>

#include "game.h"
#include "game_battle.h"
#include "game_bgm.h"
#include "game_event_manager.h"
#include "game_event_command.h"
#include "game_event_map_chip.h"
#include "game_event_picture.h"
#include "game_collision.h"
#include "game_field.h"
#include "game_inventory.h"
#include "game_map.h"
#include "game_message_window.h"
#include "game_name_input_menu.h"
#include "game_npc.h"
#include "game_player.h"
#include "game_select_window.h"
#include "game_shop_menu.h"
#include "game_skill_anime.h"

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;
using rpg2k::structure::EventState;
using rpg2k::model::DataBase;
using rpg2k::model::SaveData;


namespace
{
	void setPictureInfo(SaveData& lsd, rpg2k::structure::Instruction const& com)
	{
		Array1D& dst = lsd.picture()[com.at(0)];
		// coord
		int x = com.at(1) == 0? com.at(2) : lsd.getVar(com.at(2));
		int y = com.at(1) == 0? com.at(3) : lsd.getVar(com.at(3));
		if( com.code() == CODE_PICT_SHOW ) {
			dst[1] = com.getString();
			dst[6] = bool(com.at(4)); // scroll
			dst[9] = bool(com.at(7)); // useAlpha
			// dst[] = com.at(14); // alpha(lower)

			dst[2] = dst[5] = x;
			dst[3] = dst[6] = y;

			dst[51] = 0;
		} else if( com.code() == CODE_PICT_MOVE ) {
			dst[51] = com.at(14); // set counter
			// dst[] = com.at(15); // wait setting

			dst[31] = x;
			dst[32] = y;

			dst[11] = double( dst[41].get<int>() ); // red
			dst[12] = double( dst[42].get<int>() ); // green
			dst[13] = double( dst[43].get<int>() ); // blue
			dst[14] = double( dst[44].get<int>() ); // chroma
		} else kuto_assert(false);
		dst[33] = com.at( 5); // scale
		dst[34] = com.at( 6); // alpha(upper)

		dst[41] = com.at( 8); // red
		dst[42] = com.at( 9); // green
		dst[43] = com.at(10); // blue
		dst[44] = com.at(11); // chroma

		dst[15] = com.at(12); // effect type
		dst[16] = double( com.at(13) ); // speed or power
	}

	namespace EventKey { enum Type {
		NONE = 0,
		DOWN = 1, LEFT = 2, RIGHT = 3, UP = 4,
		ENTER = 5, CANCEL = 6, SHIFT = 7,
		#if defined(RPG2003)
			NUM_0 = 10, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
			PLUS = 20, MINUS = 21, MULTIPLY = 22, DIVIDE = 23, DOT = 24,
		#endif
	}; }
	EventKey::Type getInputKeyValue(const rpg2k::structure::Instruction& com)
	{
		kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
		EventKey::Type key = EventKey::NONE;
		if (com.at(3) && virtualPad->press(kuto::VirtualPad::KEY_A))
			key = EventKey::ENTER;
		if (com.at(4) && virtualPad->press(kuto::VirtualPad::KEY_B))
			key = EventKey::CANCEL;
		if ( com.at(2) ) {
			#if( defined(RPG2000) || defined(RPG2000_VALUE) )
				if (virtualPad->press(kuto::VirtualPad::KEY_DOWN))
					key = EventKey::DOWN;
				if (virtualPad->press(kuto::VirtualPad::KEY_LEFT))
					key = EventKey::LEFT;
				if (virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
					key = EventKey::RIGHT;
				if (virtualPad->press(kuto::VirtualPad::KEY_UP))
					key = EventKey::UP;
			#elif defined(RPG2003)
				/*
				if (com.at( 5) && ) // number
				if (com.at( 6) && ) // symbol
				if ( com.at( 7) ) { // get time until key is pressed
					com.at(8) // <- variable ID the time(1/10 second) will be written
				}
				 */
				if (com.at( 9) && virtualPad->press(kuto::VirtualPad::KEY_X))
					key = EventKey::SHIFT;
				if (com.at(10) && virtualPad->press(kuto::VirtualPad::KEY_DOWN))
					key = EventKey::DOWN;
				if (com.at(11) && virtualPad->press(kuto::VirtualPad::KEY_LEFT))
					key = EventKey::LEFT;
				if (com.at(12) && virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
					key = EventKey::RIGHT;
				if (com.at(13) && virtualPad->press(kuto::VirtualPad::KEY_UP))
					key = EventKey::UP;
			#else
				#error
			#endif
		}
		#if ( defined(RPG2000_VALUE) || defined(RPG2003) )
		else {
			if (com.at(5) && virtualPad->press(kuto::VirtualPad::KEY_X))
				key = EventKey::SHIFT;
			if (com.at(6) && virtualPad->press(kuto::VirtualPad::KEY_DOWN))
				key = EventKey::DOWN;
			if (com.at(7) && virtualPad->press(kuto::VirtualPad::KEY_LEFT))
				key = EventKey::LEFT;
			if (com.at(8) && virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
				key = EventKey::RIGHT;
			if (com.at(9) && virtualPad->press(kuto::VirtualPad::KEY_UP))
				key = EventKey::UP;
		}
		#endif
		return key;
	}
}

void GameEventManager::commandDummy(const rpg2k::structure::Instruction&)
{
}
void GameEventManager::commandWaitDummy(const rpg2k::structure::Instruction&)
{
}

PP_protoType(CODE_OPERATE_SWITCH)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	std::pair<int, int> range(0, 0);
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		range.first = range.second = com.at(1);
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		range.first = com.at(1);
		range.second = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		range.first = range.second = system.getLSD().getVar(com.at(1));
		break;
	}
	for (int iSwitch = range.first; iSwitch <= range.second; iSwitch++) {
	switch( com.at(3) ) {
		case 0: // 0:ONにする
			system.getLSD().setFlag(iSwitch, true); break;
		case 1: // 1:OFFにする
			system.getLSD().setFlag(iSwitch, false); break;
		case 2: // 2:ON/OFFを逆転
			system.getLSD().setFlag(iSwitch, !system.getLSD().getFlag(iSwitch)); break;
	} }
}

PP_protoType(CODE_OPERATE_VAR)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	SaveData& lsd = system.getLSD();

	std::pair<int, int> range(0, 0);
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		range.first = range.second = com.at(1);
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		range.first = com.at(1);
		range.second = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		range.first = range.second = system.getLSD().getVar(com.at(1));
		break;
	}
	int value = 0;
	switch (com.at(4)) {
	case 0:		// [定数] Cの値を直接適用
		value = com.at(5);
		break;
	case 1:		// [変数] V[C]
		value = system.getLSD().getVar(com.at(5));
		break;
	case 2:		// [変数(参照)] V[V[C]]
		value = system.getLSD().getVar(system.getLSD().getVar(com.at(5)));
		break;
	case 3:		// [乱数] C〜Dの範囲の乱数
		value = rpg2k::random(com.at(5), com.at(6));
		break;
	case 4:		// [アイテム]
		switch( com.at(6) ) {
		case 0:	// 所持数
			value = lsd.getItemNum(com.at(5)); break;
		case 1:	// 装備数
			value = lsd.getEquipNum(com.at(5)); break;
		default: kuto_assert(false);
		}
		break;
	case 5:		// [主人公]
		{
			//const GameCharaStatus& status = gameField_->getGameSystem().getPlayerStatus(com.at(5));
			int charID = com.at(5);
			switch (com.at(6)) {
			case 0:		value = system.level(charID);	break;		// レベル
			case 1:		value = system.exp(charID);		break;		// 経験値
			case 2:		value = system.hp(charID);		break;		// HP
			case 3:		value = system.mp(charID);		break;		// MP
			case 4:		value = system.param(charID, rpg2k::Param::HP);		break;		// 最大HP
			case 5:		value = system.param(charID, rpg2k::Param::MP);		break;		// 最大MP
			case 6:		value = system.param(charID, rpg2k::Param::ATTACK);	break;		// 攻撃力
			case 7:		value = system.param(charID, rpg2k::Param::GAURD);	break;		// 防御力
			case 8:		value = system.param(charID, rpg2k::Param::MIND);	break;		// 精神力
			case 9:		value = system.param(charID, rpg2k::Param::SPEED);	break;		// 敏捷力
			case 10: case 11: case 12: case 13: case 14:
				value = lsd.equip(charID)[com.at(6)-10]; break;
			}
		}
		break;
	case 6:		// [キャラ]
		{
			EventState& event = lsd.eventState(com.at(5));
			switch (com.at(6)) {
			case 0:		// マップID
				value = event.mapID(); break;
			case 1:		// 	X座標
				value = event.x(); break;
			case 2:		// 	Y座標
				value = event.y(); break;
			case 3:		// 	向き
				value = event.eventDir(); break;
			case 4:		// 	画面X(画面上の水平座標)
				value = 0; // TODO
				break;
			case 5:		// 	画面Y(画面上の垂直座標)
				value = 0; // TODO
				break;
			}
		}
		break;
	case 7:		// [その他]
		switch( com.at(5) ) {
		case 0: value = system.getLSD().getMoney(); break; // 所持金
		case 1: value = (timer_.count + 59) / 60; break; // タイマー1の残り秒数
		case 2: value = system.getLSD().memberNum(); break; // パーティ人数
		case 3: value = system.getLSD()[101].getArray1D()[131]; break; // セーブ回数
		case 4: value = system.getLSD()[109].getArray1D()[32]; break; // 戦闘回数
		case 5: value = system.getLSD()[109].getArray1D()[34]; break; // 勝利回数
		case 6: value = system.getLSD()[109].getArray1D()[33]; break; // 敗北回数
		case 7: value = system.getLSD()[109].getArray1D()[35]; break; // 逃走回数
		case 8: value = 0; break; // TODO // MIDIの演奏位置(Tick)
		}
		break;
	case 8:		// [敵キャラ]
		// TODO
		break;
	}
	for (int iSwitch = range.first; iSwitch <= range.second; iSwitch++) {
	switch (com.at(3)) {
		case 0: system.getLSD().setVar(iSwitch, value); break; // 0:代入
		#define PP_operator(val, op) \
			case val: system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) op value); break
		PP_operator(1, +);
		PP_operator(2, -);
		PP_operator(3, *);
		PP_operator(4, /);
		PP_operator(5, %);
		#undef PP_operator
	} }
}

PP_protoType(CODE_OPERATE_ITEM)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int itemId = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	int num = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	system.getLSD().addItemNum(itemId, com.at(0) == 0? num : -num);
}

PP_protoType(CODE_OPERATE_MONEY)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int num = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	system.getLSD().addMoney(com.at(0) == 0? num : -num);
}

PP_protoType(CODE_OPERATE_TIMER)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	switch (com.at(0)) {
	case 0:		// 値の設定
		timer_.count = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
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

PP_protoType(CODE_GOTO_LABEL)
{
	currentCommandIndex_ = labels_[com.at(0) - 1];		// 戻り先で+1されるのでちょうどLabelの次になる
}

PP_protoType(CODE_PARTY_CHANGE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int playerId = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	if (com.at(0) == 0) {		// メンバーを加える
		gameField_->addPlayer(playerId);
	} else {							// メンバーを外す
		gameField_->removePlayer(playerId);
	}
}

PP_protoType(CODE_LOCATE_MOVE)
{
	int dir = com.getArgNum() >= 4? com.at(3) : 0;
	gameField_->changeMap(com.at(0), com.at(1), com.at(2), dir);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_LOCATE_MOVE)
{
	waitEventInfo_.enable = false;
}

PP_protoType(CODE_LOCATE_SAVE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.getLSD().setVar(com.at(0), system.getLSD().eventState(rpg2k::EV_ID_PARTY).mapID());
	system.getLSD().setVar(com.at(1), system.getLSD().eventState(rpg2k::EV_ID_PARTY).x());
	system.getLSD().setVar(com.at(2), system.getLSD().eventState(rpg2k::EV_ID_PARTY).y());
}

PP_protoType(CODE_LOCATE_LOAD)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	gameField_->changeMap(system.getLSD().getVar(com.at(0)), system.getLSD().getVar(com.at(1)), system.getLSD().getVar(com.at(2)), 0);
	waitEventInfo_.enable = true;
}

// print comments to stdout
PP_protoType(CODE_TXT_REM)
{
#if RPG2K_DEBUG
	std::cout << com.getString().toSystem() << std::endl;
#endif
}
PP_protoType(CODE_TXT_REM_ADD)
{
#if RPG2K_DEBUG
	std::cout << com.getString().toSystem() << std::endl;
#endif
}

PP_protoType(CODE_TXT_SHOW)
{
	openGameMassageWindow();
	gameMessageWindow_->addLine( com.getString().toSystem() );
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->instNum(); i++) {
		const rpg2k::structure::Instruction& comNext = (*currentEventPage_)[i];
		if (comNext.code() == CODE_TXT_SHOW_ADD) {
			gameMessageWindow_->addLine(comNext.getString().toSystem());
		} else {
			break;
		}
	}

	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_TXT_SHOW)
{
	if (gameMessageWindow_->closed()) {
		waitEventInfo_.enable = false;
		gameMessageWindow_->freeze(true);
	}
}

PP_protoType(CODE_TXT_OPTION)
{
	messageWindowSetting_.showFrame = !com.at(0);
	messageWindowSetting_.pos = (MessageWindowSetting::PosType)com.at(1);
	messageWindowSetting_.autoMove = com.at(2);
	messageWindowSetting_.enableOtherEvent = com.at(3);

	Array1D& lsdSys = gameField_->getGameSystem().getLSD()[101];
	lsdSys[41] = com[0];
	lsdSys[42] = com[1];
	lsdSys[43] = bool(com[3]);
	lsdSys[44] = bool(com[4]);
}
PP_protoType(CODE_TXT_FACE)
{
	gameMessageWindow_->setFaceTexture(com.getString().toSystem(), com.at(0), (bool)com.at(1), (bool)com.at(2));
	selectWindow_->setFaceTexture(com.getString().toSystem(), com.at(0), (bool)com.at(1), (bool)com.at(2));

	Array1D& lsdSys = gameField_->getGameSystem().getLSD()[101];
	lsdSys[51] = com.getString();
	lsdSys[52] = com[0];
	lsdSys[53] = com[1];
	lsdSys[54] = bool(com[2]);
}

PP_protoType(CODE_BTL_GO_START)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	std::string terrain;
	if (com.at(2) == 0) {
		int terrainId = gameField_->getMap()->getTerrainId(eventPageInfos_[currentEventIndex_].x, eventPageInfos_[currentEventIndex_].y);
		terrain = system.getLDB().terrain()[terrainId][4].get_string().toSystem();
	} else {
		terrain = com.getString().toSystem();
	}
	int enemyId = com.at(0) == 0? com.at(1) : system.getLSD().getVar(com.at(1));
	gameField_->startBattle(terrain, enemyId, (bool)com.at(5), com.at(3) != 0, com.at(4) == 0);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_BTL_GO_START)
{
	waitEventInfo_.enable = false;
	int result = gameField_->getBattleResult();
	if (com.at(3) == 1 && result == GameBattle::kResultEscape) {
		command<CODE_EVENT_BREAK>(com);		// Escape -> Break
	} else if (com.at(3) == 2 || com.at(4) == 1) {
		conditionStack_.push(ConditionInfo(com.nest(), result));
	}
}

PP_protoType(CODE_BTL_GO_WIN)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultWin;
}

PP_protoType(CODE_BTL_GO_ESCAPE)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultEscape;
}

PP_protoType(CODE_BTL_GO_LOSE)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultLose;
}

PP_protoType(CODE_IF_START)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	bool condValue = false; // int condValue = 0;
	switch (com.at(0)) {
	case 0:		// 0:スイッチ
		condValue = system.getLSD().getFlag(com.at(1)) == (com.at(2) == 0? true : false);
		break;
	case 1:		// 1:変数
		{
			int value = com.at(2) == 0? com.at(3) : system.getLSD().getVar(com.at(3));
			switch (com.at(4)) {
			case 0:		// と同値
				condValue = (system.getLSD().getVar(com.at(1)) == value);
				break;
			case 1:		// 	以上
				condValue = (system.getLSD().getVar(com.at(1)) >= value);
				break;
			case 2:		// 	以下
				condValue = (system.getLSD().getVar(com.at(1)) <= value);
				break;
			case 3:		// より大きい
				condValue = (system.getLSD().getVar(com.at(1)) > value);
				break;
			case 4:		// より小さい
				condValue = (system.getLSD().getVar(com.at(1)) < value);
				break;
			case 5:		// 以外
				condValue = (system.getLSD().getVar(com.at(1)) != value);
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
			condValue = (system.getLSD().getMoney() >= com.at(1));
			break;
		case 1:		// 以下
			condValue = (system.getLSD().getMoney() <= com.at(1));
			break;
		}
		break;
	case 4:		// 4:アイテム
		switch (com.at(2)) {
		case 0:		// 持っている
			condValue = (system.getLSD().getItemNum(com.at(1)) > 0);
			break;
		case 1:		// 持っていない
			condValue = (system.getLSD().getItemNum(com.at(1)) == 0);
			break;
		}
		break;
	case 5:		// 5:主人公
		switch (com.at(2)) {
		case 0:		// パーティにいる
			condValue = gameField_->getPlayerFromId(com.at(1)) != NULL;
			break;
		case 1:		// 主人公の名前が文字列引数と等しい
			condValue = com.getString() == system.name(com.at(1));
			break;
		case 2:		// レベルがCの値以上
			condValue = system.level(com.at(1)) >= com.at(3);
			break;
		case 3:		// HPがCの値以上
			condValue = system.hp(com.at(1)) >= com.at(3);
			break;
		case 4:		// 特殊技能IDがCの値の特殊技能を使用できる
			kuto_assert(false);
			// condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).isLearnedSkill(com.at(3));
			break;
		case 5:		// アイテムIDがCの値のアイテムを装備している
			kuto_assert(false);
			/*
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::WEAPON] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::SHIELD] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::ARMOR] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::HELMET] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::OTHER] == com.at(3);
			 */
			break;
		case 6:		// 状態IDがCの状態になっている
			kuto_assert(false);
			// condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getBadConditionIndex(com.at(3)) >= 0;
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
		condValue = (false);	// TODO
		break;
	case 8:		// 8:決定キーでこのイベントを開始した
		condValue = startDecideButton_;
		break;
	case 9:		// 9:演奏中のBGMが一周した
		condValue = bgm_->isLooped();	// TODO
		break;
	}
	conditionStack_.push(ConditionInfo(com.nest(), condValue));
	executeChildCommands_ = conditionStack_.top().value == true;
}

PP_protoType(CODE_IF_ELSE)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

PP_protoType(CODE_BLOCK_END)
{
	conditionStack_.pop();
	executeChildCommands_ = true;
}

PP_protoType(CODE_SELECT_START)
{
	openGameSelectWindow();
	std::string::size_type oldPos = 0;
	rpg2k::SystemString const target = com.getString().toSystem();
	for (int i = 0; i < 4; i++) {
		std::string::size_type pos = target.find('/', oldPos);
		if (pos == std::string::npos) {
			selectWindow_->addLine(target.substr(oldPos));
			break;
		} else {
			selectWindow_->addLine(target.substr(oldPos, pos - oldPos));
			oldPos = pos + 1;
		}
	}
	selectWindow_->setEnableCancel(com.at(0) != 0);

	waitEventInfo_.enable = true;
}
PP_protoTypeWait(CODE_SELECT_START)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled()) {
			selectIndex = com.at(0) - 1;
		}
		int value = selectIndex < (int)selectWindow_->getMessageSize()? kuto::crc32(selectWindow_->getMessage(selectIndex).str) : 0;
		conditionStack_.push(ConditionInfo(com.nest(), value));
	}
}

PP_protoType(CODE_SELECT_CASE)
{
	executeChildCommands_ = (uint)conditionStack_.top().value == kuto::crc32(com.getString().toSystem());
}

PP_protoType(CODE_GAMEOVER)
{
	gameField_->getGame()->gameOver();
	waitEventInfo_.enable = true;
}

PP_protoType(CODE_TITLE)
{
	gameField_->getGame()->returnTitle();
	waitEventInfo_.enable = true;
}

PP_protoType(CODE_EVENT_BREAK)
{
	currentCommandIndex_ = -1;
}

PP_protoType(CODE_EVENT_CLEAR)
{
	eventPageInfos_[currentEventIndex_].cleared = true;
	if (eventPageInfos_[currentEventIndex_].npc) {
		eventPageInfos_[currentEventIndex_].npc->freeze(true);
	}
}

PP_protoType(CODE_LOOP_START)
{
	LoopInfo info;
	info.startIndex = currentCommandIndex_;
	info.conditionSize = conditionStack_.size();
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->instNum(); i++) {
		const rpg2k::structure::Instruction& comNext = (*currentEventPage_)[i];
		if (com.nest() != comNext.nest()) {
			continue;
		}
		if (comNext.code() == CODE_LOOP_END) {
			info.endIndex = i;
			break;
		}
	}
	loopStack_.push(info);
}

PP_protoType(CODE_LOOP_BREAK)
{
	while (loopStack_.top().conditionSize < (int)conditionStack_.size()) {
		conditionStack_.pop();
	}
	currentCommandIndex_ = loopStack_.top().endIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
	loopStack_.pop();
}

PP_protoType(CODE_LOOP_END)
{
	currentCommandIndex_ = loopStack_.top().startIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
}

PP_protoType(CODE_WAIT)
{
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_WAIT)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(0) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_PICT_SHOW)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
	setPictureInfo( system.getLSD(), com );

	pictures_[picIndex] = addChild( GameEventPicture::createTask(std::string( system.gameDir() ).append("/Picture/").append( com.getString().toSystem() ), system.getLSD().picture()[com[0]]));
	pictures_[picIndex]->setPriority(1.f + (float)picIndex * -0.0001f);
}

PP_protoType(CODE_PICT_MOVE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]) {
		return;
	}
	setPictureInfo( system.getLSD(), com );

	pictures_[picIndex]->move(com, com.at(14) * 60 / 10);
	waitEventInfo_.enable = (bool)com.at(15);
}

PP_protoTypeWait(CODE_PICT_MOVE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]->isMoving()) {
		waitEventInfo_.enable = false;
	}
	system.getLSD().picture()[com[0]].clear();
}

PP_protoType(CODE_PICT_CLEAR)
{
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
}

PP_protoType(CODE_SYSTEM_SCREEN)
{
	gameField_->setFadeInfo(com.at(0), com.at(1));
}

PP_protoType(CODE_SCREEN_CLEAR)
{
	gameField_->fadeOut(com.at(0));
}

PP_protoType(CODE_SCREEN_SHOW)
{
	gameField_->fadeIn(com.at(0));
}

PP_protoType(CODE_SCREEN_SCROLL)
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

PP_protoTypeWait(CODE_SCREEN_SCROLL)
{
	if (!gameField_->getMap()->isScrolling())
		waitEventInfo_.enable = false;
}

PP_protoType(CODE_CHARA_TRANS)
{
	gameField_->getPlayerLeader()->setVisible((bool)com.at(0));
}

PP_protoType(CODE_CHARA_MOVE)
{
	// TODO
/*
	GameChara* chara = getCharaFromEventId(com.at(0));
	if (!chara)
		return;
	// int frequency = com.at(1);
	CRpgRoute route;
	route.repeat = com.at(2) == 1;
	route.ignore = com.at(3) == 1;
	for (uint i = 4; i < com.getArgNum(); i++) {
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
 */
}

PP_protoType(CODE_MOVEALL_START)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->startRoute();
}

PP_protoType(CODE_MOVEALL_CANCEL)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->endRoute();
}

PP_protoType(CODE_NAME_INPUT)
{
	nameInputMenu_->freeze(false);
	nameInputMenu_->setPlayerInfo(com.at(0), (bool)com.at(1), (bool)com.at(2));
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_NAME_INPUT)
{
	if (nameInputMenu_->closed()) {
		waitEventInfo_.enable = false;
		nameInputMenu_->freeze(true);
	}
}

PP_protoType(CODE_EVENT_SWAP)
{
	SaveData& lsd = gameField_->getGameSystem().getLSD();

	EventState& stateA = lsd.eventState(com[0]);
	EventState& stateB = lsd.eventState(com[1]);

	uint xBuff = stateA.x(), yBuff = stateA.y();

	stateA[12] = stateB.x();
	stateA[13] = stateB.y();

	stateB[12] = xBuff;
	stateB[13] = yBuff;
}

PP_protoType(CODE_EVENT_LOCATE)
{
	SaveData& lsd = gameField_->getGameSystem().getLSD();
	EventState& state = lsd.eventState(com[0]);

	uint x, y;
	switch(com[1]) {
		case 0:
			x = com[2];
			y = com[3];
			break;
		case 1:
			x = lsd.getVar(com[2]);
			y = lsd.getVar(com[3]);
			break;
		default: kuto_assert(false);
	}

	state[12] = x;
	state[13] = y;
}

PP_protoType(CODE_PARTY_NAME)
{
	gameField_->getGameSystem().setName( com.at(0), com.getString() );
}

PP_protoType(CODE_PARTY_TITLE)
{
	gameField_->getGameSystem().setTitle( com.at(0), com.getString() );
}

PP_protoType(CODE_PARTY_WALK)
{
	Array1D& charData = gameField_->getGameSystem().getLSD().character()[ com[0] ];

	charData[11] = com.getString();
	charData[12] = com[1];
	charData[13] = bool(com[2]);
}

PP_protoType(CODE_PARTY_FACE)
{
	Array1D& charData = gameField_->getGameSystem().getLSD().character()[ com[0] ];

	charData[21] = com.getString();
	charData[22] = com[1];
}

PP_protoType(CODE_MM_BGM_PLAY)
{
	// TODO
}

PP_protoType(CODE_OPERATE_KEY)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	waitEventInfo_.enable = com.at(1);
	if (!waitEventInfo_.enable)
		system.getLSD().setVar(com.at(0), getInputKeyValue(com));
}

PP_protoTypeWait(CODE_OPERATE_KEY)
{
	int key = getInputKeyValue(com);
	if (key != 0) {
		waitEventInfo_.enable = false;
		rpg2k::model::Project& system = gameField_->getGameSystem();
		system.getLSD().setVar(com.at(0), key);
	}
}

PP_protoType(CODE_PANORAMA)
{
	Array1D& map = gameField_->getGameSystem().getLSD()[111];
	map[31] = true;
	map[32] = com.getString();
	map[33] = bool(com.at(0));
	map[34] = bool(com.at(1));
	map[35] = bool(com.at(2));
	map[36] = int(com.at(3));
	map[37] = bool(com.at(4));
	map[38] = int(com.at(5));
/*
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
 */
}

PP_protoType(CODE_INN)
{
	openGameSelectWindow();
	const rpg2k::model::Project& system = gameField_->getGameSystem();
	const DataBase& ldb = system.getLDB();
	int base = 80 + com.at(0) * 5;
	std::string mes = ldb.vocabulary(base + 0);
	std::ostringstream oss;
	oss << com.at(1);
	mes += oss.str();
	mes += ldb.vocabulary(95).toSystem();
	mes += ldb.vocabulary(base + 1);
	selectWindow_->addLine(mes);
	selectWindow_->addLine(ldb.vocabulary(base + 2));
	selectWindow_->addLine(ldb.vocabulary(base + 3), system.getLSD().getMoney() >= com.at(1));
	selectWindow_->addLine(ldb.vocabulary(base + 4));
	selectWindow_->setCursorStart(2);
	selectWindow_->setEnableCancel(true);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_INN)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled())
			selectIndex = 3;
		if (selectIndex == 2) {
			gameField_->getGameSystem().getLSD().addMoney(-com.at(1));
			for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
				gameField_->getPlayers()[i]->getStatus().fullCure();
			}
		}
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.nest(), selectIndex == 2));
	}
}

PP_protoType(CODE_INN_IF_START)
{
	executeChildCommands_ = conditionStack_.top().value == true;
}

PP_protoType(CODE_INN_IF_ELSE)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

PP_protoType(CODE_SHOP)
{
	shopMenu_->freeze(false);
	int shopType = com.at(0);
	int mesType = com.at(1);
	std::vector<int> items;
	for (uint i = 4; i < com.getArgNum(); i++)
		items.push_back(com.at(i));
	shopMenu_->setShopData(shopType, mesType, items);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_SHOP)
{
	if (shopMenu_->closed()) {
		shopMenu_->freeze(true);
		waitEventInfo_.enable = false;
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.nest(), shopMenu_->buyOrSell()));
	}
}

PP_protoType(CODE_MM_SOUND)
{
	// TODO
}

PP_protoType(CODE_SCREEN_COLOR)
{
	// TODO
	waitEventInfo_.enable = (com.at(5) == 1);
}

PP_protoTypeWait(CODE_SCREEN_COLOR)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(4) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_BTLANIME)
{
	GameSkillAnime* anime = addChild( GameSkillAnime::createTask(gameField_->getGameSystem(), com.at(0)));
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

PP_protoTypeWait(CODE_BTLANIME)
{
	if (skillAnime_->isFinished()) {
		skillAnime_->release();
		skillAnime_ = NULL;
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_PARTY_SOUBI)
{
	rpg2k::model::Project& proj = gameField_->getGameSystem();
	SaveData& lsd = proj.getLSD();

	std::vector<uint16_t> target;
	switch(com[0]) {
		case 0: target = lsd.member(); break;
		case 1: target.push_back(com[1]); break;
		case 2: target.push_back( lsd.getVar(com[1]) ); break;
	}

	switch(com[2]) {
	// equip
		case 0: {
			uint itemID;
			switch(com[4]) {
				case 0: itemID = (com[3] == 0) ? 1 : com[3]; break;
				case 1: itemID = lsd.getVar(com[3]); break;
			}

			for(std::vector<uint16_t>::const_iterator it = target.begin(); it != target.end(); it++) {
				if( !proj.equip(*it, itemID) ) break;
			}
		} break;
	// unequip
		case 1: {
			uint start, end;
			switch(com[3]) {
				case rpg2k::Equip::WEAPON: case rpg2k::Equip::SHIELD: case rpg2k::Equip::ARMOR: case rpg2k::Equip::HELMET: case rpg2k::Equip::OTHER:
					start = end = com[3]; break;
				case rpg2k::Equip::END: // all
					start = 0; end = rpg2k::Equip::END-1; break;
			}
			for(std::vector<uint16_t>::const_iterator it = target.begin(); it != target.end(); ++it) {
				proj.unequip( *it, static_cast< rpg2k::Equip::Type >(end) );
			}
		} break;
	}
}

PP_protoType(CODE_PARTY_REFRESH)
{
	rpg2k::model::Project& proj = gameField_->getGameSystem();
	SaveData& lsd = proj.getLSD();
	Array2D& charDatas = lsd.character();
	std::vector< uint > charIDs;

	switch(com[0]) {
		case 0: {
			std::vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(com[1]); break;
		case 2: charIDs.push_back( lsd.getVar(com[1]) ); break;
	}

	for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		charDatas[*it][71] = proj.param(*it, rpg2k::Param::HP);
		charDatas[*it][72] = proj.param(*it, rpg2k::Param::MP);
	}
}

PP_protoType(CODE_PARTY_EXP)
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
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int exp = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
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

PP_protoType(CODE_PARTY_LV)
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
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int level = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
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

PP_protoType(CODE_PARTY_POWER)
{
	SaveData& lsd = gameField_->getGameSystem().getLSD();
	Array2D& charDatas = lsd.character();

	std::vector< uint16_t > target;
	switch(com[0]) {
		case 0: target = lsd.member(); break;
		case 1: target.push_back(com[1]); break;
		case 2: target.push_back( lsd.getVar(com[1]) ); break;
	}

	int index, max, min;
	switch(com[3]) {
		case rpg2k::Param::HP:
			max = rpg2k::CHAR_HP_MAX; min = rpg2k::PARAM_MIN;
			index = 33 + com[3];
			break;
		case rpg2k::Param::MP:
			max =   rpg2k::PARAM_MAX; min =    rpg2k::MP_MIN;
			index = 33 + com[3];
			break;
		case rpg2k::Param::ATTACK: case rpg2k::Param::GAURD: case rpg2k::Param::MIND: case rpg2k::Param::SPEED:
			max =   rpg2k::PARAM_MAX; min = rpg2k::PARAM_MIN;
			index = 41 + com[3] - rpg2k::Param::ATTACK;
			break;
	}

	int val = com[5];
	if(val == 0) val = 1;
	else switch(com[4]) {
		case 0: break;
		case 1: val = lsd.getVar(val); break;
	}
	switch(com[2]) {
		case 0: val =  val; break;
		case 1: val = -val; break;
	}

	for( std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); it++ ) {
		charDatas[*it][index] = charDatas[*it][index].get<int>() + val;
	}
}

PP_protoType(CODE_PARTY_SKILL)
{
	// TODO: to rpg2k::model::Project code
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int skillId = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	for (uint i = 0; i < statusList.size(); i++) {
		if (com.at(2) == 0)
			statusList[i]->learnSkill(skillId);
		else
			statusList[i]->forgetSkill(skillId);
	}
}

PP_protoType(CODE_EVENT_GOSUB)
{
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getGameSystem().getLMU();
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
		currentEventIndex_ = eventId + rpgLmu.event().rbegin().first();
		executeCommands(system.getLDB().commonEvent()[eventId][22], 0);
	} else {
		// map event
		int eventId = (com.at(0) == 1)? com.at(1) : system.getLSD().getVar(com.at(1));
		int eventPage = (com.at(0) == 1)? com.at(2) : system.getLSD().getVar(com.at(2));
		if (eventId == 10005)
			eventId = currentEventIndex_;
		currentEventIndex_ = eventId;
		executeCommands(rpgLmu.event()[eventId][5].getArray2D()[eventPage][52], 0);
	}
	if (waitEventInfo_.enable) {
		backupWaitInfoEnable_ = true;
	} else {
		// restore
		restoreCallStack();
	}
}
