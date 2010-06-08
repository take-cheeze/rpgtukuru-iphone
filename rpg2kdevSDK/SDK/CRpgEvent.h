/**
	@file
	@brief		イベントデータ1つ分
	@author		sue445
*/
#ifndef _INC_CRPGEVENT
#define _INC_CRPGEVENT

#include "CRpgIOBase.h"
#include <vector>
#include <string>
#include <stdint.h> // #include <kuto/kuto_types.h>


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


/// イベントデータ1つ分を管理するクラス
class CRpgEvent
{
friend class CRpgEventStream;
public:
	template< typename T > struct ExtraParam { int index, subIndex; T value; };
	typedef ExtraParam< int > ExtraIntParam;
	typedef ExtraParam< std::string > ExtraStringParam;
/*
	struct ExtraIntParam {
		int		index;
		int		subIndex;
		int		value;
	};
	struct ExtraStringParam {
		int			index;
		int			subIndex;
		std::string	value;
	};
 */

private:
	int					eventCode_;			///< 命令の種類(ツクール内コード)
	int 				nest_;				///< ネスト数
	std::vector<int>	intParams_;			///< 整数引数リスト
	std::string			stringParam_;		///< 文字列引数
	std::vector<ExtraIntParam>		extraIntParam_;		///< 特別なIntParam
	std::vector<ExtraStringParam>	extraStringParam_;	///< 特別なStringParam	

private:
	template< typename ExtraType >
	const ExtraType& getExtraParam(const std::vector< ExtraType >& exList, int index, int subIndex) const {
		for (typename std::vector< ExtraType >::const_iterator i = exList.begin(); i < exList.end(); ++i) {
			if (i->index == index && i->subIndex == subIndex)
				return *i;
		}
		return exList[0];
	}
/*
	template<class ReturnType, class ListType>
	const ReturnType& getExtraParam(const ListType& exList, int index, int subIndex) const {
		for (u32 i = 0; i < exList.size(); i++) {
			if (exList[i].index == index && exList[i].subIndex == subIndex)
				return exList[i];
		}
		return exList[0];
	}
 */

public:
	CRpgEvent() : eventCode_(0), nest_(0) {}		///< コンストラクタ

	int getEventCode() const { return eventCode_; }	///< イベントコードを取得
	int getNest() const { return nest_; }		///< イベントのネスト数を取得
	int getIntParamNum() const { return intParams_.size(); }		///< 整数引数の個数取得
	int getIntParam(int index) const { return intParams_[index]; }	///< 整数引数取得
	const std::string& getStringParam() const { return stringParam_; }	///< 文字列引数取得
	const ExtraIntParam& getExtraIntParam(int index, int subIndex) const { return getExtraParam<ExtraIntParam>(extraIntParam_, index, subIndex); }
	const ExtraStringParam& getExtraStringParam(int index, int subIndex) const { return getExtraParam<ExtraStringParam>(extraStringParam_, index, subIndex); }
};

/// イベント出現条件
class CRpgEventCondition
{
public:
	enum Flag {
		kFlagSwitch1	= 1<<0,		///< 0ビット目:◇スイッチ[%04d:%s]がON(1)\n
		kFlagSwitch2	= 1<<1,		///< 1ビット目:◇スイッチ[%04d:%s]がON(2)\n
		kFlagVar		= 1<<2,		///< 2ビット目:◇変数[%04d:%s]の値が[%d]以上\n
		kFlagItem		= 1<<3,		///< 3ビット目:◇アイテム[%04d:%s]を所持\n
		kFlagPlayer		= 1<<4,		///< 4ビット目:◇主人公[%04d:%s]がいる\n
		kFlagTimer		= 1<<5,		///< 5ビット目:◇タイマー[%d分%02秒]以下
	};
	enum StartType {
		kStartTypeButton,		///< 0:◇決定キーが押されたとき
		kStartTypeTouchPlayer,	///< 1:◇主人公から触れたとき
		kStartTypeTouchEvent,	///< 2:◇イベントから触れたとき
		kStartTypeAuto,			///< 3:◇自動的に始まる
		kStartTypeParallel,		///< 4:◇定期的に並列処理する
		kStartTypeCalled,		///< 5:◇呼び出されたときのみ
	};

public:
	CRpgEventCondition()
	: nFlag(0), nSw1(1), nSw2(1), nVarNum(1), nVarOver(0), nItem(1), nChara(1), nTimer(0), nStart(0) {}		///< コンストラクタ

public:
	int nFlag;		///< 0x01:チェックボックスのチェック状態(6ビットフラグ) (Flag)
	int nSw1;		///< 0x02:スイッチ(1)		初期値:1
	int nSw2;		///< 0x03:スイッチ(2)		初期値:1
	int nVarNum;	///< 0x04:変数番号		初期値:1
	int nVarOver;	///< 0x05:変数が〜以上
	int nItem;		///< 0x06:アイテム〜を所持	初期値:1
	int nChara;		///< 0x07:主人公〜がいる	初期値:1
	int nTimer;		///< 0x08:タイマー(秒)
			
