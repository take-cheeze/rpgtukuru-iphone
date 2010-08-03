/**
 * @file
 * @brief Game Main
 * @author project.kuto
 */

#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_memory.h>
#include "game.h"
#include "game_field.h"
#include "game_title.h"
#include "game_over.h"


Game* Game::instance_ = NULL;

Game::Game(kuto::Task* parent, const Option& option)
: kuto::Task(parent)
, gameSystem_(option.projectName.c_str())
, gameField_(NULL), gameTitle_(NULL), gameOver_(NULL)
{
	kuto::VirtualPad::instance()->pauseDraw(false);

	gameTitle_ = GameTitle::createTask(this, gameSystem_);
}

Game::~Game()
{
	instance_ = NULL;
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
				gameSystem_.resetLSD();
				gameField_ = GameField::createTask(this, gameSystem_, 0);
				break;
			case GameTitle::kSelectContinue:
				gameSystem_.resetLSD();
				gameField_ = GameField::createTask(this, gameSystem_, gameTitle_->getSaveId());
				break;
			case GameTitle::kSelectShutDown:
				this->release();
				break;
			default: break;
			}
			gameTitle_->release();
			gameTitle_ = NULL;
		}
	}
}

void Game::gameOver()
{
	kuto_assert(!gameOver_);
	gameOver_ = GameOver::createTask(this, gameSystem_);
	if (gameField_)
		gameField_->release();
	gameField_ = NULL;
}

void Game::returnTitle()
{
	kuto_assert(!gameTitle_);
	gameTitle_ = GameTitle::createTask(this, gameSystem_);
	if (gameField_)
		gameField_->release();
	gameField_ = NULL;
	if (gameOver_)
		gameOver_->release();
	gameOver_ = NULL;
}


