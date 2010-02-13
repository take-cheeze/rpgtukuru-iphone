#ifndef _INC__RPG2K__MODEL__EXECUTE_HPP
#define _INC__RPG2K__MODEL__EXECUTE_HPP

#include "Project.hpp"

namespace rpg2kLib
{
	namespace model
	{
		enum ExecuteState
		{
			EXE_SUCCESS = 0, EXE_FAILED, EXE_NOT_IMPLEMENTED,
			EXE_NEXT_FRAME,
		};

		class Execute
		{
		private:
			Project& PROJECT;

			enum EventType { EV_END = 0, EV_COMMON, EV_MAP, EV_BATTLE, };
			struct ExecInfo
			{
				EventType type;
				uint id;

				uint ip;
				bool end;

				EventStart start;
				uint page;
				Event* event;
			} CUR_EXEC;

			vector< map< uint, ExecInfo > > EXEC_INFO;
			map< uint, uint > PAGE_MAP;

			vector< map< uint, multimap< uint, uint > > >* EVENT_MAP;
			map< uint, uint >* PAGE_NO;
		protected:
			Project& getProject() { return PROJECT; }

			void skipToEndOfJunction(uint eventCode);
			void skipToElse(uint eventCode);

			void nextEvent(ExecInfo& curState);

			bool isJunction(const Instruction& inst);

/*
			enum BattleState { ATTACK, SKILL, BLOCK, ITEM, };

			struct Parameter
			{
				vector< uint8_t  > attribute;
				vector< uint16_t > weapon;
				uint criticalRate;
				uint HP, MP;
				bool strongDefencer;
				float attack, defence, mind, speed;

				BattleState state;
			};
			void getDamage(const Parameter& attacker, Prameter& blocker)
			{
				float basic = (attacker.attack/2) - (blocker.block/4);
				float result = basic;
			// check for critical
				if( random(attacker.criticalRate)==0 ) result *= 3.0;
			// check for attribute
				
			// decrease damage if blocker's state is set to BLOCK
				if(blocker.state == BLOCK) {
				// decrease more if blocker is strong defencer
					if(blocker.strongDefencer) result /= 4;
					else result /= 2;
				}
				
			}
			
 */
		public:
			Execute(Project& p) : PROJECT(p) {}

			const Instruction& currentInst();

			void refresh(
				vector< map< uint, multimap< uint, uint > > >& eventMap,
				map< uint, uint >& pageNo
			);

#if defined USE_EVENT_COMMAND

#define PP_codeProto(codeNo) ExecuteState code##codeNo(const Instruction& inst)

			// end of event
			PP_codeProto(0);
			// end of junction
			PP_codeProto(10);

			// show message
			PP_codeProto(10110);
			PP_codeProto(20110);
			// set message config
			PP_codeProto(10120);
			// set message face set
			PP_codeProto(10130);
			// select junction
			PP_codeProto(10140);
			PP_codeProto(20140);
			PP_codeProto(20141);
			// input number
			PP_codeProto(10150);

			// change switch
			PP_codeProto(10210);
			// change variable
			PP_codeProto(10220);
			// set timer
			PP_codeProto(10230);

			// set money
			PP_codeProto(10310);
			// set item
			PP_codeProto(10320);
			// exchange member
			PP_codeProto(10330);

			// change exp value
			PP_codeProto(10410);
			// change level
			PP_codeProto(10420);
			// change character parameter
			PP_codeProto(10430);
			// add or remove skill
			PP_codeProto(10440);
			// change equipment
			PP_codeProto(10450);
			// change HP
			PP_codeProto(10460);
			// change MP
			PP_codeProto(10470);
			// set condition
			PP_codeProto(10480);
			// full cure
			PP_codeProto(10490);
			// damage character
			PP_codeProto(10500);

			// change character name
			PP_codeProto(10610);
			// change character title
			PP_codeProto(10620);
			// change character graphic
			PP_codeProto(10630);
			// change face graphic
			PP_codeProto(10640);
			// change char graphic
			PP_codeProto(10650);
			// change system BGM
			PP_codeProto(10660);
			// change system SE
			PP_codeProto(10670);
			// change system graphic
			PP_codeProto(10680);
			// change screen refresh
			PP_codeProto(10690);

