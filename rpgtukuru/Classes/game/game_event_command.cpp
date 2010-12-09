#include <deque>
#include <iterator>
#include <sstream>
#include <utility>

#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_virtual_pad.h>

#include <rpg2k/Debug.hpp>
#include <rpg2k/Event.hpp>

#include "game.h"
#include "game_battle.h"
#include "game_bgm.h"
#include "game_event_manager.h"
#include "game_event_command.h"
#include "game_field.h"
#include "game_map.h"
#include "game_message_window.h"
#include "game_name_input_menu.h"
#include "game_picture_manager.h"
#include "game_save_menu.h"
#include "game_select_window.h"
#include "game_shop_menu.h"
#include "game_skill_anime.h"
#include "game_system_menu.h"

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;
using rpg2k::structure::Element;
using rpg2k::structure::Event;
using rpg2k::structure::EventState;
using rpg2k::structure::Music;
using rpg2k::structure::Sound;
using rpg2k::model::DataBase;
using rpg2k::model::MapUnit;
using rpg2k::model::Project;
using rpg2k::model::SaveData;


namespace
{
	void setPictureInfo(SaveData& lsd, rpg2k::structure::Instruction const& com)
	{
		Array1D& dst = lsd.picture()[com[0]];
		// coord
		int x = com[1] == 0? com[2] : lsd.var(com[2]);
		int y = com[1] == 0? com[3] : lsd.var(com[3]);
		if( com.code() == CODE_PICT_SHOW ) {
			dst[1] = com.string();
			dst[6] = bool(com[4]); // scroll
			dst[9] = bool(com[7]); // useAlpha
			// dst[] = com[14]; // alpha(lower)

			dst[2] = dst[5] = x;
			dst[3] = dst[6] = y;

			dst[51] = 0;
		} else if( com.code() == CODE_PICT_MOVE ) {
			dst[51] = com[14]; // set counter
			// dst[] = com[15]; // wait setting

			dst[31] = x;
			dst[32] = y;

			dst[11] = double( dst[41].to<int>() ); // red
			dst[12] = double( dst[42].to<int>() ); // green
			dst[13] = double( dst[43].to<int>() ); // blue
			dst[14] = double( dst[44].to<int>() ); // chroma
		} else kuto_assert(false);
		dst[33] = com.at( 5); // scale
		dst[34] = com.at( 6); // alpha(upper)

		dst[41] = com.at( 8); // red
		dst[42] = com.at( 9); // green
		dst[43] = com[10]; // blue
		dst[44] = com[11]; // chroma

		dst[15] = com[12]; // effect type
		dst[16] = double( com[13] ); // speed or power
	}

	struct EventKey { enum Type {
		NONE = 0,
		DOWN = 1, LEFT = 2, RIGHT = 3, UP = 4,
		ENTER = 5, CANCEL = 6, SHIFT = 7,
		#if defined(RPG2003)
			NUM_0 = 10, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
			PLUS = 20, MINUS = 21, MULTIPLY = 22, DIVIDE = 23, DOT = 24,
		#endif
	}; };
	EventKey::Type inputKeyValue(const rpg2k::structure::Instruction& com)
	{
		kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
		EventKey::Type key = EventKey::NONE;
		if (com[3] && virtualPad.press(kuto::VirtualPad::KEY_A))
			key = EventKey::ENTER;
		if (com[4] && virtualPad.press(kuto::VirtualPad::KEY_B))
			key = EventKey::CANCEL;
		if ( com[2] ) {
			#if( defined(RPG2000) || defined(RPG2000_VALUE) )
				if (virtualPad.press(kuto::VirtualPad::KEY_DOWN))
					key = EventKey::DOWN;
				if (virtualPad.press(kuto::VirtualPad::KEY_LEFT))
					key = EventKey::LEFT;
				if (virtualPad.press(kuto::VirtualPad::KEY_RIGHT))
					key = EventKey::RIGHT;
				if (virtualPad.press(kuto::VirtualPad::KEY_UP))
					key = EventKey::UP;
			#elif defined(RPG2003)
				/*
				if (com.at( 5) && ) // number
				if (com.at( 6) && ) // symbol
				if ( com.at( 7) ) { // get time until key is pressed
					com[8] // <- variable ID the time(1/10 second) will be written
				}
				 */
				if (com.at( 9) && virtualPad.press(kuto::VirtualPad::KEY_X))
					key = EventKey::SHIFT;
				if (com[10] && virtualPad.press(kuto::VirtualPad::KEY_DOWN))
					key = EventKey::DOWN;
				if (com[11] && virtualPad.press(kuto::VirtualPad::KEY_LEFT))
					key = EventKey::LEFT;
				if (com[12] && virtualPad.press(kuto::VirtualPad::KEY_RIGHT))
					key = EventKey::RIGHT;
				if (com[13] && virtualPad.press(kuto::VirtualPad::KEY_UP))
					key = EventKey::UP;
			#else
				#error
			#endif
		}
		#if ( defined(RPG2000_VALUE) || defined(RPG2003) )
		else {
			if (com[5] && virtualPad.press(kuto::VirtualPad::KEY_X))
				key = EventKey::SHIFT;
			if (com[6] && virtualPad.press(kuto::VirtualPad::KEY_DOWN))
				key = EventKey::DOWN;
			if (com[7] && virtualPad.press(kuto::VirtualPad::KEY_LEFT))
				key = EventKey::LEFT;
			if (com[8] && virtualPad.press(kuto::VirtualPad::KEY_RIGHT))
				key = EventKey::RIGHT;
			if (com[9] && virtualPad.press(kuto::VirtualPad::KEY_UP))
				key = EventKey::UP;
		}
		#endif
		return key;
	}
}

