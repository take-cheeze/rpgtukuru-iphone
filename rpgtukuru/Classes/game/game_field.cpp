/**
 * @file
 * @brief Game Field
 * @author project.kuto
 */

#include <kuto/kuto_file.h>

#include "game.h"
#include "game_battle.h"
#include "game_chara_status.h"
#include "game_debug_menu.h"
#include "game_event_manager.h"
#include "game_fade_effect.h"
#include "game_field.h"
#include "game_map.h"
#include "game_picture_manager.h"
#include "game_system_menu.h"


GameField::GameField(Game& parent, unsigned const saveId)
: kuto::Task()
, game_(parent), project_( parent.project() )
, map_( *addChildFront(GameMap::createTask(*this)) )
, eventManager_( *addChild(GameEventManager::createTask(*this)) )
, battle_(NULL)
, fadeEffect_( *addChild(GameFadeEffect::createTask()) )
, fadeEffectScreen_( *addChild(GameFadeEffect::createTask()) )
, state_(kStateField)
, systemMenu_( *addChild(GameSystemMenu::createTask(*this)) )
, pictManager_( *addChild( GamePictureManager::createTask(*this) ) )
, debugMenu_( *addChild(GameDebugMenu::createTask(*this)) )
{
	if (
		rpg2k::ID_MIN <= saveId &&
		saveId <= rpg2k::SAVE_DATA_MAX &&
		project_.getLSD(saveId).exists()
	) { project_.loadLSD(saveId); }
	else { project_.newGame(); }

	rpg2k::structure::EventState& party = project_.getLSD().party();
	kuto::Point2 const playerPos(party.x(), party.y());
	// std::vector<uint16_t> const playerIds = project_.getLSD().member();
	// int const playerDir = rpg2k::EventDir::DOWN;

	changeMap( party.mapID(), party.x(), party.y() );

	GameCharaStatus status;
	/*
	dummyLeader_ = addChild(kuto::TaskCreatorParam3<GamePlayer, GameField&, int, GameCharaStatus&>::createTask(*this, 1, status)); // project_.playerStatus(1));
	dummyLeader_->pauseUpdate();
	dummyLeader_->setPosition(playerPos);
	dummyLeader_->setDirection(rpg2k::EventDir::Type(playerDir));
	 */

	fadeInfos_[kFadePlaceMapHide] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceMapShow] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleStartHide] = GameFadeEffect::kTypeStripeVertical;
	fadeInfos_[kFadePlaceBattleStartShow] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleEndHide] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleEndShow] = GameFadeEffect::kTypeFade;

	/*
	for (uint i = 0; i < playerIds.size(); i++) {
		addPlayer(playerIds[i]);
	}
	 */
}


bool GameField::initialize()
{
	return isInitializedChildren();
}

void GameField::update()
{
	switch (state_) {
	case kStateField:
		break;
	case kStateBattleStart:
		if (fadeEffect_.state() == GameFadeEffect::kStateFadeOutEnd) {
			state_ = kStateBattle;
			battle_->freeze(false);
			fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleStartShow], GameFadeEffect::kStateFadeIn);
			map_.freeze();
			eventManager_.freeze();
			/*
			if (!players_.empty())
				players_[0]->freeze();
			 */
		}
		break;
	case kStateBattle:
		if (battle_->state() == GameBattle::kStateEnd) {
			state_ = kStateBattleEnd;
			battle_->pauseUpdate();
			fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleEndHide], GameFadeEffect::kStateFadeOut);
		}
		break;
	case kStateBattleEnd:
		if (fadeEffect_.state() == GameFadeEffect::kStateFadeOutEnd) {
			endBattle();
		}
		break;
	case kStateChangeMap:
		if (fadeEffect_.state() == GameFadeEffect::kStateFadeOutEnd) {
			/*
			if (!players_.empty()) {
				players_[0]->pauseUpdate(false);
			}
			rpg2k::structure::EventState& ev = project_.getLSD().party();
			playerLeader()->setPosition(kuto::Point2(ev.x(), ev.y()));
			if (ev.eventDir() > 0)
				playerLeader()->setDirection(ev.eventDir());
			playerLeader()->updateMapPosition();
			 */

			map_.pauseUpdate(false);
			eventManager_.pauseUpdate(false);
			fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceMapShow], GameFadeEffect::kStateFadeIn);
			state_ = kStateField;
		}
		break;
	case kStateSystemMenuStart:
		if (fadeEffect_.state() == GameFadeEffect::kStateFadeOutEnd) {
			state_ = kStateSystemMenu;
			systemMenu_.start();
			fadeEffect_.start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeIn);
			map_.freeze();
			eventManager_.freeze();
			/*
			if (!players_.empty())
				players_[0]->freeze();
			 */
		}
		break;
	case kStateSystemMenu:
		if (systemMenu_.isEnd()) {
			state_ = kStateSystemMenuEnd;
			systemMenu_.pauseUpdate();
			fadeEffect_.start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeOut);
		}
		break;
	case kStateSystemMenuEnd:
		if (fadeEffect_.state() == GameFadeEffect::kStateFadeOutEnd) {
			endSystemMenu();
		}
		break;
	}
}

