#include <deque>

#include <SDL/SDL.h>

#include "Audio2D.hpp"
#include "Debug.hpp"
#include "Execute.hpp"
#include "Graphics2D.hpp"
#include "KeyListener.hpp"
#include "Main.hpp"
#include "Project.hpp"

#if RPG2K_IS_PSP
	/* PSP library */
	#include <pspdebug.h>
	#include <pspkernel.h>
	#include <psppower.h>
#endif


namespace rpg2k
{
	const double Main::MILLI_SEC_PER_LOOP = 1000.0 / FRAME_PER_LOOP;

	Main::Main(SystemString const& base)
	: gameDir_(base), rtpDir_(base)
	, gameModeChanged_(true)
	, hideTitle_(false), testPlay_(true), magnify_(false)
	, alwaysOnTop_(false), fullScreen_(false)
	{
	// init counter
		loopCount_ = delayCount_ = drawCount_ = 0;
		#ifdef PSP
			// over clock
			scePowerSetClockFrequency(333, 333, 166);
			cout << "OverClocked PSP to 333MHz." << endl;
		#endif
	// initialize SDL
		bool res = SDL_Init(SDL_INIT_EVERYTHING) == 0; rpg2k_assert(res);
		debug::addAtExitFunction(SDL_Quit);
	// config values
		config();
	// init Project
		project_.reset( new model::Project(gameDir_) );
		cout << "Project initialized." << endl;
	// set window title
		SDL_WM_SetCaption( project_->getGameTitle().toSystem().c_str(), NULL);
	// init Graphics2D
		graphics2d_.reset( new Graphics2D(*this) );
		cout << "Graphics2D initialized." << endl;
	// init Audio2D
		audio2d_.reset( new Audio2D(*this) );
		cout << "Audio2D initialized." << endl;
	// init KeyListener
		keyListener_.reset( new input::KeyListener(*this) );
		cout << "KeyListener initialized." << endl;
	// init Material
		material_.reset( new Material(*this) );
		cout << "Material initialized." << endl;
	// init GameModes
		#define PP_add2map(name) \
			gameMode_.addPointer( GameMode::name, structure::Map< GameMode::Type, GameMode >::Pointer( new gamemode::name(*this) ) ); \
			gameMode_[GameMode::name].reset();
		PP_allGameMode(PP_add2map)
		#undef PP_add2map
		cout << "All GameModes are initialized." << endl;
		gotoTitle();
	// init Execute
		execute_.reset( new model::Execute(*this) );
	}
	Main::~Main()
	{
		dispose();
	}

	void Main::dispose()
	{
		printResult();
	}

	void Main::gotoTitle()
	{
		resetGameMode(GameMode::Title);

		keyListener_->clear();
		while( !gameModeStack_.empty() ) gameModeStack_.pop();

		currentMode_ = GameMode::Title;
		gameModeChanged_ = true;

		getProject().newGame();
	}
	void Main::resetGameMode(GameMode::Type mode)
	{
		gameMode_[mode].reset();
	}
	void Main::callGameMode(GameMode::Type mode)
	{
		keyListener_->clear();
		gameModeStack_.push(currentMode_);
		currentMode_ = mode;
		gameModeChanged_ = true;
	}
	void Main::passToGameMode(GameMode::Type mode, int val)
	{
		gameMode_[mode].pass(val);
	}
	void Main::returnGameMode()
	{
		keyListener_->clear();
		currentMode_ = gameModeStack_.top();
		gameModeStack_.pop();
		gameModeChanged_ = true;
	}

	void Main::start()
	{
		mainLoop();
	}
	void Main::quit()
	{
		dispose();

		exit(EXIT_SUCCESS);
	}

	void Main::run()
	{
	// get the last key input
		getKeyListener().load();
	// run the process
		if(gameModeChanged_) {
			gameMode_[currentMode_].gameModeChanged();
			gameModeChanged_ = false;
		}

		gameMode_[currentMode_].run( getKeyListener() );

		if(gameModeChanged_) {
			gameMode_[currentMode_].gameModeChanged();
			gameModeChanged_ = false;
		}

		getKeyListener().clearBuffer();
	}

