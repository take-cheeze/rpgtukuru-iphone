/**
	@file
	@brief		RPG_RT.ldb(LcfDataBase)を管理するクラス
	@author		sue445
*/
#ifndef _INC_CRPGLDB
#define _INC_CRPGLDB

#include <vector>
#include <kuto/kuto_types.h>
#include "CRpgIOBase.h"
#include "CRpgEvent.h"


/// RPG_RT.ldb(LcfDataBase)を管理するクラス
class CRpgLdb : public CRpgIOBase{
private:
	const char* GetHeader(){ return "LcfDataBase"; }	///< ファイルごとの固有ヘッダ(CRpgIOBaseからオーバーライドして使います)

public:
	/// ステータス
	struct Status {
		u16			maxHP;			///< 0:最大HP
		u16			maxMP;			///< 1:最大MP
		u16			attack;			///< 2:攻撃力
		u16			defence;		///< 3:防御力
		u16			magic;			///< 4:精神力
		u16			speed;			///< 5:敏捷力
	};
	/// 装備
	struct Equip {
		u16			weapon;			///< 0:武器
		u16			shield;			///< 1:盾
		u16			protector;		///< 2:防具
		u16			helmet;			///< 3:兜
		u16			accessory;		///< 4:装飾品
	};
	/// 習得特技
	struct LearnSkill {
		int			level;
		int			skill;
	};
	/// 主人公
	struct Player {
		std::string		name;				///< 0x01	名前
		std::string		title;				///< 0x02	肩書き
		std::string		walkGraphicName;	///< 0x03	歩行グラフィック/ファイル名
		int				walkGraphicPos;		///< 0x04	歩行グラフィック/位置
		bool			walkGraphicSemi;	///< 0x05	歩行グラフィック/半透明かどうか
		int				startLevel;			///< 0x07	初期レベル
		int				maxLevel;			///< 0x08	最高レベル
		bool			criticalEnable;		///< 0x09	必殺確率/有効
		int				criticalRatio;		///< 0x0A	必殺確率/○回中1回 (1 - 100)
		std::string		faceGraphicName;	///< 0x0F	顔グラフィック/ファイル名
		int				faceGraphicPos;		///< 0x10	顔グラフィック/位置
		bool			twinSword;			///< 0x15	オプション/二刀流
		bool			fixEquip;			///< 0x16	オプション/装備固定
		bool			execAI;				///< 0x17	オプション/強制AI行動
		bool			strongGuard;		///< 0x18	オプション/強力防御
		std::vector<Status>	status;		///< 0x1F	各レベルごとのステータス
		int				expBase;			///< 0x29	経験値曲線/基本値
		int				expGain;			///< 0x2A	経験値曲線/増加度
		int				expOffset;			///< 0x2B	経験値曲線/補正値
		Equip			initEquip;			///< 0x33	初期装備
		int				bareHandsAnime;		///< 0x38	素手で攻撃した時の戦闘アニメ
		std::vector<LearnSkill>	learnSkill;		///< 0x3F	特殊技能/各レベルで習得する特殊技能
		bool			useUserCommandName;	///< 0x42	特殊技能/独自戦闘コマンド名/使うかどうか
		std::string		userCommandName;	///< 0x43	特殊技能/独自戦闘コマンド名/名称
		std::vector<char>	conditionAvailability;	///< 0x48	状態有効度/データ
		std::vector<char>	attributeAvailavility;	///< 0x4A	属性有効度/データ
	};
	enum SkillType {
		kSkillTypeNormal,		///< 0: 通常
		kSkillTypeTeleport,		///< 1: テレポート
		kSkillTypeEscape,		///< 2: エスケープ
		kSkillTypeSwitch,		///< 3: スイッチ
	};
	enum SkillScope {
		kSkillScopeEnemySingle,		///< 0: 敵単体
		kSkillScopeEnemyAll,		///< 1: 敵全体
		kSkillScopeMyself,			///< 2: 使用者
		kSkillScopeFriendSingle,	///< 3: 味方単体
		kSkillScopeFriendAll,		///< 4: 味方全体
	};
	/// 特殊技能
	struct Skill {
		std::string		name;				///< 0x01	名前
		std::string		explain;			///< 0x02	説明
		std::string		message1;			///< 0x03	使用時メッセージ(1行目)
		std::string		message2;			///< 0x04	使用時メッセージ(2行目)
		int				messageMiss;		///< 0x07	失敗時メッセージ 0 - 3
		int				type;				///< 0x08	種別
		int				consumeMPFix;		///< 0x0B	消費MP/固定
		int				scope;				///< 0x0C	効果範囲
		int				anime;				///< 0x0E	戦闘アニメ
		int				attackRatio;		///< 0x15	打撃関係度
		int				magicRatio;			///< 0x16	精神関係度
		int				variance;			///< 0x17	数値分散度
		int				baseEffect;			///< 0x18	基本効果量
		int				baseSuccess;		///< 0x19	基本成功率
		bool			statusDownHP;		///< 0x1F	能力値低下/HP
		bool			statusDownMP;		///< 0x20	能力値低下/MP
		bool			statusDownAttack;	///< 0x21	能力値低下/攻撃力
		bool			statusDownDefence;	///< 0x22	能力値低下/防御力
		bool			statusDownMagic;	///< 0x23	能力値低下/精神力
		bool			statusDownSpeed;	///< 0x24	能力値低下/敏捷性
		bool			statusDownAbsorption;			///< 0x25	能力値低下/(吸収)
		bool			statusDownIgnoreDefence;		///< 0x26	能力値低下/(防御無視)
		std::vector<bool>	conditionChange;	///< 0x2A	状態変化(治療)/データ
		std::vector<bool>	attribute;			///< 0x2B	攻撃(防御)属性/データ数
		bool			attributeDefenceDown;			///< 0x2D	攻撃属性/属性防御低下/上昇
		int				onSwitch;			///< 0x0D	ONにするスイッチ
		bool			useField;			///< 0x12	使用可能な場面/フィールド
		bool			useBattle;			///< 0x13	使用可能な場面/バトル
	};
	/// アイテムタイプ
	enum ItemType {
		kItemTypeNormal,		///< 0:通常物品
		kItemTypeWeapon,		///< 1:武器
		kItemTypeShield,		///< 2:盾
		kItemTypeProtector,		///< 3:鎧
		kItemTypeHelmet,		///< 4:兜
		kItemTypeAccessory,		///< 5:装飾品
		kItemTypeMedicine,		///< 6:薬
		kItemTypeBook,			///< 7:本
		kItemTypeSeed,			///< 8:種
		kItemTypeSpecial,		///< 9:特殊
		kItemTypeSwitch,		///< 10:スイッチ
	};
	enum ItemScope {
		kItemScopeSingle,
		kItemScopeAll,
	};
	/// アイテム
	struct Item {
		std::string		name;				///< 0x01	名前
		std::string		explain;			///< 0x02	説明
		int				type;				///< 0x03	種別
		int				price;				///< 0x05	値段
		int				useCount;			///< 0x06	使用回数
		int				statusUpAttack;		///< 0x0B	能力上昇量/攻撃力
		int				statusUpDefence;	///< 0x0C	能力上昇量/防御力
		int				statusUpMagic;		///< 0x0D	能力上昇量/精神力
		int				statusUpSpeed;		///< 0x0E	能力上昇量/敏捷性
		int				handle;				///< 0x0F	持ち手
		int				consumeMP;			///< 0x10	消費MP
		int				baseHitRatio;		///< 0x11	基本命中率
		int				criticalUp;			///< 0x12	必殺確率増加
		int				anime;				///< 0x14	戦闘アニメ
		bool			firstAttack;		///< 0x15	オプション/ターンの最初に先制攻撃
		bool			doubleAttack;		///< 0x16	オプション/一度に2回攻撃
		bool			wholeAttack;		///< 0x17	オプション/一度敵全体を攻撃
		bool			ignoreEnemyAvoidance;	///< 0x18	オプション/敵の回避率を無視
		bool			protectCritical;	///< 0x19	オプション/必殺(痛恨の一撃)防止
		bool			attackAvoidanceUp;	///< 0x1A	オプション/物理攻撃の回避率アップ
		bool			halfConsumeMP;		///< 0x1B	オプション/MP消費量半分
		bool			protectDamageTerrain;	///< 0x1C	オプション/地形ダメージ無効
		int				scope;				///< 0x1F	効果範囲
		int				cureHPRatio;		///< 0x20	HP回復量/割合
		int				cureHPValue;		///< 0x21	HP回復量/絶対量
		int				cureMPRatio;		///< 0x22	MP回復量/割合
		int				cureMPValue;		///< 0x23	MP回復量/絶対量
		bool			fieldOnly;			///< 0x25	オプション/フィールド(メニュー画面)でのみ使用可
		bool			deadOnly;			///< 0x26	オプション/戦闘不能のキャラクターにのみ有効
		int				statusChangeHP;		///< 0x29	能力値変化量/最大HP
		int				statusChangeMP;		///< 0x2A	能力値変化量/最大MP
		int				statusChangeAttack;	///< 0x2B	能力値変化量/攻撃力
		int				statusChangeDefence;	///< 0x2C	能力値変化量/防御力
		int				statusChangeMagic;	///< 0x2D	能力値変化量/精神力
		int				statusChangeSpeed;	///< 0x2E	能力値変化量/敏捷性
		int				messageType;		///< 0x33	使用時メッセージ
		int				skill;				///< 0x35	習得特殊技能
		int				onSwitch;			///< 0x37	ONにするスイッチ
		bool			useField;			///< 0x39	使用可能な場面/フィールド
		bool			useBattle;			///< 0x3A	使用可能な場面/バトル
		std::vector<bool>	equipPlayer;		///< 0x3E	装備可能な主人公/データ
		std::vector<bool>	conditionChange;	///< 0x40	状態変化/データ
		std::vector<bool>	attribute;			///< 0x42	攻撃属性/データ
		int				conditionChangeRatio;	///< 0x43	状態変化確率
		bool			revertCondition;	///< 0x44	状態/「状態変化」と「状態治療」の反転
	};
	/// 攻撃パターン
	struct AttackPattern {
		int			type;					///< 0x01	行動内容ラジオ部分
		int			base;					///< 0x02	行動内容/基本行動
		int			skill;					///< 0x03	行動内容/特殊技能
		int			transform;				///< 0x04	行動内容/変身
		int			conditionType;			///< 0x05	行動条件/種類
		int			conditionNumberA;		///< 0x06	行動条件/数値A
		int			conditionNumberB;		///< 0x07	行動条件/数値B
		int			conditionSwitch;		///< 0x08	行動条件/スイッチ
		bool		onSwitchEnable;			///< 0x09	行動後にONにするスイッチ/チェック
		int			onSwitchID;				///< 0x0A	行動後にONにするスイッチ
		bool		offSwitchEnable;		///< 0x0B	行動後にOFFにするスイッチ/チェック	
		int			offSwitchID;			///< 0x0C	行動後にOFFにするスイッチ
		int			priority;				///< 0x0D	優先度
	};
	/// 敵キャラ
	struct Enemy {
		std::string			name;				///< 0x01	名前
		std::string			graphicName;		///< 0x02	敵キャラグラフィック/ファイル名
		int					graphicColor;		///< 0x03	敵キャラグラフィック/色相
		Status				status;				///< 0x04〜0x09	能力値
		bool				graphicSemi;		///< 0x0A	敵キャラグラフィック/半透明
		int					resultExp;			///< 0x0B	戦利品/経験値
		int					resultGold;			///< 0x0C	戦利品/お金
		int					resultItem;			///< 0x0D	戦利品/アイテム
		int					resultItemDropRatio;			///< 0x0E	戦利品/アイテム/アイテム出現率
		bool				criticalEnable;		///< 0x15	必殺確率/有効
		int					criticalRatio;		///< 0x16	必殺確率/○回中1回
		bool				missRush;			///< 0x1A	オプション/通常時のミス多発
		bool				fly;				///< 0x1C	敵キャラグラフィック/空中
		std::vector<char>	conditionAvailability;	///< 0x20	状態有効度/データ
		std::vector<char>	attributeAvailavility;	///< 0x22	属性有効度/データ
		std::vector<AttackPattern>	attackPattern;	///< 0x2A	攻撃パターン
	};
	/// 敵配置情報
	struct EnemyPlacement {
		int				enemyID;		///< 0x01	敵キャラID
		int				x;				///< 0x02	X位置
		int				y;				///< 0x03	Y位置
		bool			breakIn;		///< 0x04	途中から出現
	};
	/// 敵グループ
	struct EnemyGroup {
		std::string		name;			///< 0x01	名前
		std::vector<EnemyPlacement>	placement;		///< 0x02	敵リスト
		std::vector<bool>			appearTerrain;	///< 0x05	出現可能地形/データ
	};
	/// 地形
	struct Terrain {
		std::string		name;				///< 0x01	名前
		int				damage;				///< 0x02	ダメージ
		int				encounterRatio;		///< 0x03	敵出現倍率
		std::string		battleGraphic;		///< 0x04	戦闘背景グラフィック/ファイル名
		bool			passBoat;			///< 0x05	小型船で通行可能
		bool			passVessel;			///< 0x06	大型船で通行可能
		bool			passAirship;		///< 0x07	飛行船で通行可能
		bool			landAirship;		///< 0x09	飛行船で着陸可能
		int				charaDisplayMode;	///< 0x0B	通常キャラの表示方法
	};
	/// 属性
	struct Attribute {
		std::string		name;				///< 0x01	名前
		int				type;				///< 0x02	種別
		int				effectRatioA;		///< 0x0B	効果量変動率/A
		int				effectRatioB;		///< 0x0C	効果量変動率/B
		int				effectRatioC;		///< 0x0D	効果量変動率/C	
		int				effectRatioD;		///< 0x0E	効果量変動率/D
		int				effectRatioE;		///< 0x0F	効果量変動率/E
	};
	enum LimitActionType {
		kLimitActionNone,
		kLimitActionDoNotAction,
		kLimitActionAttackEnemy,
		kLimitActionAttackFriend,
	};
	/// 状態
	struct Condition {
		std::string		name;				///< 0x01	名前
		int				type;				///< 0x02	種別
		int				color;				///< 0x03	表示色
		int				priority;			///< 0x04	優先度
		int				limitAction;		///< 0x05	行動制限
		int				applyRatioA;		///< 0x0B	異常発生率/A
		int				applyRatioB;		///< 0x0C	異常発生率/B
		int				applyRatioC;		///< 0x0D	異常発生率/C
		int				applyRatioD;		///< 0x0E	異常発生率/D
		int				applyRatioE;		///< 0x0F	異常発生率/E
		int				cureTurn;			///< 0x15	治療方法/自然治癒/最低○ターン持続
		int				cureRatioNatural;	///< 0x16	治療方法/自然治癒/○%の確率で治る
		int				cureRatioImpact;	///< 0x17	治療方法/衝撃による治癒/○%の確率で治る
		bool			changeAttack;		///< 0x1F	能力値/攻撃力
		bool			changeDefence;		///< 0x20	能力値/防御力
		bool			changeMagic;		///< 0x21	能力値/精神力
		bool			changeSpeed;		///< 0x22	能力値/敏捷性
		int				changeHitRatio;		///< 0x23	命中変化値
		bool			enableAttackRatio;	///< 0x29	使用できなくなる特殊技能/打撃関係度/チェック	
		int				attackRatio;		///< 0x2A	使用できなくなる特殊技能/打撃関係度
		bool			enableMagicRatio;	///< 0x2B	使用できなくなる特殊技能/精神関係度/チェック
		int				magicRatio;			///< 0x2C	使用できなくなる特殊技能/精神関係度
		std::string		friendMessage;		///< 0x33	味方がこの状態になったときのメッセージ	
		std::string		enemyMessage;		///< 0x34	敵がこの状態になったときのメッセージ
		std::string		alreadyMessage;		///< 0x35	すでにこの状態になっているときのメッセージ
		std::string		myTurnMessage;		///< 0x36	この状態のときの自分のターンのメッセージ
		std::string		cureMessage;		///< 0x37	この状態が回復したときのメッセージ
		int				changeHPBattleRatio;	///< 0x3D	HP/戦闘中毎ターン変化するHP/%
		int				changeHPBattleValue;	///< 0x3E	HP/戦闘中毎ターン変化するHP/P
		int				changeHPMoveRatio;		///< 0x3F	HP/マップ移動中に変化するHP/○歩ごとに	
		int				changeHPMoveValue;		///< 0x40	HP/マップ移動中に変化するHP/P
		int				changeMPBattleRatio;	///< 0x41	MP/戦闘中毎ターン変化するMP/%
		int				changeMPBattleValue;	///< 0x42	MP/戦闘中毎ターン変化するMP/P
		int				changeMPMoveRatio;		///< 0x43	MP/マップ移動中に変化するMP/○歩ごとに	
		int				changeMPMoveValue;		///< 0x44	MP/マップ移動中に変化するMP/P
	};
	enum FlashScope {
		kFlashScopeNone,
		kFlashScopeTarget,
		kFlashScopeScreen,
	};
	/// 効果音とフラッシュのタイミング
	struct TimingOfFlash {
		int				frame;			///< 0x01 フレーム番号
		int				scope;			///< 0x03 フラッシュの範囲
		int				colorR;			///< 0x04 色（赤） 0 - 31
		int				colorG;			///< 0x05 色（緑）
		int				colorB;			///< 0x06 色（青）
		int				strength;		///< 0x07 色（強さ）
	};
	/// セル
	struct AnimeCell {
		bool			visible;		///< 0x01 表示する
		int				pattern;		///< 0x02 パターン番号-1
		int				x;				///< 0x03 相対X座標
		int				y; 				///< 0x04 相対Y座標
		int				scale;			///< 0x05 拡大率
		int				colorR;			///< 0x06 色調（赤）
		int				colorG;			///< 0x07 色調（緑）
		int				colorB;			///< 0x08 色調（青）		
		int				colorIntensity;	///< 0x09 色調（彩度）
		int				colorA;			///< 0x0A 透明度
	};
	/// フレーム
	struct AnimeFrame {
		sueLib::smart_array<AnimeCell>	cells;		///< 0x01	セルの情報
	};
	enum AnimeScope {
		kAnimeScopeSingle,
		kAnimeScopeAll,
	};
	enum AnimeBaseLine {
		kAnimeBaseLineTop,
		kAnimeBaseLineCenter,
		kAnimeBaseLineBottom,
	};
	/// 戦闘アニメ
	struct BattleAnime {
		std::string		name;				///< 0x01	名前
		std::string		filename;			///< 0x02	ファイル名
		std::vector<TimingOfFlash>	timingOfFlashes;		///< 0x06	効果音とフラッシュのタイミング
		int				scope;				///< 0x09	適用範囲
		int				baseLine;			///< 0x0A	Ｙ座標標準ライン
		bool			useGrid;			///< 0x0B	グリッド使用
		std::vector<AnimeFrame>		animeFrames;			///< 0x0C  	フレームの情報
	};
	enum {
		LOWER_CHIP_NUM 		= 162,		///< 下層チップ数
		UPPER_CHIP_NUM 		= 144,		///< 上層チップ数
		FLAG_MOVE_DOWN 		= 1<<0,		///< 下に行ける
		FLAG_MOVE_LEFT 		= 1<<1,		///< 左に行ける
		FLAG_MOVE_RIGHT 	= 1<<2,		///< 左に行ける
		FLAG_MOVE_UP 		= 1<<3,		///< 上に行ける
		FLAG_CHARACTER_UP 	= 1<<4,		///< キャラの上に表示
		FLAG_WALL 			= 1<<5,		///< 壁属性
		FLAG_COUNTER		= 1<<6,		///< カウンター属性
		SEA_ANIME_1232		= 0,		///< 海チップアニメ方式 1-2-3-2
		SEA_ANIME_123		= 1,		///< 海チップアニメ方式 1-2-3
		SEA_ANIME_SPEED_SLOW	= 0,	///< 海チップアニメ速度 低速
		SEA_ANIME_SPEED_HIGH	= 1,	///< 海チップアニメ速度 高速
	};
	/// チップセット
	struct ChipSet{
		std::string strName;		///< 0x01:名前
		std::string strFile;		///< 0x02:ファイル名
		std::vector<short>		randforms;		///< 0x03:地形
		std::vector<char>		blockLower;		///< 0x04:下層チップの通行・ブロック
		std::vector<char>		blockUpper;		///< 0x05:上層チップの通行・ブロック
		int			seaAnimeType;	///< 0x0B:海チップアニメ方式
		int			seaAnimeSpeed;	///< 0x0C:海チップアニメ速度
	};
	/// システム
	struct System {
		std::string		boat;		///< 0x0B	小型船
		std::string		vessel;		///< 0x0C	大型船
		std::string		airship;	///< 0x0D	飛行船
		std::string		title;		///< 0x11	タイトル
		std::string		gameover;	///< 0x12	ゲームオーバー
		std::string		system;		///< 0x13	システム
		std::vector<short>		startParty;		///< 0x15	初期パーティー
	};
	/// Battle用語
	struct BattleTerm {
		std::string		battleStart;	///< 戦闘開始  	(キャラ名)[0x01]
		std::string		firstAttack;	///< 先制行動のチャンス  	[0x02]
		std::string		escapeSuccess;	///< 逃走成功  	[0x03]
		std::string		escapeMiss;		///< 逃走失敗  	[0x04]
		std::string		win;			///< 勝利  	[0x05]
		std::string		lose;			///< 全滅  	[0x06]
		std::string		getExp;			///< 経験値獲得  	(X)[0x07]
		std::string		getMoney[2];		///< お金獲得  	[0x08](X)(通貨単位・[0x5F])[0x09]  	
		std::string		getItem;		///< アイテム獲得  	(アイテム名)[0x0A]	
		std::string		normalAttack;	///< 通常攻撃  	(キャラ名)[0x0B]  
		std::string		criticalFriend;	///< 味方必殺(会心の一撃)  	[0x0C]
		std::string		criticalEnemy;	///<  敵必殺(痛恨の一撃)  	[0x0D]  	
		std::string		guard;			///< 防御する  	(キャラ名)[0x0E]  	
		std::string		seek;			///< 様子を見る(敵のみ)  	(キャラ名)[0x0F]  	
		std::string		charge;			///< 力をためる(敵のみ)  	(キャラ名)[0x10]  	
		std::string		suicideBombing;	///< 自爆する(敵のみ)  	(キャラ名)[0x11]  	
		std::string		escapeEnemy;	///< 逃げる(敵のみ)  	(キャラ名)[0x12]  	
		std::string		transform;		///< 変身する(敵のみ)  	(キャラ名)[0x13]  	
		std::string		enemyDamage;	///< 敵にダメージを与えた  	(キャラ名)に(X)[0x14]
		std::string		enemyNoDamage;	///< 敵にダメージを与えられない  	(キャラ名)[0x15]  	
		std::string		friendDamage;	///< 味方がダメージを受けた  	(キャラ名)は(X)[0x16]  	
		std::string		friendNoDamage;	///< 味方がダメージを受けていない  	(キャラ名)[0x17]  	
		std::string		skillMiss[3];	///< 特殊技能失敗ABC  	(キャラ名)[0x18 19 1A]  	
		std::string		attackMiss;		///< 物理攻撃回避  	(キャラ名)[0x1B]
		std::string		useItem;		///< アイテム使用  	(キャラ名)は(アイテム名)[0x1C]  	
		std::string		paramCure;		///< 能力値回復  	(キャラ名)の(能力値名)が(X)[0x1D]  	
		std::string		paramUp;		///< 能力値上昇  	(キャラ名)の(能力値名)が(X)[0x1E]  	
		std::string		paramDown;		///< 能力値下降  	(キャラ名)の(能力値名)が(X)[0x1F]  	
		std::string		friendAbsorb;	///< 味方が能力値を吸収  	(キャラ名)の(能力値名)を(X)[0x20]  	
		std::string		enemyAbsorb;	///< 敵が能力値を吸収  	(キャラ名)は(能力値名)を(X)[0x21]  	
		std::string		attributeUp;	///< 属性防御上昇  	(キャラ名)は(属性名)[0x22]  	
		std::string		attributeDown;	///< 属性防御下降  	(キャラ名)は(属性名)[0x23]  	
		std::string		levelUp;		///< レベルアップ  	(キャラ名)は(レベル)(X)[0x24]  	
		std::string		getSkill;		///< 特殊技能習得  	(特殊技能名)[0x25]  	
	};
	struct ShopTerm {
		std::string		what;			///< 用件選択  	[0x29]  	
		std::string		what2;			///< 用件選択(二度目以降)  	[0x2A]  	
		std::string		buy;			///< 購入する  	[0x2B]  	
		std::string		sell;			///< 売却する  	[0x2C]  	
		std::string		cancel;			///< やめる  	[0x2D]  	
		std::string		buyItem;		///< 購入アイテムの選択  	[0x2E]  	
		std::string		buyNum;			///< 購入個数入力  	[0x2F]  	
		std::string		buyEnd;			///< 購入完了  	[0x30]  	
		std::string		sellItem;		///< 売却アイテム選択  	[0x31]  	
		std::string		sellNum;		///< 売却個数入力  	[0x32]  	
		std::string		sellEnd;		///< 売却完了  	[0x33]  	
	};
	struct InnTerm {
		std::string		what[3];		///< 宿泊選択  	[0x50](X)(通貨単位・[0x5F])[0x51][0x52]
		std::string		ok;			///< 泊まる  	[0x53]
		std::string		cancel;		///< 泊まらない  	[0x54]
	};
	struct ShopParamTerm {
		std::string		itemGet;		///< アイテム所持数  	[0x5C]  	
		std::string		itemEquiped;	///< アイテム装備数  	[0x5D]  	
		std::string		money;			///< 通貨単位  	[0x5F]  	
	};
	struct ParamTerm {
		std::string		level;		///< レベル  	[0x7B]  	
		std::string		hp;			///< HP  	[0x7C]  	
		std::string		mp;			///< MP  	[0x7D]  	
		std::string		condition;	///< 正常  	[0x7E]  	
		std::string		exp;		///< 経験値(略)  	[0x7F]  	
		std::string		levelShort;	///< レベル(略)  	[0x80]  	
		std::string		hpShort;	///< HP(略)  	[0x81]  	
		std::string		mpShort;	///< MP(略)  	[0x82]  	
		std::string		consumeMp;	///< 消費MP  	[0x83]  	
		std::string		attack;		///< 攻撃力  	[0x84]  	
		std::string		defence;	///< 防御力  	[0x85]  	
		std::string		magic;		///< 精神力  	[0x86]  	
		std::string		speed;		///< 敏捷性  	[0x87]  	
		std::string		weapon;		///< 武器  	[0x88]  	
		std::string		shield;		///< 盾  	[0x89]  	
		std::string		protector;	///< 鎧  	[0x8A]  	
		std::string		helmet;		///< 兜  	[0x8B]  	
		std::string		accessory;	///< 装飾品  	[0x8C]  	
	};
	struct MenuTerm {
		std::string		battle;			///< 戦う  	[0x65]  	
		std::string		autoBattle;		///< オート 	[0x66] 	
		std::string		escape;			///< 逃げる 	[0x67] 	
		std::string		attack;			///< 攻撃 	[0x68] 	
		std::string		guard;			///< 防御 	[0x69] 	
		std::string		item;			///< アイテム 	[0x6A] 	
		std::string		skill;			///< 特殊技能 	[0x6B] 	
		std::string		equip;			///< 装備 	[0x6C] 	
		std::string		save;			///< セーブ 	[0x6E] 	
		std::string		endGame;		///< ゲーム終了 	[0x70]
	};
	struct TitleTerm {
		std::string		newGame;		///< ニューゲーム  	[0x72]
		std::string		loadGame;		///< コンティニュー  	[0x73]
		std::string		endGame;		///< シャットダウン  	[0x75]
	};
	struct SaveLoadTerm {
		std::string		selectSave;		///< セーブファイル選択  	[0x92]  	
		std::string		selectLoad;		///< ロードファイル選択 	[0x93] 	
		std::string		checkEndGame;	///< ゲーム終了確認 	[0x94] 	
		std::string		file;			///< ファイル 	[0x97] 	
		std::string		yes;			///< はい 	[0x98] 	
		std::string		no;				///< いいえ 	[0x99]
	};
	/// 用語
	struct Term {
		BattleTerm		battle;
		ShopTerm		shop[3];
		InnTerm			inn[2];
		ShopParamTerm	shopParam;
		ParamTerm		param;
		MenuTerm		menu;
		TitleTerm		title;
		SaveLoadTerm	saveLoad;
	};
	/// スイッチ
	struct Switch {
		std::string		name;				///< 0x01	名前
	};
	/// 変数
	struct Var {
		std::string		name;				///< 0x01	名前
	};
	/// コモンイベント
	struct CommonEvent {
		std::string		name;				///< 0x01	名前
		CRpgEventList	eventList;			///< イベントリスト
	};

private:
	void AnalyzePlayer(sueLib::smart_buffer& buf);
	void AnalyzeSkill(sueLib::smart_buffer& buf);
	void AnalyzeItem(sueLib::smart_buffer& buf);
	void AnalyzeEnemy(sueLib::smart_buffer& buf);
	void AnalyzeEnemyGroup(sueLib::smart_buffer& buf);
	void AnalyzeTerrain(sueLib::smart_buffer& buf);
	void AnalyzeAttribute(sueLib::smart_buffer& buf);
	void AnalyzeCondition(sueLib::smart_buffer& buf);
	void AnalyzeBattleAnime(sueLib::smart_buffer& buf);
	void AnalyzeChipSet(sueLib::smart_buffer& buf);
	void AnalyzeSystem(sueLib::smart_buffer& buf);
	void AnalyzeTerm(sueLib::smart_buffer& buf);
	void AnalyzeSwitch(sueLib::smart_buffer& buf);
	void AnalyzeVar(sueLib::smart_buffer& buf);
	void AnalyzeCommonEvent(sueLib::smart_buffer& buf);
	
public:
	sueLib::smart_array< Player >	saPlayer;
	sueLib::smart_array< Skill >	saSkill;
	sueLib::smart_array< Item >		saItem;
	sueLib::smart_array< Enemy >	saEnemy;
	sueLib::smart_array< EnemyGroup >	saEnemyGroup;
	sueLib::smart_array< Terrain >	saTerrain;
	sueLib::smart_array< Attribute >	saAttribute;
	sueLib::smart_array< Condition >	saCondition;
	sueLib::smart_array< BattleAnime >	saBattleAnime;
	sueLib::smart_array< ChipSet >	saChipSet;
	System							system;
	Term							term;
	sueLib::smart_array< Switch >	saSwitch;
	sueLib::smart_array< Var >	saVar;
	sueLib::smart_array< CommonEvent >	saCommonEvent;

private:
	std::string		rootFolder_;		///< Root Folder Name

public:
	CRpgLdb(){}								///< コンストラクタ
	~CRpgLdb(){}							///< デストラクタ

	bool Init(const char* szDir="");		///< 初期化
	const std::string& getRootFolder() const { return rootFolder_; }	
};


#endif
