/**
 * @file
 * @brief Game Main
 * @author project.kuto
 */

#include <kuto/kuto_graphics_device.h>
#include <kuto/kuto_memory.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>

#include "game.h"
#include "game_field.h"
#include "game_over.h"
#include "game_title.h"


Game::Game(const GameConfig& config)
: gameSystem_(config.projectName())
, texPool_(gameSystem_)
, config_(config)
, gameField_(NULL), gameTitle_(NULL), gameOver_(NULL)
{
	kuto::VirtualPad::instance()->pauseDraw(false);

	gameTitle_ = addChild(GameTitle::createTask(gameSystem_));

	kuto::GraphicsDevice::instance()->setTitle( gameSystem_.gameTitle().toSystem() );
}

bool Game::initialize()
{
	return isInitializedChildren();
}

void Game::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (virtualPad->repeat(kuto::VirtualPad::KEY_START)) {
		// return debug menu
		if (gameTitle_) {
			this->release();
		} else {
			returnTitle();
		}
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_Y)) {
		kuto::Memory::instance()->print();
	}

	if (gameTitle_) {
		// Title
		if (gameTitle_->getSelectMenu() != GameTitle::kSelectNone) {
			switch (gameTitle_->getSelectMenu()) {
			case GameTitle::kSelectNewGame:
				gameSystem_.newGame();
				gameField_ = GameField::createTask(this, gameSystem_, 0);
				break;
			case GameTitle::kSelectContinue:
				gameSystem_.newGame();
				gameField_ = GameField::createTask(this, gameSystem_, gameTitle_->getSaveId());
				break;
			case GameTitle::kSelectShutDown:
				this->release();
				break;
			default: kuto_assert(false);
			}
			gameTitle_->release();
			gameTitle_ = NULL;
		}
	}
}

void Game::gameOver()
{
	kuto_assert(!gameOver_);
	gameOver_ = addChild(GameOver::createTask(this, gameSystem_));
	if (gameField_)
		gameField_->release();
	gameField_ = NULL;
}

void Game::returnTitle()
{
	kuto_assert(!gameTitle_);
	gameTitle_ = addChild(GameTitle::createTask(gameSystem_));
	if (gameField_)
		gameField_->release();
	gameField_ = NULL;
	if (gameOver_)
		gameOver_->release();
	gameOver_ = NULL;
}