			// battle junction
			PP_codeProto(10710);
			PP_codeProto(20710);
			PP_codeProto(20711);
			PP_codeProto(20712);
			PP_codeProto(20713);
			// shop junction
			PP_codeProto(10720);
			PP_codeProto(20720);
			PP_codeProto(20721);
			PP_codeProto(20722);
			// hotel junction
			PP_codeProto(10730);
			PP_codeProto(20730);
			PP_codeProto(20731);
			PP_codeProto(20732);
			// input character name
			PP_codeProto(10740);

			// move to a place
			PP_codeProto(10810);
			// memorize current point
			PP_codeProto(10820);
			// move to the memorized point
			PP_codeProto(10830);
			// get on to the transport
			PP_codeProto(10840);
			// set transport position
			PP_codeProto(10850);
			// set event position
			PP_codeProto(10860);
			// exchange event position
			PP_codeProto(10870);

			// get terrain id
			PP_codeProto(10910);
			// get event id
			PP_codeProto(10920);

			// erase screen
			PP_codeProto(11010);
			// show screen
			PP_codeProto(11020);
			// set screen color tone
			PP_codeProto(11030);
			// flash screen
			PP_codeProto(11040);
			// shake screen
			PP_codeProto(11050);
			// scroll screen
			PP_codeProto(11060);
			// set weather effect
			PP_codeProto(11070);

			// show picture
			PP_codeProto(11110);
			// move picture
			PP_codeProto(11120);
			// erase picture
			PP_codeProto(11130);

			// battle anime at field
			PP_codeProto(11210);

			// transparent hero
			PP_codeProto(11310);
			// flash event
			PP_codeProto(11320);
			// set move
			PP_codeProto(11330);
			// process all moves
			PP_codeProto(11340);
			// dispose all moves
			PP_codeProto(11350);

			// wait
			PP_codeProto(11410);

			// play music
			PP_codeProto(11510);
			// fade out music
			PP_codeProto(11520);
			// memorize music
			PP_codeProto(11530);
			// play memorized music
			PP_codeProto(11540);
			// play sound
			PP_codeProto(11550);
			// play movie
			PP_codeProto(11560);

			// get key input
			PP_codeProto(11610);

			// change chip set
			PP_codeProto(11710);
			// change panorama
			PP_codeProto(11720);
			// change encount step
			PP_codeProto(11740);
			// replace chip
			PP_codeProto(11750);

			// add teleport link
			PP_codeProto(11810);
			// disable or enable teleport
			PP_codeProto(11820);
			// set escape link
			PP_codeProto(11830);
			// disable or enable escape
			PP_codeProto(11840);
			// call save screen

			PP_codeProto(11910);
			// disable or enable saving
			PP_codeProto(11930);
			// call menu
			PP_codeProto(11950);
			// disable or enable menu
			PP_codeProto(11960);

			// junction
			PP_codeProto(12010);
			PP_codeProto(22010);
			PP_codeProto(22011);

			// set label
			PP_codeProto(12110);
			// goto label
			PP_codeProto(12120);

			// loop
			PP_codeProto(12210);
			PP_codeProto(22210);
			// interrupt loop
			PP_codeProto(12220);

			// interrupt event
			PP_codeProto(12310);
			// erase event
			PP_codeProto(12320);
			// call event
			PP_codeProto(12330);

			// comment
			PP_codeProto(12410);
			PP_codeProto(22410);
			// gameover
			PP_codeProto(12420);

			// title
			PP_codeProto(12510);

		// battle events
			// change enemy HP
			PP_codeProto(13110);
			// change enemy MP
			PP_codeProto(13120);
			// change enemy condition
			PP_codeProto(13130);
			// appear enemy
			PP_codeProto(13150);

			// change backdrop
			PP_codeProto(13210);
			// show battle animation
			PP_codeProto(13260);

			// junction true  way
			PP_codeProto(13310);
			// junction false way
			PP_codeProto(23310);
			// end of junction
			PP_codeProto(23311);

			// interrupt battle
			PP_codeProto(13410);

#undef PP_codeProto

#endif

		};

	};
};

#endif
