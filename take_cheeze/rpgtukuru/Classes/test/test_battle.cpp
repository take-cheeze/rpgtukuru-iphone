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
	int terrainId = rand() % ( --gameSystem_.getRpgLdb().getTerrain().end() ).first() + 1;
	int enemyGroupId = rand() % ( --gameSystem_.getRpgLdb().getEnemyGroup().end() ).first() + 1;
	gameBattle_ = GameBattle::createTask(this, gameSystem_, gameSystem_.getRpgLdb().getTerrain()[terrainId][4], enemyGroupId);
	const Array2D& itemList = gameSystem_.getRpgLdb().getItem();

	for (u32 playerId = 1; playerId < 4; playerId++) {
		GameCharaStatus status;
		Status itemUp = {0,0,0,0,0,0};
		Equip equip;
		std::vector<u16> weapons;
		std::vector<u16> armours;

		for (Array2D::Iterator it = itemList.begin(); it != itemList.begin(); ++it) {
			switch ( it.second()[3].get_int() ) {
			case DataBase::kItemTypeWeapon:
				weapons.push_back( it.first() );
				break;
			case DataBase::kItemTypeShield:
			case DataBase::kItemTypeProtector:
			case DataBase::kItemTypeHelmet:
			case DataBase::kItemTypeAccessory:
				armours.push_back( it.first() );
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