GameEventManager::TargetCharacter
GameEventManager::targetCharacter(GameEventManager::Target::Type const t
, unsigned const val) const
{
	TargetCharacter ret;
	switch (t) {
	case Target::PARTY: {	// 0:パーティーメンバー全員
		std::vector<uint16_t> const& mem = cache_.lsd->member();
		std::copy( mem.begin(), mem.end(), std::back_inserter(ret) );
	} break;
	case Target::IMMEDIATE:		// 1:[固定] 主人公IDがAの主人公
		ret.push_back(val);
		break;
	case Target::VARIABLE:		// 2:[変数] 主人公IDがV[A]の主人公
		ret.push_back(cache_.lsd->var(val));
		break;
	default: kuto_assert(false);
	}
	return ret;
}

PP_protoType(CODE_OPERATE_SWITCH)
{
	SaveData& lsd = *cache_.lsd;
	std::pair<int, int> range(0, 0);
	switch (com[0]) {
	case 0:		// 0:[単独] S[A]
		range.first = range.second = com[1];
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		range.first = com[1];
		range.second = com[2];
		break;
	case 2:		// 2:[変数] S[V[A]]
		range.first = range.second = lsd.var(com[1]);
		break;
	}
	for (int iSwitch = range.first; iSwitch <= range.second; iSwitch++) {
	switch( com[3] ) {
		case 0: // 0:ONにする
			lsd.setFlag(iSwitch, true); break;
		case 1: // 1:OFFにする
			lsd.setFlag(iSwitch, false); break;
		case 2: // 2:ON/OFFを逆転
			lsd.setFlag(iSwitch, !lsd.flag(iSwitch)); break;
	} }
}

