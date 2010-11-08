/**
 * @file
 * @brief Battle Test
 * @author project.kuto
 */

#include <vector>

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>

#include "game_battle.h"
#include "game_chara_status.h"
#include "test_battle.h"


TestBattle::TestBattle()
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 0.f)
, gameSystem_(GAME_DATA_PATH)
{
	kuto::VirtualPad::instance().pauseDraw(false);
	int terrainId = kuto::random( (gameSystem_.getLDB().terrain().rend()->first) + 1 );
	int enemyGroupId = kuto::random( (gameSystem_.getLDB().enemyGroup().rend()->first) + 1 );
	gameBattle_ = NULL;
	// addChild(GameBattle::createTask(gameSystem_, gameSystem_.getLDB().terrain()[terrainId][4].to_string().toSystem(), enemyGroupId));
	for (uint playerId = 1; playerId < 4; playerId++) {
		std::vector<uint16_t> itemUp(6, 0);
		std::vector<uint16_t> equip(5, 0);
		std::vector<uint16_t> weapons;
		std::vector<uint16_t> armours;
		for (uint i = 1; i <= gameSystem_.getLDB().item().rend()->first; i++) {
			switch (gameSystem_.getLDB().item()[i][3].to<int>()) {
			case rpg2k::Item::WEAPON:
				weapons.push_back(i);
				break;
			case rpg2k::Item::SHIELD:
			case rpg2k::Item::ARMOR:
			case rpg2k::Item::HELMET:
			case rpg2k::Item::ACCESSORY:
				armours.push_back(i);
				break;
			}
		}
		equip[rpg2k::Equip::WEAPON] = weapons[kuto::random(weapons.size())];
		equip[rpg2k::Equip::SHIELD] = armours[kuto::random(armours.size())];
		equip[rpg2k::Equip::ARMOR] = armours[kuto::random(armours.size())];
		equip[rpg2k::Equip::HELMET] = armours[kuto::random(armours.size())];
		equip[rpg2k::Equip::OTHER] = armours[kuto::random(armours.size())];

		gameBattle_->addPlayer(playerId);
	}
}

bool TestBattle::initialize()
{
	return isInitializedChildren();
}

void TestBattle::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	if (gameBattle_->state() == GameBattle::kStateEnd
	|| virtualPad.repeat(kuto::VirtualPad::KEY_START)) {
		this->release();
	}
}

void TestBattle::render(kuto::Graphics2D& g) const
{
}