/*
GamePlayer* GameField::playerFromId(int playerId)
{
	for (uint i = 0; i < players_.size(); i++) {
		if (players_[i]->playerID() == playerId)
			return players_[i];
	}
	return NULL;
}

void GameField::addPlayer(int playerId)
{
	if (!playerFromId(playerId)) {
		GameCharaStatus status;
		GamePlayer* player = addChild(kuto::TaskCreatorParam3<GamePlayer, GameField&, int, GameCharaStatus&>::createTask(*this, playerId, status)); // project_.playerStatus(playerId));
		players_.push_back(player);
		if (players_.size() == 1) {
			players_[0]->setPosition(dummyLeader_->position());
			players_[0]->setDirection(dummyLeader_->direction());
		} else {
			player->pauseUpdate();
		}
	}
}
 */

/*
void GameField::removePlayer(int playerId)
{
	for (uint i = 0; i < players_.size(); i++) {
		if (players_[i]->playerID() == playerId) {
			players_.erase(players_.begin() + i);
			if (i == 0 && !players_.empty()) {
				players_[0]->pauseUpdate(false);
			}
			break;
		}
	}
}
 */

void GameField::startBattle(const std::string& terrain, int enemyGroupId, bool firstAttack, bool enableEscape, bool loseGameOver)
{
	state_ = kStateBattleStart;
	battleLoseGameOver_ = loseGameOver;
	battle_ = addChild(GameBattle::createTask(*this, terrain, enemyGroupId));
	battle_->setFirstAttack(firstAttack);
	battle_->enableEscape(enableEscape);
	std::vector<uint16_t> const& mem = project_.getLSD().member();
	for (uint i = 0; i < mem.size(); i++) { battle_->addPlayer(mem[i]); }
	battle_->freeze();
	fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleStartHide], GameFadeEffect::kStateEncountFlash);
	map_.pauseUpdate();
	eventManager_.pauseUpdate();
}

void GameField::endBattle()
{
	state_ = kStateField;
	battleResult_ = battle_->result();
	battle_->release();
	battle_ = NULL;
	fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleEndShow], GameFadeEffect::kStateFadeIn);
	if (battleLoseGameOver_ && battleResult_ == GameBattle::kResultLose) {
		game_.gameOver();
	} else {
		map_.pauseUpdate(false);
		map_.freeze(false);
		eventManager_.pauseUpdate(false);
		eventManager_.freeze(false);
		/*
		if (!players_.empty()) {
			players_[0]->pauseUpdate(false);
			players_[0]->freeze(false);
		}
		 */
	}
}

void GameField::changeMap(int mapId, int x, int y)
{
	state_ = kStateChangeMap;
	fadeEffect_.start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceMapHide], GameFadeEffect::kStateFadeOut);
	map_.pauseUpdate();
	eventManager_.pauseUpdate();
	/*
	if (!players_.empty()) {
		players_[0]->pauseUpdate();
	}
	 */

	project_.move(mapId, x, y);
	map_.moveMap(mapId, x, y);
}

void GameField::fadeOut(int type)
{
	fadeEffectScreen_.start((GameFadeEffect::FadeType)(type == -1? fadeInfos_[kFadePlaceMapHide] : type), GameFadeEffect::kStateFadeOut);
}

void GameField::fadeIn(int type)
{
	fadeEffectScreen_.start((GameFadeEffect::FadeType)(type == -1? fadeInfos_[kFadePlaceMapShow] : type), GameFadeEffect::kStateFadeIn);
}

void GameField::startSystemMenu()
{
	state_ = kStateSystemMenuStart;
	fadeEffect_.start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeOut);
	map_.pauseUpdate();
	eventManager_.pauseUpdate();
	/*
	if (!players_.empty())
		players_[0]->pauseUpdate();
	 */
}

void GameField::endSystemMenu()
{
	state_ = kStateField;
	systemMenu_.freeze();
	systemMenu_.pauseUpdate(false);
	fadeEffect_.start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeIn);
	map_.pauseUpdate(false);
	map_.freeze(false);
	eventManager_.pauseUpdate(false);
	eventManager_.freeze(false);
	/*
	if (!players_.empty()) {
		players_[0]->pauseUpdate(false);
		players_[0]->freeze(false);
	}
	 */
}