	void Main::mainLoop()
	{
		startTime_ = SDL_GetTicks();
		for(;;) {
			frameBegin_ = SDL_GetTicks();

			run();

			loopCount_++;

			requiredTime_ = SDL_GetTicks() - frameBegin_;

			if(requiredTime_ < MILLI_SEC_PER_LOOP) {
				drawCount_++;
				// graphics2d_->clear();
				gameMode_[currentMode_].draw(*graphics2d_);
				graphics2d_->swapBuffers();

				// check the time left
				requiredTime_ = SDL_GetTicks() - frameBegin_;
				if(requiredTime_ < MILLI_SEC_PER_LOOP) {
					SDL_Delay(MILLI_SEC_PER_LOOP-requiredTime_);
				} else {
					delayCount_++;
					// printResult();
				}
			} else {
				delayCount_++;
				// printResult();
			}
		}
	}

	void Main::config()
	{
		std::deque< RPG2kString > token;
		RPG2kString name, base, game;
		uint line = 1;
		enum { NAME, EQUALS, VAL, EXP_END, } preType = EXP_END;
		// to token
		std::ifstream inf( (gameDir_ + PATH_SEPR + SETTING_FILE).c_str() );
		model::DefineLoader::toToken(token, inf);
		// parse
		for(std::deque< RPG2kString >::const_iterator it = token.begin(); it != token.end(); ++it) {
			if(*it == "\n") { line++; continue; }
			else switch(preType) {
				case EXP_END: name = *it; preType = NAME; continue;
				case NAME:
					if(*it == "=") { preType = EQUALS; continue; }
					else break;
				case EQUALS:
					// strings
					if(name == "RTP") rtpDir_ += PATH_SEPR + it->substr( 1, it->length()-2 );
					else if(name == "GAME") game = it->substr( 1, it->length()-2 );
					else if(name == "BASE") base = it->substr( 1, it->length()-2 );
					// bools
					else {
						std::istringstream ss(*it);
						if(name == "HIDE_TITLE") ss >> std::boolalpha >> hideTitle_;
						else if(name == "TEST_PLAY") ss >> std::boolalpha >> testPlay_;
						else if(name == "MAGNIFY") ss >> std::boolalpha >> magnify_;
						else if(name == "ALWAYS_ON_TOP") ss >> std::boolalpha >> alwaysOnTop_;
						else if(name == "FULL_SCREEN") ss >> std::boolalpha >> fullScreen_;
						else break;
					}

					preType = VAL; continue;
				case VAL:
					if(*it == ";") { preType = EXP_END; continue; }
					else break;
				default:
					break;
			}

			clog << "Error when loading setting Stream at line: " << std::dec << line << ";";
			rpg2k_assert(false);
		}
		rpg2k_assert(preType == EXP_END);

		gameDir_ += PATH_SEPR + base + PATH_SEPR + game;
	}

	void Main::printResult(std::ostream& output) const
	{
	#if RPG2K_IS_PSP
		output << "MemoryMaxFree: "   << std::dec << sceKernelMaxFreeMemSize() << endl;
		output << "MemoryTotalFree: " << std::dec << sceKernelTotalFreeMemSize() << endl;
	#endif
		output << "MainLoop:" << endl
			<< "\tdelay count: " << std::dec << delayCount_ << "/ " << loopCount_ << endl
			<< "\tfrom mainloop start: " << (SDL_GetTicks() - startTime_) << "ms" << endl
				<< "\t\tlast loop required time: " << requiredTime_ << "ms" << endl
			<< "\taverage time: " << (SDL_GetTicks() - startTime_)/loopCount_ << "ms" << endl
			<< "\taverage fps: " << ( (double)drawCount_/((double)loopCount_/60) ) << endl;
	}
} // namespace rpg2k
