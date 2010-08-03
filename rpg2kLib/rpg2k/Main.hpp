#ifndef _INC__RPG2K__MAIN_HPP
#define _INC__RPG2K__MAIN_HPP

#include <memory>
#include <stack>

#include "GameMode.hpp"


namespace rpg2k
{
	class Audio2D;
	class Graphics2D;
	class Material;

	namespace input
	{
		class KeyListener;
	} // namespace input
	namespace model
	{
		class Project;
		class Execute;
	} // namespace model

	class Main
	{
	private:
		std::auto_ptr< Audio2D > audio2d_;
		std::auto_ptr< model::Project > project_;
		std::auto_ptr< Graphics2D > graphics2d_;
		std::auto_ptr< input::KeyListener > keyListener_;
		std::auto_ptr< Material > material_;
		std::auto_ptr< model::Execute > execute_;
		structure::Map< GameMode::Type, GameMode > gameMode_;

		SystemString gameDir_, rtpDir_;

		GameMode::Type currentMode_;
		bool gameModeChanged_;

		bool hideTitle_, testPlay_, magnify_, alwaysOnTop_, fullScreen_;

		static const double MILLI_SEC_PER_LOOP;
		uint64_t
			startTime_, frameBegin_, requiredTime_,
			loopCount_, delayCount_, drawCount_   ;

		std::stack< GameMode::Type > gameModeStack_;

		 /*
		  * todos
		  * - if true drawMoneyWindow()
		  * - at closeMessageWindow() set this false
		  */
		bool isMoneyWindowVisible_;
	protected:
		void mainLoop();
		void run();
		void refreshScreen();

		void dispose();

		void config();
	public:
		Main(SystemString const& base);
		~Main();

		void messageWindowOpened();
		void closeMessageWindow();
		// returns false if mssage window was already opened
		bool openMessageWindow();

		void start();
		void quit();

		SystemString const& getGameDir() const { return gameDir_; }
		SystemString const& getRTPDir () const { return  rtpDir_;  }

		model::Project& getProject() const { return *project_; }
		Graphics2D& getGraphics2D() const { return *graphics2d_; }
		Audio2D& getAudio2D() const { return *audio2d_; }
		input::KeyListener& getKeyListener() const { return *keyListener_; }
		Material& getMaterial() const { return *material_; }
		model::Execute& getExecute() const { return *execute_; }

		void gotoTitle();

		void resetGameMode(GameMode::Type mode);

		void callGameMode(GameMode::Type mode);
		void returnGameMode();

		GameMode::Type currentGameMode() const { return currentMode_; }
		GameMode::Type previousGameMode() const { return gameModeStack_.top(); }

		void passToGameMode(GameMode::Type mode, int val);

		void printResult(std::ostream& output = std::cout) const;

		uint64_t loopCount() const { return loopCount_; }

		bool hideTitle() const { return hideTitle_; }
		bool isTestPlay() const { return testPlay_; }
		bool magnify() const { return magnify_; }
		bool isAlwaysOnTop() const { return alwaysOnTop_; }
		bool isFullScreen() const { return fullScreen_; }
	}; // class Main
} // namespace rpg2k

#endif // _INC__RPG2K__MAIN_HPP
