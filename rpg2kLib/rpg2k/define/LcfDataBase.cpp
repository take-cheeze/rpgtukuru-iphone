#include "Define.hpp"

const char* rpg2k::define::LcfDataBase = (" \
Array1D Grobal \n \
{ \n \
	[11]: Array2D Character \n \
	{ \n \
		[1]: string name; \n \
		[2]: string title; \n \
		[3]: string charSet; \n \
		[4]: int    charSetPos = 0; \n \
		[5]: bool semiTrans = false; \n \
 \n \
		[ 7]: int  startLv      = 1   ; \n \
		[ 8]: int  maxLv        = 50  ; \n \
		[ 9]: bool critical     = true; \n \
		[10]: int  criticalRate = 30  ; \n \
 \n \
		[15]: string faceSet; \n \
		[16]: int    faceSetPos = 0; \n \
 \n \
		[21]: bool doubleHand    = false; \n \
		[22]: bool equipFix      = false; \n \
		[23]: bool aiAction      = false; \n \
		[24]: bool strongDefence = false; \n \
 \n \
		[31]: Binary status; // std::vector< uint16_t >[6][maxLv] \n \
 \n \
		[41]: int expBasic; \n \
		[42]: int expIncrease; \n \
		[43]: int expCorrection = 0; \n \
 \n \
		[51]: Binary equipment; // std::array<uint16_t, 5> \n \
		[56]: int    unarmedAttack = 1; // battle anime ID \n \
		[57]: int    job           = 0; \n \
 \n \
		[62]: int attackAnime = 0; \n \
		[63]: Array2D skill \n \
		{ \n \
			[1]: int level; \n \
			[2]: int skillID = 1; \n \
		}; \n \
		[66]: bool   useOriginalCommand = false; \n \
		[67]: string originalCommand; \n \
 \n \
		[71]: int    conditionDataNum = 0; \n \
		[72]: Binary conditionData; \n \
		[73]: int    attribDataNum = 0; \n \
		[74]: Binary attribData; \n \
 \n \
		[80]: Binary battleCommand; \n \
	}; \n \
	[12]: Array2D Skill \n \
	{ \n \
		[1]: string name; \n \
		[2]: string comment; \n \
		[3]: string messageL1; \n \
		[4]: string messageL2; \n \
		[7]: int messageFaild; \n \
		[8]: int type = 0; \n \
 \n \
		[11]: int usingMPAmount = 0; \n \
		[12]: int range = 0; \n \
		[13]: int switchID = 1; \n \
		[14]: int animeID  = 1; \n \
		[16]: Sound sound; \n \
		[18]: bool useAtField  = true ; \n \
		[19]: bool useAtBattle = false; \n \
 \n \
		[21]: int blowRelate = 0; \n \
		[22]: int mindRelate = 3; \n \
		[23]: int effectSpread = 4; \n \
		[24]: int basicEffect = 0; \n \
		[25]: int succeedRate = 100; \n \
 \n \
		[31]: bool decreaseHP = false; \n \
		[32]: bool decreaseMP = false; \n \
		[33]: bool decreaseAttack  = false; \n \
		[34]: bool decreaseDefence = false; \n \
		[35]: bool decreaseMind    = false; \n \
		[36]: bool decreaseSpeed   = false; \n \
		[37]: bool absorption = false; \n \
		[38]: bool ignoreDefence = false; \n \
 \n \
		[41]: int conditionDataNum = 0; \n \
		[42]: Binary conditionData; // std::vector< uint8_t > (as bool) \n \
		[43]: int attribDataNum = 0; \n \
		[44]: Binary attribData; // std::vector< uint8_t > (as bool) \n \
		[45]: bool changeAttrib; \n \
	}; \n \
	[13]: Array2D Item \n \
	{ \n \
		[1]: string name; \n \
		[2]: string comment; \n \
		[3]: int type  = 0; \n \
		[5]: int price = 0; \n \
		[6]: int usable = 1; \n \
 \n \
		[11]: int attack  = 0; \n \
		[12]: int defence = 0; \n \
		[13]: int mind    = 0; \n \
		[14]: int speed   = 0; \n \
 \n \
		[15]: int hand = 0; \n \
		[16]: int usingMP = 0; \n \
		[17]: int hitRate = 0; \n \
		[18]: int criticalRate = 0; \n \
 \n \
		[20]: int animeID = 1; \n \
 \n \
		[21]: bool firstStrike = false; \n \
		[22]: bool doubleAttack = false; \n \
		[23]: bool attackAll = false; \n \
		[24]: bool ignoreEvasionRate = false; \n \
		[25]: bool preventCritical = false; \n \
		[26]: bool increaseEvasionRate = false; \n \
		[27]: bool halfUsingMP = false; \n \
		[28]: bool ignoreTerrainDamage = false; \n \
		[29]: bool fixEquipment = false; \n \
 \n \
		[31]: int range = 0; \n \
		[32]: int hpAmount = 0; \n \
		[33]: int hpRate = 0; \n \
		[34]: int mpAmount = 0; \n \
		[35]: int mpRate = 0; \n \
 \n \
		[37]: bool onlyAtField = false; \n \
		[38]: bool onlyForKnockout = false; \n \
 \n \
		[41]: int increaseHP = 0; \n \
		[42]: int increaseMP = 0; \n \
		[43]: int increaseAttack  = 0; \n \
		[44]: int increaseDefence = 0; \n \
		[45]: int increaseMind    = 0; \n \
		[46]: int increaseSpeed   = 0; \n \
 \n \
		[51]: int messageType = 0; \n \
		[53]: int skillID = 1; \n \
		[55]: int switchID = 1; \n \
		[57]: bool useAtField  = true ; \n \
		// [58]: bool \n \
		[59]: bool useAtBattle = false; \n \
 \n \
		[61]: int equipDataNum = 0; \n \
		[62]: Binary equipData; // std::vector< uint8_t > (as bool) \n \
		[63]: int conditionDataNum = 0; \n \
		[64]: Binary conditionData; // std::vector< uint8_t > (as bool) \n \
		[65]: int attribDataNum = 0; \n \
		[66]: Binary attribData; // std::vector< uint8_t > (as bool) \n \
		[67]: int conditionRate = 0; \n \
		[68]: bool conditionFlip = false; \n \
		[69]: int useAnimeID; \n \
 \n \
		[71]: bool useSkillAsItem = false; \n \
		[72]: int jobDataNum = 0; \n \
		[73]: Binary jobData; \n \
	}; \n \
	[14]: Array2D Enemy \n \
	{ \n \
		[1]: string name; \n \
		[2]: string monster; \n \
		[3]: int colorTone = 0; \n \
		[4]: int HP = 10; \n \
		[5]: int MP = 10; \n \
		[6]: int attack  = 10; \n \
		[7]: int defence = 10; \n \
		[8]: int mind    = 10; \n \
		[9]: int speed   = 10; \n \
		[10]: bool semiTrans = false; \n \
		[11]: int exp = 0; \n \
		[12]: int money = 0; \n \
		[13]: int droppingItemID = 0; \n \
		[14]: int itemDropRate = 100; \n \
 \n \
		[21]: bool critical = false; \n \
		[22]: int criticalRate = 30; \n \
 \n \
		[26]: bool increaseMiss = false; \n \
		[28]: bool inTheAir = false; \n \
 \n \
		[31]: int conditionDataNum = 0; \n \
		[32]: Binary conditionData; \n \
		[33]: int attribDataNum = 0; \n \
		[34]: Binary attribData; \n \
 \n \
		[42]: Array2D ActionPattern \n \
		{ \n \
			[1]: int actionType; \n \
			[2]: int basicAction = 1; \n \
			[3]: int skillID = 1; \n \
			[4]: int enemyID = 1; \n \
			[5]: int attackTerm = 0; \n \
			[6]: int A = 0; \n \
			[7]: int B = 0; \n \
			[8]: int switchID = 1; \n \
			[9]: bool onAfterAction = false; \n \
			[10]: int onSwitchID = 1; \n \
			[11]: bool offAfterAction = false; \n \
			[12]: int offSwitchID = 1; \n \
			[13]: int priority = 50; \n \
		}; \n \
	}; \n \
	[15]: Array2D EnemyGroup \n \
	{ \n \
		[1]: string name; \n \
		[2]: Array2D EnemyEnum \n \
		{ \n \
			[1]: int enemyID = 1; \n \
			[2]: int x = 0; \n \
			[3]: int y = 0; \n \
			[4]: bool liveJoin = false; \n \
		}; \n \
		[4]: int terrainDataNum; \n \
		[5]: Binary terrainData; \n \
 \n \
		[11]: Array2D BattleEvent \n \
		{ \n \
			[2]: Array1D Term \n \
			{ \n \
				[1]: int flag = 0; \n \
/* \n \
					[bit] \n \
					[0]: switch1 \n \
					[1]: switch2 \n \
					[2]: variable \n \
					[3]: turn \n \
					[4]: consume \n \
					[5]: enemyState \n \
					[6]: charState \n \
*/ \n \
				[2]: int switchID1 = 1; \n \
 \n \
				[3]: int switchID2 = 1; \n \
 \n \
				[4]: int variableID = 1; \n \
				[5]: int value = 0; \n \
 \n \
				[6]: int trunFreq = 0; \n \
				[7]: int turnOffset = 0; \n \
 \n \
				[ 8]: int consumeFrom = 0; \n \
				[ 9]: int consumeTo   = 100; \n \
 \n \
				[10]: int enemyID = 0; \n \
				[11]: int enemyHPfrom = 0; \n \
				[12]: int enemyHPto   = 100; \n \
 \n \
				[13]: int charID = 1; \n \
				[14]: int charHPfrom = 0; \n \
				[15]: int charHPto   = 100; \n \
			}; \n \
 \n \
			[11]: int eventLength = 0; \n \
			[12]: Event event; \n \
		}; \n \
	}; \n \
	[16]: Array2D Terrain \n \
	{ \n \
		[1]: string name; \n \
		[2]: int damage = 0; \n \
		[3]: int rate = 100; \n \
		[4]: string backdrop ; \n \
		[5]: bool boat = false; \n \
		[6]: bool ship = false; \n \
		[7]: bool airship = true; \n \
		[9]: bool airshipLand = true; \n \
 \n \
		[11]: int charViewType = 0; \n \
	}; \n \
	[17]: Array2D Attribute \n \
	{ \n \
		[1]: string name; \n \
		[2]: int type = 0; \n \
 \n \
		[11]: int rateA = 300; \n \
		[12]: int rateB = 200; \n \
		[13]: int rateC = 100; \n \
		[14]: int rateD = 50; \n \
		[15]: int rateE = 0; \n \
	}; \n \
	[18]: Array2D Condition \n \
	{ \n \
		[1]: string name ; \n \
		[2]: int type = 0; \n \
		[3]: int color = 6; \n \
		[4]: int priority = 50; \n \
		[5]: int restrict = 0; \n \
 \n \
		[11]: int rateA = 100; \n \
		[12]: int rateB = 80; \n \
		[13]: int rateC = 60; \n \
		[14]: int rateD = 30; \n \
		[15]: int rateE = 0; \n \
 \n \
		[21]: int cureTurn = 0; \n \
		[22]: int cureRate = 0; \n \
		[23]: int shockCureRate = 0; \n \
 \n \
		[31]: bool attack  = false; \n \
		[32]: bool defence = false; \n \
		[33]: bool mind    = false; \n \
		[34]: bool speed   = false; \n \
		[35]: int hitRate = 100; \n \
 \n \
		[41]: bool disableBlowSkill = false; \n \
		[42]: int blowRelate = 0; \n \
		[43]: bool disableMindSkill = false; \n \
		[44]: int mindRelate = 0; \n \
 \n \
		[51]: string messageMember ; \n \
		[52]: string messageEnemy  ; \n \
		[53]: string messageAlready; \n \
		[54]: string messageLast   ; \n \
		[55]: string messageCure   ; \n \
 \n \
		[61]: int turnHPAmount  = 0; \n \
		[62]: int turnHPRate    = 0; \n \
		[63]: int fieldHPStep   = 0; \n \
		[64]: int fieldHPAmount = 0; \n \
		[65]: int turnMPAmount  = 0; \n \
		[66]: int turnMPRate    = 0; \n \
		[67]: int fieldMPStep   = 0; \n \
		[68]: int fieldMPAmount = 0; \n \
	}; \n \
	[19]: Array2D BattleAnime \n \
	{ \n \
		[1]: string name ; \n \
		[2]: string battle ; \n \
		[6]: Array2D Effect \n \
		{ \n \
			[1]: int frameID = 0; \n \
			[2]: Sound sound; \n \
			[3]: int range = 0; \n \
			[4]: int red   = 31; \n \
			[5]: int green = 31; \n \
			[6]: int blue  = 31; \n \
			[7]: int flash = 31; \n \
		}; \n \
		[9]: int range = 0; \n \
		[10]: int yBase = 0; \n \
		[11]: int useGrid = 0; \n \
		[12]: Array2D Frame \n \
		{ \n \
			[1]: Array2D Cell \n \
			{ \n \
				[1]: bool visible = true; \n \
				[2]: int battlePos; // or pattern \n \
				[3]: int x = 0; \n \
				[4]: int y = 0; \n \
				[5]: int magnify = 0; \n \
				[6]: int red    = 100; \n \
				[7]: int green  = 100; \n \
				[8]: int blue   = 100; \n \
				[9]: int chroma = 100; \n \
				[10]: int trans = 0; \n \
			}; \n \
		}; \n \
	}; \n \
	[20]: Array2D ChipSet \n \
	{ \n \
		[1]: string name; \n \
		[2]: string chipSet; \n \
		[3]: Binary terrainID; // std::vector< uint16_t > \n \
		[4]: Binary lower; // std::vector< uint8_t > \n \
		[5]: Binary upper; // std::vector< uint8_t > \n \
 \n \
		[11]: int oceanSequence = 0; \n \
		[12]: int oceanSpeed    = 0; \n \
	}; \n \
	[21]: Array1D Vocabulary \n \
	{ \n \
		[1]:string battleStart; \n \
		[2]:string firstStrike; \n \
		[3]:string escapeSuccess; \n \
		[4]:string escapeFailure; \n \
		[5]:string battleWon; \n \
		[6]:string battleLost; \n \
		[7]:string gainExp; \n \
		[8]:string earnMoneyBegin; \n \
		[9]:string earnMoneyEnd; \n \
		[10]:string itemGet; \n \
		[11]:string normalAttack; \n \
		[12]:string criticalAttackParty; \n \
		[13]:string criticalAttackEnemy; \n \
		[14]:string defend; \n \
		[15]:string doNothing; \n \
		[16]:string chargePower; \n \
		[17]:string selfDestruct; \n \
		[18]:string runAway; \n \
		[19]:string transform; \n \
		[20]:string   damageToEnemy; \n \
		[21]:string npDamageToEnemy; \n \
		[22]:string   damageToParty; \n \
		[23]:string npDamageToParty; \n \
		[24]:string skillFailureA; \n \
		[25]:string skillFailureB; \n \
		[26]:string skillFailureC; \n \
		[27]:string physicalAtrackFailure; \n \
		[28]:string itemUse; \n \
		[29]:string parameterRecovery; \n \
		[30]:string parameerUp; \n \
		[31]:string parameerDown; \n \
		[32]:string partyAbusorbParameter; \n \
		[33]:string enemyAbusorbParameter; \n \
		[34]:string attributeGaurdUp; \n \
		[35]:string attributeGaurdDown; \n \
		[36]:string levelUp; \n \
		[37]:string skillMastered; \n \
 \n \
		[41]:string selectActiion; // shop A \n \
		[42]:string selectActiionSecond; \n \
		[43]:string buy; \n \
		[44]:string sell; \n \
		[45]:string cancel; \n \
		[46]:string selectBuyingItem; \n \
		[47]:string buyingItemNum; \n \
		[48]:string endBuy; \n \
		[49]:string selectSellingItem; \n \
		[50]:string sellingItemNum; \n \
		[51]:string endSell; \n \
 \n \
		[54]:string v54 ; // shop B \n \
		[55]:string v55 ; \n \
		[56]:string v56 ; \n \
		[57]:string v57 ; \n \
		[58]:string v58 ; \n \
		[59]:string v59 ; \n \
		[60]:string v60 ; \n \
		[61]:string v61 ; \n \
		[62]:string v62 ; \n \
		[63]:string v63 ; \n \
		[64]:string v64 ; \n \
 \n \
		[67]:string v67 ; // shop C \n \
		[68]:string v68 ; \n \
		[69]:string v69 ; \n \
		[70]:string v70 ; \n \
		[71]:string v71 ; \n \
		[72]:string v72 ; \n \
		[73]:string v73 ; \n \
		[74]:string v74 ; \n \
		[75]:string v75 ; \n \
		[76]:string v76 ; \n \
		[77]:string v77 ; \n \
 \n \
		[80]:string mes0; // hotel A \n \
		[81]:string mes1; \n \
		[82]:string mes2; \n \
		[83]:string yes; \n \
		[84]:string no; \n \
		[85]:string v85 ; // hotel B \n \
		[86]:string v86 ; \n \
		[87]:string v87 ; \n \
		[88]:string v88 ; \n \
		[89]:string v89 ; \n \
 \n \
		[92]:string itemOwnedNum; \n \
		[93]:string itemEquipedNum; \n \
 \n \
		[95]:string moneyName; \n \
 \n \
		[101]:string battle; \n \
		[102]:string autoAction; \n \
		[103]:string escape; \n \
		[104]:string attack; \n \
		[105]:string defend; \n \
		[106]:string useItem; \n \
		[107]:string useSkill; \n \
		[108]:string equip; \n \
 \n \
		[110]:string save; \n \
 \n \
		[112]:string discardCurrent; \n \
 \n \
		[114]:string newGame; \n \
		[115]:string continue; \n \
 \n \
		[117]:string quit; \n \
 \n \
		[123]:string level; \n \
		[124]:string hp; \n \
		[125]:string mp; \n \
		[126]:string normalCondition; \n \
		[127]:string expShort; \n \
		[128]:string levelShort; \n \
		[129]:string hpShort; \n \
		[130]:string mpShort; \n \
		[131]:string usingMp; \n \
		[132]:string attack; \n \
		[133]:string defence; \n \
		[134]:string mind; \n \
		[135]:string speed; \n \
		[136]:string weapon; \n \
		[137]:string shield; \n \
		[138]:string armor; \n \
		[139]:string helmet; \n \
		[140]:string other; \n \
 \n \
		[146]:string saveSelectMessage; \n \
		[147]:string loadSelectMessage; \n \
		[148]:string saveDataPrefix; \n \
 \n \
		[151]:string remindDiscarding; \n \
		[152]:string yes; \n \
		[153]:string no ; \n \
	}; \n \
	[22]: Array1D System \n \
	{ \n \
		[11]: string boat; \n \
		[12]: string ship; \n \
		[13]: string airship; \n \
		[14]: int boatPos; \n \
		[15]: int shipPos; \n \
		[16]: int airshipPos; \n \
		[17]: string title; \n \
		[18]: string gameOver; \n \
		[19]: string system; \n \
 \n \
		[21]: int memberNum = 0; \n \
		[22]: Binary member; \n \
 \n \
		[31]: Music titeBGM; \n \
		[32]: Music battleBGM; \n \
		[33]: Music battleEndBGM; \n \
		[34]: Music innBGM; \n \
		[35]: Music boatBGM; \n \
		[36]: Music shipBGM; \n \
		[37]: Music airshipBGM; \n \
		[38]: Music gameOverBGM; \n \
 \n \
		[41]: Sound cursorMoveSE; \n \
		[42]: Sound enterSE; \n \
		[43]: Sound cancelSE; \n \
		[44]: Sound buzzerSE; \n \
		[45]: Sound battleStartSE; \n \
		[46]: Sound escapeSE; \n \
		[47]: Sound enemyAttackSE; \n \
		[48]: Sound enemyDamagedSE; \n \
		[49]: Sound memberDamagedSE; \n \
		[50]: Sound evasionSE; \n \
		[51]: Sound enemyDefeatSE; \n \
		[52]: Sound useItemSE; \n \
 \n \
		[61]: int moveErace = 0; \n \
		[62]: int moveDisplay = 0; \n \
		[63]: int battleStartErace = 0; \n \
		[64]: int battleStartDisplay = 0; \n \
		[65]: int battleEndErace = 0; \n \
		[66]: int battleEndDisplay = 0; \n \
 \n \
		[71]: int wallpaper = 0; \n \
		[72]: int font = 0; \n \
 \n \
		[81]: int dummy; \n \
		[82]: bool dummy; \n \
		[83]: int dummy; \n \
		[84]: string defBackdrop; \n \
		[85]: Array2D dummy \n \
		{ \n \
/* \n \
			[1]: int dummy; \n \
			[2]: int dummy; \n \
 \n \
			[11]: int dummy; \n \
			[12]: int dummy; \n \
			[13]: int dummy; \n \
			[14]: int dummy; \n \
			[15]: int dummy; \n \
*/ \n \
		}; \n \
 \n \
		[91]: int saveTime; \n \
	}; \n \
	[23]: Array2D Switch \n \
	{ \n \
		[1]: string name ; \n \
	}; \n \
	[24]: Array2D Variable \n \
	{ \n \
		[1]: string name ; \n \
	}; \n \
	[25]: Array2D CommonEvent \n \
	{ \n \
		[1]: string name ; \n \
 \n \
		[11]: int startType = 5; \n \
		[12]: bool needSwitch = false; \n \
		[13]: int switchID = 1; \n \
 \n \
		[21]: int eventLength; \n \
		[22]: Event event; \n \
	}; \n \
}; \n \
");