	int nStart;		///< 0x21:開始条件 (StartType)
};

/// イベントリスト
class CRpgEventList
{
public:
	CRpgEventCondition			condition;
	std::vector<CRpgEvent>		events;		///< 0x16	イベントデータ/データ
};

/// ルート設定
class CRpgRoute
{
public:
	enum CommandType {
		kComMoveUp,		///< 0:上に移動
		kComMoveRight,	///< 1:右に移動
		kComMoveDown,	///< 2:下に移動
		kComMoveLeft,	///< 3:左に移動
		kComMoveRightUp,	///< 4:右上に移動
		kComMoveRightDown,	///< 5:右下に移動
		kComMoveLeftDown,	///< 6:左下に移動
		kComMoveLeftUp,		///< 7:左上に移動
		kComMoveRandom,		///< 8:ランダムに移動
		kComApproachPlayer,	///< 9:主人公に近寄る
		kComEscapePlayer,	///< 10:主人公から逃げる
		kComMoveForward,	///< 11:一歩前進
		kComSightUp,		///< 12:上を向く
		kComSightRight,		///< 13:右を向く
		kComSightDown,		///< 14:下を向く
		kComSightLeft,		///< 15:左を向く
		kComTurnRight90,	///< 16:右に90度回転
		kComTurnLeft90,		///< 17:左に90度回転
		kComTurn180,		///< 18:180度方向転換
		kComTurnRandom90,	///< 19:右か左に90度回転
		kComTurnRandom,		///< 20:ランダムに方向転換
		kComTurnPlayer,		///< 21:主人公の方を向く
		kComTurnPlayerRev,	///< 22:主人公の逆を向く
		kComPause,			///< 23:一時停止
		kComJumpStart,		///< 24:ジャンプ開始
		kComJumpEnd,		///< 25:ジャンプ終了
		kComSightPause,		///< 26:向き固定
		kComSightStart,		///< 27:向き固定解除
		kComSpeedUp,		///< 28:移動速度アップ
		kComSpeedDown,		///< 29:移動速度ダウン
		kComFrequencyUp,	///< 30:移動頻度アップ
		kComFriquencyDown,	///< 31:移動頻度ダウン
		kComSwitchOn,		///< 32:スイッチON o [スイッチ番号]
		kComSwitchOff,		///< 33:スイッチOF o [スイッチ番号]
		kComGrapnicsChange,	///< 34:グラフィック変更 o [ファイル名の長さ]  o [ファイル名の文字コード]×文字列長 o [歩行絵の位置番号]:左上から順に0〜7(上段が左から0〜3,下段が左から4〜7)
		kComPlaySe,			///< 35:効果音の演奏 o [ファイル名の長さ] o [ファイル名の文字コード]×文字列長 o [ボリューム]:「音量(0(無音)〜100(最大))」ではないっぽい o [テンポ]:「再生速度(50≦100(標準)≦150)」ではないっぽい
		kComThroughStart,	///< 36:すりぬけ開始
		kComThroughEnd,		///< 37:すりぬけ終了
		kComAnimePause,		///< 38:アニメ停止
		kComAnimeStart,		///< 39:アニメ再開
		kComAlphaUp,		///< 40:透明度アップ
		kComAlphaDown,		///< 41:透明度ダウン
	};
	
	CRpgRoute() : repeat(false), ignore(false) {}
	
	int getExtraIntParamValue(int index, int subIndex) {
		for (unsigned int i = 0; i < extraIntParam.size(); i++) {
			if (extraIntParam[i].index == index && extraIntParam[i].subIndex == subIndex)
				return extraIntParam[i].value;
		}
		return 0;
	}
	std::string getExtraStringParamValue(int index, int subIndex) {
		for (unsigned int i = 0; i < extraStringParam.size(); i++) {
			if (extraStringParam[i].index == index && extraStringParam[i].subIndex == subIndex)
				return extraStringParam[i].value;
		}
		return "";
	}

public:
	std::vector<int>	commands;		///< 0x0C:移動パターンのデータ
	bool				repeat;			///< 0x15:オプション・動作を繰り返す
	bool				ignore;			///< 0x16:オプション･移動できない時は無視	
	std::vector<CRpgEvent::ExtraIntParam>		extraIntParam;		///< 特別なIntParam
	std::vector<CRpgEvent::ExtraStringParam>	extraStringParam;	///< 特別なStringParam	
};


#endif