PP_protoType(CODE_OPERATE_VAR)
{
	Project& proj = *cache_.project;
	SaveData& lsd = *cache_.lsd;

	std::pair<int, int> range(0, 0);
	switch (com[0]) {
	case 0:		// 0:[単独] S[A]
		range.first = range.second = com[1];
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		range.first = com[1];
		range.second = com[2];
		break;
	case 2:		// 2:[変数] S[V[A]]
		range.first = range.second = lsd.var(com[1]);
		break;
	}
	int value = 0;
	switch (com[4]) {
	case 0:		// [定数] Cの値を直接適用
		value = com[5];
		break;
	case 1:		// [変数] V[C]
		value = lsd.var(com[5]);
		break;
	case 2:		// [変数(参照)] V[V[C]]
		value = lsd.var(lsd.var(com[5]));
		break;
	case 3:		// [乱数] C〜Dの範囲の乱数
		value = rpg2k::random(com[5], com[6]);
		break;
	case 4:		// [アイテム]
		switch( com[6] ) {
		case 0:	// 所持数
			value = lsd.itemNum(com[5]); break;
		case 1:	// 装備数
			value = proj.equipNum(com[5]); break;
		default: kuto_assert(false);
		}
		break;
	case 5:		// [主人公]
		{
			unsigned const charID = com[5];
			Project::Character const& c = proj.character(charID);
			switch (com[6]) {
			case 0:		value = c.level();	break;		// レベル
			case 1:		value = c.exp();		break;		// 経験値
			case 2:		value = c.hp();		break;		// HP
			case 3:		value = c.mp();		break;		// MP
			case 4:		value = c.param(rpg2k::Param::HP);		break;		// 最大HP
			case 5:		value = c.param(rpg2k::Param::MP);		break;		// 最大MP
			case 6:		value = proj.paramWithEquip(charID, rpg2k::Param::ATTACK);	break;		// 攻撃力
			case 7:		value = proj.paramWithEquip(charID, rpg2k::Param::GAURD);	break;		// 防御力
			case 8:		value = proj.paramWithEquip(charID, rpg2k::Param::MIND);	break;		// 精神力
			case 9:		value = proj.paramWithEquip(charID, rpg2k::Param::SPEED);	break;		// 敏捷力
			case 10: case 11: case 12: case 13: case 14:
				value = c.equip()[com[6]-10]; break;
			}
		}
		break;
	case 6:		// [キャラ]
		{
			EventState const& event = lsd.eventState(com[5]);
			switch (com[6]) {
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
		switch( com[5] ) {
		case 0: value = lsd.money(); break; // 所持金
		case 1: value = (timer_[0]->left() + 59) / 60; break; // タイマー1の残り秒数
		case 2: value = lsd.memberNum(); break; // パーティ人数
		case 3: value = lsd.system()[131]; break; // セーブ回数
		case 4: value = lsd[109].toArray1D()[32]; break; // 戦闘回数
		case 5: value = lsd[109].toArray1D()[34]; break; // 勝利回数
		case 6: value = lsd[109].toArray1D()[33]; break; // 敗北回数
		case 7: value = lsd[109].toArray1D()[35]; break; // 逃走回数
		case 8: kuto_assert(false); break; // TODO // MIDIの演奏位置(Tick)
		#if RPG2003
		case 9: value = (timer_[1]->left() + 59) / 60; break; // タイマー2の残り秒数
		#endif
		}
		break;
	case 8:		// [敵キャラ]
		// TODO
		break;
	}
	for (int iSwitch = range.first; iSwitch <= range.second; iSwitch++) {
	switch (com[3]) {
		case 0: lsd.setVar(iSwitch, value); break; // 0:代入
		#define PP_operator(val, op) \
			case val: lsd.setVar(iSwitch, lsd.var(iSwitch) op value); break
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
	SaveData& lsd = *cache_.lsd;
	int itemId = com[1] == 0? com[2] : lsd.var(com[2]);
	int num = com[3] == 0? com[4] : lsd.var(com[4]);
	lsd.addItemNum(itemId, com[0] == 0? num : -num);
}

PP_protoType(CODE_OPERATE_MONEY)
{
	SaveData& lsd = *cache_.lsd;
	int num = com[1] == 0? com[2] : lsd.var(com[2]);
	lsd.addMoney(com[0] == 0? num : -num);
}

PP_protoType(CODE_OPERATE_TIMER)
{
	SaveData& lsd = *cache_.lsd;
	#if RPG2003
		int timerID = com[5];
	#else
		int timerID = 0;
	#endif
	switch (com[0]) {
	case 0:		// 値の設定
		timer_[timerID]->setCount( ( com[1] == 0? com[2] : lsd.var(com[2]) ) * 60);
		timer_[timerID]->pauseUpdate();
		timer_[timerID]->pauseDraw();
		break;
	case 1:		// 作動開始
		timer_[timerID]->pauseUpdate(false);
		timer_[timerID]->pauseDraw(com[3] == 1);
		break;
	case 2:		// 作動停止
		timer_[timerID]->pauseUpdate();
		break;
	}
}

PP_protoType(CODE_GOTO_LABEL)
{
	// rpg2k::structure::Event knows where the label is
}

PP_protoType(CODE_PARTY_CHANGE)
{
	SaveData& lsd = *cache_.lsd;
	int playerId = com[1] == 0? com[2] : lsd.var(com[2]);
	switch(com[0]) {
		case 0: // メンバーを加える
			lsd.addMember(playerId);
			break;
		case 1: // メンバーを外す
			lsd.removeMember(playerId);
			break;
	}
}

PP_protoType(CODE_LOCATE_MOVE)
{
	field_.changeMap(com[0], com[1], com[2]);
	if( com[4] != 0 ) {
		cache_.lsd->party()[22] =
			int( rpg2k::toEventDir( rpg2k::CharSet::Dir::Type( com[4] - 1 ) ) );
	}
	setWait(true);
}

PP_protoTypeWait(CODE_LOCATE_MOVE)
{
	setWait(false);
}

PP_protoType(CODE_LOCATE_SAVE)
{
	EventState& evSt = cache_.lsd->party();
	cache_.lsd->setVar(com[0], evSt.mapID());
	cache_.lsd->setVar(com[1], evSt.x());
	cache_.lsd->setVar(com[2], evSt.y());
}

PP_protoType(CODE_LOCATE_LOAD)
{
	SaveData& lsd = *cache_.lsd;
	field_.changeMap( lsd.var(com[0]), lsd.var(com[1]), lsd.var(com[2]) );
	setWait(true);
}

// print comments to stdout
PP_protoType(CODE_TXT_REM)
{
#if RPG2K_DEBUG
	std::cout << "COMMENT: \"" << com.string().toSystem() << "\"" << std::endl;
#endif
}
PP_protoType(CODE_TXT_REM_ADD)
{
#if RPG2K_DEBUG
	std::cout << "COMMENT: \"" << com.string().toSystem() << "\"" << std::endl;
#endif
}

PP_protoType(CODE_TXT_SHOW)
{
	openGameMassageWindow();
	messageWindow_.addLine( com.string().toSystem() );
	for (uint i = current_->pointer() + 1; i < current_->event().size(); i++) {
		const rpg2k::structure::Instruction& comNext = current_->event()[i];
		if (comNext.code() == CODE_TXT_SHOW_ADD) {
			messageWindow_.addLine(comNext.string().toSystem());
		} else {
			break;
		}
	}

	setWait(true);
}

PP_protoTypeWait(CODE_TXT_SHOW)
{
	if (messageWindow_.closed()) {
		setWait(false);
		messageWindow_.freeze();
	}
}

PP_protoType(CODE_TXT_OPTION)
{
	Array1D& lsdSys = cache_.lsd->system();
	lsdSys[41] = com[0];
	lsdSys[42] = com[1];
	lsdSys[43] = bool(com[3]);
	lsdSys[44] = bool(com[4]);
}
PP_protoType(CODE_TXT_FACE)
{
	messageWindow_.setFaceTexture(com.string().toSystem(), com[0], (bool)com[1], (bool)com[2]);
	selectWindow_.setFaceTexture(com.string().toSystem(), com[0], (bool)com[1], (bool)com[2]);

	Array1D& lsdSys = cache_.lsd->system();
	lsdSys[51] = com.string();
	lsdSys[52] = com[0];
	lsdSys[53] = com[1];
	lsdSys[54] = bool(com[2]);
}

PP_protoType(CODE_BTL_GO_START)
{
	SaveData& lsd = *cache_.lsd;
	EventState& party = lsd.party();
	std::string terrain;
	if (com[2] == 0) {
		int terrainId = field_.map().terrainID(party.x(), party.y());
		terrain = cache_.ldb->terrain()[terrainId][4].to_string().toSystem();
	} else {
		terrain = com.string().toSystem();
	}
	int enemyId = com[0] == 0? com[1] : lsd.var(com[1]);
	field_.startBattle(terrain, enemyId, (bool)com[5], com[3] != 0, com[4] == 0);
	setWait(true);
}

PP_protoTypeWait(CODE_BTL_GO_START)
{
	setWait(false);
	switch( field_.battleResult() ) {
	case GameBattle::kResultWin:
		current_->skipToElse( com.nest(), CODE_BTL_GO_WIN );
		break;
	case GameBattle::kResultEscape:
		switch( com[3] ) {
		case 1: current_->ret(); break;
		case 2: current_->skipToElse( com.nest(), CODE_BTL_GO_ESCAPE ); break;
		}
		break;
	case GameBattle::kResultLose:
		switch( com[4] ) {
		case 0: field_.game().gameOver(); break;
		case 1: current_->skipToElse( com.nest(), CODE_BTL_GO_LOSE ); break;
		}
		break;
	}
}

PP_protoType(CODE_IF_START)
{
	Project& system = *cache_.project;
	SaveData& lsd = *cache_.lsd;

	bool result = false; // int result = 0;
	switch (com[0]) {
	case 0:		// 0:スイッチ
		result = lsd.flag(com[1]) == (com[2] == 0? true : false);
		break;
	case 1: {		// 1:変数
		int value = com[2] == 0? com[3] : lsd.var(com[3]);
		switch (com[4]) {
			#define PP_operator(CASE_VAL, OP) \
				case CASE_VAL: result = (lsd.var(com[1]) OP value); break
			PP_operator(0, ==); // と同値
			PP_operator(1, >=); // 	以上
			PP_operator(2, <=); // 	以下
			PP_operator(3, > ); // より大きい
			PP_operator(4, < ); // より小さい
			PP_operator(5, !=); // 以外
			#undef  PP_operator
		}
	} break;
	case 2:		// 2:タイマー1
		switch (com[2]) {
		case 0:		// 以上
			result = (timer_[0]->left() >= unsigned(com[1]) * 60);
			break;
		case 1:		// 以下
			result = (timer_[0]->left() <= unsigned(com[1]) * 60);
			break;
		}
		break;
	case 3:		// 3:所持金
		switch (com[2]) {
		case 0:		// 以上
			result = (lsd.money() >= com[1]);
			break;
		case 1:		// 以下
			result = (lsd.money() <= com[1]);
			break;
		}
		break;
	case 4:		// 4:アイテム
		result = system.hasItem(com[1]);
		switch (com[2]) {
		case 0: result =  result; break; // 持っている
		case 1: result = !result; break; // 持っていない
		}
		break;
	case 5: {		// 5:主人公
		Project::Character const& c = cache_.project->character(com[1]);
		switch (com[2]) {
		case 0: {		// パーティにいる
			std::vector<uint16_t> const& mem = lsd.member();
			result = std::find( mem.begin(), mem.end(), com[1] ) != mem.end();
		} break;
		case 1:		// 主人公の名前が文字列引数と等しい
			result = ( com.string() == c.name() );
			break;
		case 2:		// レベルがCの値以上
			result = c.level() >= com[3];
			break;
		case 3:		// HPがCの値以上
			result = c.hp() >= com[3];
			break;
		case 4: {		// 特殊技能IDがCの値の特殊技能を使用できる
			std::set<uint16_t> const& skill = c.skill();
			result = skill.find(com[3]) != skill.end();
		} break;
		case 5: {		// アイテムIDがCの値のアイテムを装備している
			Project::Character::Equip const& equip = c.equip();
			result = std::find( equip.begin(), equip.end(), com[3] ) != equip.end();
		} break;
		case 6: {		// 状態IDがCの状態になっている
			std::vector<uint8_t> const& condition = c.condition();
			result = std::find( condition.begin(), condition.end(), com[3] ) != condition.end();
		} break;
		}
	} break;
	case 6:		// 6:キャラの向き
		result = ( lsd.eventState(com[1]).eventDir() == com[2] );
		break;
	case 7:		// 7:乗り物
		result = ( lsd.party()[103].to<int>() == (com[2] + 1) );
		break;
	case 8:		// 8:決定キーでこのイベントを開始した
		result = ( current_->startType() == rpg2k::EventStart::KEY_ENTER );
		break;
	case 9:		// 9:演奏中のBGMが一周した
		kuto_assert(false); // TODO
		break;
	#if RPG2003
	case 10: // 2nd timer
		switch (com[2]) {
		case 0:		// 以上
			result = (timer_[1]->left() >= com[1] * 60);
			break;
		case 1:		// 以下
			result = (timer_[1]->left() <= com[1] * 60);
			break;
		}
		break;
	#endif
	}

	if( !result ) {
		if( bool( com[5] ) ) { current_->skipToElse( com.nest(), 22010 ); }
		else { current_->skipToEndOfJunction( com.nest(), com.code() ); }
	}
}

namespace
{
	std::deque<rpg2k::SystemString> tokenizeSelect(rpg2k::SystemString const& str)
	{
		kuto_assert( str.size() );

		std::string::size_type begin = 0, end = 0;
		std::deque<rpg2k::SystemString> ret;

		while(end != std::string::npos) {
			end = str.find('/', begin);
			ret.push_back( str.substr(begin, end) );
			begin = end + 1;
		}

		return ret;
	}
}

PP_protoType(CODE_SELECT_START)
{
	openGameSelectWindow();
	std::deque<rpg2k::SystemString> selList = tokenizeSelect( com.string().toSystem() );
	for(std::size_t i = 0; i < selList.size(); i++) {
		selectWindow_.addLine(selList[i]);
	}
	selectWindow_.enableCancel(com[0] != 0);

	setWait(true);
}
PP_protoTypeWait(CODE_SELECT_START)
{
	if (!selectWindow_.closed()) return;

	setWait(false);
	selectWindow_.freeze();

	if( com[0] == 0 ) {
		current_->skipToEndOfJunction( com.nest(), com.code() );
	} else {
		// TODO: string check with "tokenizeSelect()"
		int const selectIndex = selectWindow_.canceled()? com[0] : selectWindow_.cursor() + 1;
		for(int i = 0; i < selectIndex; i++) {
			current_->skipToElse( com.nest(), 20140 );
		}
	}
}

PP_protoType(CODE_GAMEOVER)
{
	field_.game().gameOver();
	setWait(true);
}

PP_protoType(CODE_TITLE)
{
	field_.game().returnTitle();
	setWait(true);
}

PP_protoType(CODE_EVENT_BREAK)
{
	current_->ret();
}

PP_protoType(CODE_EVENT_CLEAR)
{
	Array2D& evSt = cache_.lsd->eventState();
	Array2D::iterator it = evSt.find( current_->eventID() );
	if( it != evSt.end() ) {
		evSt.erase(it);
	}
}

PP_protoType(CODE_LOOP_START)
{
	current_->startLoop( com.nest() );
}

PP_protoType(CODE_LOOP_BREAK)
{
	current_->breakLoop();
}

PP_protoType(CODE_LOOP_END)
{
	current_->continueLoop();
}

PP_protoType(CODE_WAIT)
{
	setWaitCount(com[0] * rpg2k::FRAME_PER_SECOND / 10);
}

PP_protoTypeWait(CODE_WAIT)
{
}

PP_protoType(CODE_PICT_SHOW)
{
	setPictureInfo( *cache_.lsd, com );
}

PP_protoType(CODE_PICT_MOVE)
{
	setPictureInfo( *cache_.lsd, com );

	setWaitCount( com[15] );
}

PP_protoTypeWait(CODE_PICT_MOVE)
{
	if( !field_.pictureManager().isMoving(com[0]) ) {
		setWait(false);
	}
}

PP_protoType(CODE_PICT_CLEAR)
{
	Array2D& pict = cache_.lsd->picture();
	Array2D::iterator it = pict.find( com[0] );
	if( it != pict.end() ) {
		pict.erase(it);
	}
}

PP_protoType(CODE_SYSTEM_SCREEN)
{
	field_.setFadeInfo(com[0], com[1]);

	Array1D& lsdSys = cache_.lsd->system();

	lsdSys[ 111+com[0] ].toBinary()[0] = com[1];
}

PP_protoType(CODE_SCREEN_CLEAR)
{
	field_.fadeOut(com[0]);
}

PP_protoType(CODE_SCREEN_SHOW)
{
	field_.fadeIn(com[0]);
}

PP_protoType(CODE_SCREEN_SCROLL)
{
	GameMap& map = field_.map();
	if (com[0] < 2)
		map.enableScroll((bool)com[0]);
	else {
		if (com[0] == 2) {
			int x = 0;
			int y = 0;
			if (com[1] == 0)	// 上
				y = -com[2];
			else if (com[1] == 1)	// 右
				x = com[2];
			else if (com[1] == 2)	// 下
				y = com[2];
			else if (com[1] == 3)	// 左
				x = -com[2];
			map.scroll(x, y, (float)(1 << com[3]));
		} else
			map.scrollBack((float)(1 << com[3]));
		if ( bool( com[4] ) ) {
			setWait(true);
		}
	}
}

PP_protoTypeWait(CODE_SCREEN_SCROLL)
{
	if (!field_.map().isScrolling())
		setWait(false);
}

PP_protoType(CODE_CHARA_TRANS)
{
	cache_.lsd->system()[55] = !bool(com[0]);
}


PP_protoType(CODE_CHARA_MOVE)
{
	// TODO
/*
	GameChara* chara = getCharaFromEventId(com[0]);
	if (!chara)
		return;
	// int frequency = com[1];
	CRpgRoute route;
	route.repeat = com[2] == 1;
	route.ignore = com[3] == 1;
	for (uint i = 4; i < com.argNum(); i++) {
		route.commands.push_back(com.at(i));
		switch (com.at(i)) {
		case 32:	// スイッチON
		case 33:	// スイッチOFF
			route.extraIntParam.push_back(com.extraIntParam(i, 0));
			break;
		case 34:	// グラフィック変更
			route.extraStringParam.push_back(com.extraStringParam(i, 0));
			route.extraIntParam.push_back(com.extraIntParam(i, 0));
			break;
		case 35:	// 効果音の演奏
			route.extraStringParam.push_back(com.extraStringParam(i, 0));
			route.extraIntParam.push_back(com.extraIntParam(i, 0));
			route.extraIntParam.push_back(com.extraIntParam(i, 1));
			route.extraIntParam.push_back(com.extraIntParam(i, 2));
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
	// TODO
/*
	if (!routeSetChara_)
		return;
	routeSetChara_->startRoute();
 */
}

PP_protoType(CODE_MOVEALL_CANCEL)
{
	// TODO
/*
	if (!routeSetChara_)
		return;
	routeSetChara_->endRoute();
 */
}

PP_protoType(CODE_NAME_INPUT)
{
	nameInputMenu_.freeze(false);
	nameInputMenu_.setPlayerInfo(com[0], (bool)com[1], (bool)com[2]);
	setWait(true);
}

PP_protoTypeWait(CODE_NAME_INPUT)
{
	if (nameInputMenu_.closed()) {
		setWait(false);
		nameInputMenu_.freeze();
	}
}

PP_protoType(CODE_EVENT_SWAP)
{
	SaveData& lsd = *cache_.lsd;

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
	SaveData& lsd = *cache_.lsd;
	EventState& state = lsd.eventState(com[0]);

	uint x, y;
	switch(com[1]) {
		case 0:
			x = com[2];
			y = com[3];
			break;
		case 1:
			x = lsd.var(com[2]);
			y = lsd.var(com[3]);
			break;
		default: kuto_assert(false);
	}

	state[12] = x;
	state[13] = y;
}

PP_protoType(CODE_PARTY_NAME)
{
	cache_.project->character(com[0]).setName( com.string() );
}

PP_protoType(CODE_PARTY_TITLE)
{
	cache_.project->character(com[0]).setTitle( com.string() );
}

PP_protoType(CODE_PARTY_WALK)
{
	Array1D& charData = cache_.lsd->character()[ com[0] ];

	charData[11] = com.string();
	charData[12] = com[1];
	charData[13] = bool(com[2]);
}

PP_protoType(CODE_PARTY_FACE)
{
	Array1D& charData = cache_.lsd->character()[ com[0] ];

	charData[21] = com.string();
	charData[22] = com[1];
}

PP_protoType(CODE_MM_BGM_PLAY)
{
	rpg2k::structure::Music& mus = cache_.lsd->system()[75];

	mus.clear();
	mus[1] = com.string();
	mus[2] = com[0];
	mus[3] = com[1];
	mus[4] = com[2];
	mus[5] = com[3];

	// TODO: play "music"
}
PP_protoType(CODE_MM_BGM_SAVE)
{
	Array1D& sys = cache_.lsd->system();

	sys[78] = sys[75].toMusic();
}
PP_protoType(CODE_MM_BGM_LOAD)
{
	Array1D& sys = cache_.lsd->system();

	sys[75] = sys[78].toMusic();

	// TODO: play "sys[75]"
}

PP_protoType(CODE_TELEPORT)
{
	Array2D& points = (*cache_.lsd)[110];

	switch(com[0]) {
	// add teleport link
		case 0: {
			Array1D& point =  points[ com[1] ];
			point[1] = com[1]; // mapID
			point[2] = com[2]; // x
			point[3] = com[3]; // y
			switch(com[4]) {
				case 0:
					point[4] = false;
					break;
				case 1:
					point[4] = true;
					point[5] = com[5];
					break;
			}
		} break;
	// remove teleport link
		case 1: {
			Array2D::iterator it = points.find(com[1]);
			if( it != points.end() ) {
				points.erase(it);
			}
		} break;
		default: kuto_assert(false);
	}
}
PP_protoType(CODE_TELEPORT_PERM)
{
	cache_.lsd->system()[121] = bool( com[0] );
}
PP_protoType(CODE_ESCAPE)
{
	Array1D& point = (*cache_.lsd)[110].toArray2D()[0];

	point[1] = com[0]; // mapID
	point[2] = com[1]; // x
	point[3] = com[2]; // y
	switch(com[3]) {
		case 0:
			point[4] = false;
			break;
		case 1:
			point[4] = true;
			point[5] = com[4];
			break;
		default: kuto_assert(false);
	}
}
PP_protoType(CODE_ESCAPE_PERM)
{
	cache_.lsd->system()[122] = bool( com[0] );
}

PP_protoType(CODE_SAVE_PERM)
{
	cache_.lsd->system()[123] = bool( com[0] );
}

PP_protoType(CODE_MENU_PERM)
{
	cache_.lsd->system()[124] = bool( com[0] );
}

PP_protoType(CODE_OPERATE_KEY)
{
	setWaitCount( bool( com[1] ) );
	if ( !isWaiting() ) {
		cache_.lsd->setVar(com[0], inputKeyValue(com));
	}
}

PP_protoTypeWait(CODE_OPERATE_KEY)
{
	int key = inputKeyValue(com);
	if (key != EventKey::NONE) {
		setWait(false);
		cache_.lsd->setVar(com[0], key);
	}
}

PP_protoType(CODE_PANORAMA)
{
	Array1D& map = (*cache_.lsd)[111];
	map[31] = true;
	map[32] = com.string();
	map[33] = bool(com[0]);
	map[34] = bool(com[1]);
	map[35] = bool(com[2]);
	map[36] = int(com[3]);
	map[37] = bool(com[4]);
	map[38] = int(com[5]);
}

PP_protoType(CODE_INN)
{
	openGameSelectWindow();

	DataBase& ldb = *cache_.ldb;
	int base = 80 + com[0] * 5;
	std::string mes = ldb.vocabulary(base + 0);
	std::ostringstream oss;
	oss << com[1];
	mes += oss.str();
	mes += ldb.vocabulary(95).toSystem();
	mes += ldb.vocabulary(base + 1);
	selectWindow_.addLine(mes);
	selectWindow_.addLine(ldb.vocabulary(base + 2));
	selectWindow_.addLine(ldb.vocabulary(base + 3), cache_.lsd->money() >= com[1]);
	selectWindow_.addLine(ldb.vocabulary(base + 4));
	selectWindow_.setCursorStart(2);
	selectWindow_.enableCancel();

	setWait(true);
}

PP_protoTypeWait(CODE_INN)
{
	if (!selectWindow_.closed()) return;

	setWait(false);
	selectWindow_.freeze();

	int selectIndex = selectWindow_.cursor();
	if (selectWindow_.canceled())
		selectIndex = 3;
	if (selectIndex == 2) {
		cache_.lsd->addMoney(-com[1]);
		std::vector<uint16_t> const& mem = cache_.lsd->member();
		for (uint i = 0; i < mem.size(); i++) {
			cache_.project->character(mem[i]).cure();
		}
	}
	if ( bool( com[2] ) ) {
		current_->skipToElse( com.nest(), (selectIndex == 2)? 20730 : 20731 );
	}
}

PP_protoType(CODE_SHOP)
{
	shopMenu_.freeze(false);
	int shopType = com[0];
	int mesType = com[1];
	std::vector<int> items;
	for (uint i = 4; i < com.argNum(); i++)
		items.push_back(com.at(i));
	shopMenu_.setShopData(shopType, mesType, items);
	setWait(true);
}

PP_protoTypeWait(CODE_SHOP)
{
	if (!shopMenu_.closed()) return;

	shopMenu_.freeze();
	setWait(false);
	if ( bool( com[2] ) ) {
		current_->skipToElse( com.nest(), shopMenu_.buyOrSell()? 20720 : 20721 );
	}
}

PP_protoType(CODE_MM_SOUND)
{
	// TODO
}

PP_protoType(CODE_SCREEN_COLOR)
{
	// TODO
	if( bool(com[5]) ) { setWaitCount(com[4]); }
}

PP_protoTypeWait(CODE_SCREEN_COLOR)
{
}

PP_protoType(CODE_BTLANIME)
{
	GameSkillAnime* anime = addChild( GameSkillAnime::createTask(*cache_.project, com[0]));
	int const& eventId = com[1];
	EventState& chara = cache_.lsd->eventState(eventId);
	anime->setPlayPosition(kuto::Vector2(chara.x() * 16.f, chara.y() * 16.f));
	setWaitCount( bool( com[2] ) );
	if ( isWaiting() )
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
		setWait(false);
	}
}

PP_protoType(CODE_PARTY_SOUBI)
{
	Project& proj = *cache_.project;
	SaveData& lsd = *cache_.lsd;

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	switch(com[2]) {
	// equip
		case 0: {
			uint itemID;
			switch(com[4]) {
				case 0: itemID = (com[3] == 0) ? 1 : com[3]; break;
				case 1: itemID = lsd.var(com[3]); break;
			}

			for(TargetCharacter::const_iterator it = target.begin(); it != target.end(); it++) {
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
			for(TargetCharacter::const_iterator it = target.begin(); it != target.end(); ++it) {
				proj.unequip( *it, static_cast< rpg2k::Equip::Type >(end) );
			}
		} break;
	}
}

PP_protoType(CODE_PARTY_REFRESH)
{
	Project& proj = *cache_.project;

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	for(size_t i = 0; i < target.size(); i++) {
		Project::Character& c = proj.character(target[i]);
		c.setHP( c.param(rpg2k::Param::HP) );
		c.setMP( c.param(rpg2k::Param::MP) );
	}
}

PP_protoType(CODE_PARTY_EXP)
{
	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	int const exp = com[3] == 0? com[4] : cache_.lsd->var(com[4]);
	kuto::StaticVector<std::pair<unsigned, int>, rpg2k::MEMBER_MAX> levelUpList;
	for (uint i = 0; i < target.size(); i++) {
		Project::Character& c = cache_.project->character(target[i]);
		if ( c.addExp(com[2] == 0? exp : -exp) ) {
			levelUpList.push_back( std::make_pair(target[i], c.level()) );
			c.addLevel();
		}
	}
	if (com[5] == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(levelUpList[i].first, levelUpList[i].second);
		}
		setWait(true);
	}
}

PP_protoType(CODE_PARTY_LV)
{
	Project& proj = *cache_.project;
	SaveData& lsd = *cache_.lsd;
	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	int level = com[3] == 0? com[4] : lsd.var(com[4]);
	kuto::StaticVector<std::pair<unsigned, int>, rpg2k::MEMBER_MAX> levelUpList;
	for (uint i = 0; i < target.size(); i++) {
		Project::Character& c = proj.character(target[i]);
		int oldLevel = c.level();
		c.addLevel(com[2] == 0? level : -level);
		if (c.level() > oldLevel) {
			levelUpList.push_back(std::make_pair(target[i], oldLevel));
		}
	}
	if (com[5] == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(levelUpList[i].first, levelUpList[i].second);
		}
		setWait(true);
	}
}

PP_protoType(CODE_PARTY_POWER)
{
	SaveData& lsd = *cache_.lsd;
	Array2D& charDatas = lsd.character();

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
		case 1: val = lsd.var(val); break;
	}
	switch(com[2]) {
		case 0: val =  val; break;
		case 1: val = -val; break;
	}

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	for(TargetCharacter::const_iterator it = target.begin(); it != target.end(); ++it) {
		charDatas[*it][index] = charDatas[*it][index].to<int>() + val;
	}
}

PP_protoType(CODE_PARTY_SKILL)
{
	SaveData& lsd = *cache_.lsd;
	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	int skillId = com[3] == 0? com[4] : lsd.var(com[4]);
	for (uint i = 0; i < target.size(); i++) {
		Project::Character& c = cache_.project->character(target[i]);
		std::set<uint16_t>& skill = c.skill();
		std::set<uint16_t>::iterator it = skill.find(skillId);
		if (com[2] == 0)
		{ if(it == skill.end()) skill.insert(skillId); }
		else
		{ if(it != skill.end()) skill.erase(it); }
	}
}

PP_protoType(CODE_EVENT_GOSUB)
{
	int mapEvID, pageNo;
	switch(com[0]) {
		case 0: { // common event
			current_->call( cache_.ldb->commonEvent()[ com[1] ][22].toEvent() );
		} return;
		case 1: // map event immediate
			mapEvID = com[1];
			pageNo  = com[2];
			break;
		case 2: { // map event variable
			mapEvID = cache_.lsd->var( com[1] );
			pageNo  = cache_.lsd->var( com[2] );
		} break;
	}
	if( mapEvID == rpg2k::ID_THIS ) mapEvID = cache_.lsd->currentEventID();
	current_->call( cache_.lmu->event()[mapEvID][5].toArray2D()[pageNo][52].toEvent() );
}

PP_protoType(CODE_OPERATE_INPUT)
{
	// TODO
}
PP_protoTypeWait(CODE_OPERATE_INPUT)
{
	// TODO
}

PP_protoType(CODE_PARTY_HP)
{
	SaveData& lsd = *cache_.lsd;

	int op;
	switch(com[3]) {
		case 0: op = com[4]; break;
		case 1: op = lsd.var(com[4]); break;
	}
	if(op > rpg2k::CHAR_HP_MAX) op = rpg2k::CHAR_HP_MAX;
	// if(com[4] == 0);
	switch(com[2]) {
		case 0: op =  op; break;
		case 1: op = -op; break;
	}

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	for(std::size_t i = 0; i < target.size(); i++) {
		Element& dst = lsd.character()[ target[i] ][71];
		if( ( dst.to<int>() - op ) <= 0 ) dst = bool(com[5])? 0 : 1;
		else dst = dst.to<int>() - op;
	}
}
PP_protoType(CODE_PARTY_MP)
{
	SaveData& lsd = *cache_.lsd;

	int op;
	switch(com[3]) {
		case 0: op = com[4]; break;
		case 1: op = lsd.var(com[4]); break;
	}
	if(op > rpg2k::CHAR_HP_MAX) op = rpg2k::PARAM_MAX;
	// if(com[4] == 0);
	switch(com[2]) {
		case 0: op =  op; break;
		case 1: op = -op; break;
	}

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	for(std::size_t i = 0; i < target.size(); i++) {
		Element& dst = lsd.character()[ target[i] ][72];
		if( ( dst.to<int>() - op ) < 0 ) dst = 0;
		else dst = dst.to<int>() - op;
	}
}
PP_protoType(CODE_PARTY_STATE)
{
	// TODO
}
PP_protoType(CODE_PARTY_DAMAGE)
{
	Project& proj = *cache_.project;
	SaveData& lsd = *cache_.lsd;

	TargetCharacter const target = targetCharacter(Target::Type(com[0]), com[1]);
	for(TargetCharacter::const_iterator it = target.begin(); it < target.end(); ++it) {
		int result =
			( float(com[2]) * ( 1.f + kuto::random(float(com[5]) * 0.1f) - float(com[5]) * 0.05f ) )
			- proj.paramWithEquip(*it, rpg2k::Param::GAURD) * float(com[3]) * 0.01f
			- proj.paramWithEquip(*it, rpg2k::Param::MIND ) * float(com[4]) * 0.01f
			;
		Element& dst = lsd.character()[*it][71];
		dst = (dst.to<int>() - result) > 0? dst.to<int>() - result : 0;

		if( bool(com[6]) ) {
			lsd.setVar( com[7], result );
		}
	}
}

PP_protoType(CODE_SYSTEM_VEHICLE)
{
	EventState& info = (*cache_.lsd)[ 105+com[0] ];
	info[73] = com.string();
	info[74] = com[1];
}
PP_protoType(CODE_SYSTEM_BGM)
{
	int index;
	switch(com[0]) {
		case 0: case 1: case 2:
			index = 72 + com[0];
			break;
		case 3: case 4: case 5: case 6:
			index = 79 + com[0];
			break;
	}
	Music& dst = cache_.lsd->system()[index];
	dst[1] = com.string();
	dst[2] = com[1];
	dst[3] = com[2];
	dst[4] = com[3];
	dst[5] = com[4];
}
PP_protoType(CODE_SYSTEM_SOUND)
{
	Sound& dst = cache_.lsd->system()[ 91 + com[0] ];
	dst[1] = com.string();
	dst[3] = com[1];
	dst[4] = com[2];
	dst[5] = com[3];
}
PP_protoType(CODE_SYSTEM_GRAPHIC)
{
	Array1D& lsdSys = cache_.lsd->system();

	lsdSys[21] = com.string();
	lsdSys[22] = com[0];
	lsdSys[23] = com[1];
}

PP_protoType(CODE_VEH_RIDE)
{
	// TODO
}
PP_protoType(CODE_VEH_LOCATE)
{
	SaveData& lsd = *cache_.lsd;

	boost::array<unsigned, 3> point;
	switch(com[1]) {
		case 0:
			for(uint i = 0; i < 3; i++) point[i] = com[1+i];
			break;
		case 1:
			for(uint i = 0; i < 3; i++) point[i] = lsd.var(com[1+i]);
			break;
	}

	switch(com[0]) {
		case 0: case 1: case 2: {
			EventState& state = lsd.eventState(rpg2k::ID_BOAT + com[0]);
			boost::array<unsigned, 3>::const_iterator it = point.begin();

			state[11] = *(it++);
			state[12] = *(it++);
			state[13] = *(it++);
		} break;
	}
}

PP_protoType(CODE_EVENT_END)
{
	current_->ret();
}
PP_protoType(CODE_IF_END)
{
	// TODO
}
PP_protoType(CODE_GOTO_MOVE)
{
	Event::LabelTable const& table = current_->event().labelTable();
	Event::LabelTable::const_iterator it = table.find( com[0] );
	kuto_assert( it != table.end() );
	current_->setPointer(it->second);
}
PP_protoType(CODE_SELECT_END)
{
	// TODO
}
PP_protoType(CODE_SHOP_IF_START)
{
	// TODO
}
PP_protoType(CODE_SHOP_IF_ELSE)
{
	// TODO
}
PP_protoType(CODE_SHOP_IF_END)
{
	// TODO
}
PP_protoType(CODE_INN_IF_END)
{
	// TODO
}

PP_protoType(CODE_MM_BGM_FADEOUT)
{
	// TODO
}
PP_protoType(CODE_MM_MOVIE)
{
	// TODO
}

PP_protoType(CODE_SCREEN_SHAKE)
{
	// TODO
}
PP_protoType(CODE_SCREEN_FLASH)
{
	// TODO
}
PP_protoType(CODE_SCREEN_WEATHER)
{
	// TODO
}

PP_protoType(CODE_CHARA_FLASH)
{
	// TODO
}

PP_protoType(CODE_TXT_SHOW_ADD)
{
	// TODO
}

PP_protoType(CODE_LAND_ID)
{
	Project& proj = *cache_.project;
	MapUnit & lmu = *cache_.lmu;
	SaveData& lsd = *cache_.lsd;

	uint x, y;
	switch(com[0]) {
		case 0:
			x = com[1];
			y = com[2];
			break;
		case 1:
			x = lsd.var(com[2]);
			y = lsd.var(com[3]);
			break;
	}

	lsd.setVar(
		com[3],
		proj.terrainID( lmu.chipIDLw(x, y) )
	);
}
PP_protoType(CODE_EVENT_ID)
{
	SaveData& lsd = *cache_.lsd;
	Array2D& states = lsd.eventState();

	int x, y;
	switch(com[0]) {
		case 0:
			x = com[1];
			y = com[2];
			break;
		case 1:
			x = lsd.var(com[1]);
			y = lsd.var(com[2]);
			break;
	}

	unsigned result = 0; // TODO: invalid value
	for(Array2D::Iterator it = states.begin(); it != states.end(); ++it) {
		if(
			it->second->exists() &&
			( (*it->second)[12].to<int>() == x ) &&
			( (*it->second)[13].to<int>() == y )
		) {
			result = it->first;
			break; // TODO: priority things
		}
	}

	lsd.setVar(com[3], result);
}
PP_protoType(CODE_CHIPSET)
{
	(*cache_.lsd)[111].toArray1D()[5] = com[0];
}
PP_protoType(CODE_ENCOUNT)
{
	(*cache_.lsd)[111].toArray1D()[3] = com[0];
}

PP_protoType(CODE_CHIP_SWAP)
{
	cache_.lsd->replace( rpg2k::ChipSet::Type(com[0]), com[1], com[2] );
}

PP_protoType(CODE_SAVE_SHOW)
{
	saveMenu_.start();
	setWait(true);
}
PP_protoTypeWait(CODE_SAVE_SHOW)
{
	if (saveMenu_.isEnd()) {
		saveMenu_.freeze();
		setWait(false);
	}
}
PP_protoType(CODE_MENU_SHOW)
{
	field_.startSystemMenu();
	setWait(true);
}
PP_protoTypeWait(CODE_MENU_SHOW)
{
	if (field_.systemMenu().isEnd()) {
		// field_.endSystemMenu(); // will automatically called
		setWait(false);
	}
}

PP_protoType(CODE_BTL_GO_END)
{
	// TODO
}
PP_protoType(CODE_BTL_ANIME)
{
	// TODO
}

PP_protoType(CODE_2003_JOB)
{
	// TODO
}
PP_protoType(CODE_2003_BTL_CMD)
{
	// TODO
}
PP_protoType(CODE_2003_ATK_REPEAT)
{
	// TODO
}
PP_protoType(CODE_2003_ESCAPE100)
{
	// TODO
}
PP_protoType(CODE_2003_BTL_COMMON)
{
	// TODO
}
