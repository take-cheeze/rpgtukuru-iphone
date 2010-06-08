/**
 * @file
 * @brief Battle Test
 * @author project.kuto
 */

#include "test_battle.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>
#include "game_battle.h"
#include "game_chara_status.h"


TestBattle::TestBattle(kuto::Task* parent)
: kuto::Task(parent)
, gameSystem_("/User/Media/Photos/RPG2000/yoake")
{
	kuto::VirtualPad::instance()->pauseDraw(false);
	int terrainId = rand() % (gameSystem_.getRpgLdb().saTerrain.GetSize() - 1) + 1;
	int enemyGroupId = rand() % (gameSystem_.getRpgLdb().saEnemyGroup.GetSize() - 1) + 1;
	gameBattle_ = GameBattle::createTask(this, gameSystem_, gameSystem_.getRpgLdb().saTerrain[terrainId].battleGraphic, enemyGroupId);
	for (unsigned int playerId = 1; playerId < 4; playerId++) {
		GameCharaStatus status;
		CRpgLdb::Status itemUp = {0,0,0,0,0,0};
		CRpgLdb::Equip equip;
		std::vector<uint16_t> weapons;
		std::vector<uint16_t> armours;
		for (unsigned int i = 1; i < gameSystem_.getRpgLdb().saItem.GetSize(); i++) {
			switch (gameSystem_.getRpgLdb().saItem[i].type) {
			case CRpgLdb::kItemTypeWeapon:
				weapons.push_back(i);
				break;
			case CRpgLdb::kItemTypeShield:
			case CRpgLdb::kItemTypeProtector:
			case CRpgLdb::kItemTypeHelmet:
			case CRpgLdb::kItemTypeAccessory:
				armours.push_back(i);
				break;
			}
		}
		equip.weapon = weapons[kuto::random((int)weapons.size())];
		equip.shield = armours[kuto::random((int)armours.size())];
		equip.protector = armours[kuto::random((int)armours.size())];
		equip.helmet = armours[kuto::random((int)armours.size())];
		equip.accessory = armours[kuto::random((int)armours.size())];
		status.setPlayerStatus(gameSystem_.getRpgLdb(), playerId, 20, itemUp, equip);
		
		gameBattle_->addPlayer(playerId, status);
	}
}

bool TestBattle::initialize()
{
	return isInitializedChildren();
}

void TestBattle::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (gameBattle_->getState() == GameBattle::kStateEnd
	|| virtualPad->repeat(kuto::VirtualPad::KEY_START)) {
		this->release();			
	}
}

void TestBattle::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void TestBattle::render()
{
}


