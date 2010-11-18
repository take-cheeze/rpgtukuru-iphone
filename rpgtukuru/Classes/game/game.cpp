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
: project_(config.projectName())
, texPool_(project_)
, audioBufferPool_(project_)
, config_(config)
, field_(NULL), title_(NULL), gameOver_(NULL)
{
	kuto::VirtualPad::instance().pauseDraw(false);

	title_ = addChild(GameTitle::createTask(*this));

	// kuto::GraphicsDevice::instance().setTitle( project_.gameTitle().toSystem() );
}

bool Game::initialize()
{
	return isInitializedChildren();
}

void Game::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	if (virtualPad.repeat(kuto::VirtualPad::KEY_START)) {
		// return debug menu
		if (title_) {
			this->release();
		} else {
			returnTitle();
		}
	}
	if (virtualPad.repeat(kuto::VirtualPad::KEY_Y)) {
		kuto::Memory::instance().print();
	}

	if (title_) {
		// Title
		if (title_->selectMenu() != GameTitle::kSelectNone) {
			switch (title_->selectMenu()) {
			case GameTitle::kSelectNewGame:
				project_.newGame();
				field_ = addChild( GameField::createTask(*this, 0) );
				break;
			case GameTitle::kSelectContinue:
				project_.newGame();
				field_ = addChild( GameField::createTask(*this, title_->saveID()) );
				break;
			case GameTitle::kSelectShutDown:
				this->release();
				break;
			default: kuto_assert(false);
			}
			title_->release();
			title_ = NULL;
		}
	}
}

void Game::gameOver()
{
	if( !field_->game().config().noGameOver ) return;

	kuto_assert(!gameOver_);
	gameOver_ = addChild(GameOver::createTask(*this));
	if (field_)
		field_->release();
	field_ = NULL;
}

void Game::returnTitle()
{
	kuto_assert(!title_);
	title_ = addChild(GameTitle::createTask(*this));
	if (field_)
		field_->release();
	field_ = NULL;
	if (gameOver_)
		gameOver_->release();
	gameOver_ = NULL;
}

kuto::Texture& Game::systemTexture()
{
	return texPool_.get( GameTexturePool::System,
		project_.systemGraphic().toSystem() );
}
