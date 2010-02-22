#ifndef _INC__RPG2K__MODEL__DATA_BASE_HPP
#define _INC__RPG2K__MODEL__DATA_BASE_HPP

#include "Model.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class DataBase : protected Base
		{
		protected:
			void init();

			virtual string getHeader() const { return "LcfDataBase"; }
			virtual string defaultName() const { return "RPG_RT.ldb"; }
		public:
			DataBase(string dir);
			DataBase(string dir, string name);
			virtual ~DataBase();

			using Base::getDirectory;
			using Base::operator [];
			using Base::save;

			string getRootFolder() { return getDirectory(); }

			Array2D& getCharacter() { return (*this)[11]; }
			Array2D& getSkill() { return (*this)[12]; }
			Array2D& getItem() { return (*this)[13]; }
			Array2D& getEnemy() { return (*this)[14]; }
			Array2D& getEnemyGroup() { return (*this)[15]; }
			Array2D& getTerrain() { return (*this)[16]; }
			Array2D& getAttribute() { return (*this)[17]; }
			Array2D& getCondition() { return (*this)[18]; }
			Array2D& getBattleAnime() { return (*this)[19]; }
			Array2D& getChipSet() { return (*this)[20]; }

			Array1D& getVocabulary() { return (*this)[21]; }
			Array1D& getSystem() { return (*this)[22]; }

			Array2D& getSwitch  () { return (*this)[23]; }
			Array2D& getVariable() { return (*this)[24]; }
			Array2D& getCommonEvent() { return (*this)[25]; }

			const Array2D& getCharacter() const { return (*this)[11]; }
			const Array2D& getSkill() const { return (*this)[12]; }
			const Array2D& getItem() const { return (*this)[13]; }
			const Array2D& getEnemy() const { return (*this)[14]; }
			const Array2D& getEnemyGroup() const { return (*this)[15]; }
			const Array2D& getTerrain() const { return (*this)[16]; }
			const Array2D& getAttribute() const { return (*this)[17]; }
			const Array2D& getCondition() const { return (*this)[18]; }
			const Array2D& getBattleAnime() const { return (*this)[19]; }
			const Array2D& getChipSet() const { return (*this)[20]; }

			const Array1D& getVocabulary() const { return (*this)[21]; }
			const Array1D& getSystem() const { return (*this)[22]; }

			const Array2D& getSwitch  () const { return (*this)[23]; }
			const Array2D& getVariable() const { return (*this)[24]; }
			const Array2D& getCommonEvent() const { return (*this)[25]; }

			enum LimitActionType {
				kLimitActionNone,
				kLimitActionDoNotAction,
				kLimitActionAttackEnemy,
				kLimitActionAttackFriend,
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
			enum FlashScope {
				kFlashScopeNone,
				kFlashScopeTarget,
				kFlashScopeScreen,
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
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
