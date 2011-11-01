#ifndef _INC__RPG2k__GAMEMODE_FIELD
#define _INC__RPG2k__GAMEMODE_FIELD

#include <stack>

// #include <GameMode.hpp>
// #include <Execute.hpp>


namespace rpg2k
{
	namespace gamemode
	{
		class Field : public GameMode
		{
		private:
			int charMoveCount;

			bool allEventExecuted_;

			std::stack< uint > touchFromParty_, touchFromEvent_;

			std::vector< std::map< uint, std::multimap< uint, uint > > > eventMap_;
			std::map< uint, uint > pageNum_;
		protected:
			bool canPassMap(EventDir::Type dir, int curX, int curY, int nxtX, int nxtY);
			void drawPictures(Graphics2D& g);
			void drawBattles(Graphics2D& g);
		public:
			Field(Main& m);

			virtual void reset();
			virtual void run(input::KeyListener& keyList);
			virtual void draw(Graphics2D& g);
			virtual void gameModeChanged();

		// returns true when move has occured
			bool judgeMove(uint eventID, EventDir::Type dir);

			std::stack< uint > const& touchFromParty() const { return touchFromParty_; }
			std::stack< uint > const& touchFromEvent() const { return touchFromEvent_; }
		};

		PP_defineGameMode(Shop);
		PP_defineGameMode(NameInput);
		PP_defineGameMode(Hotel);
	} // namespace gamemode
} // namespace rpg2k

#endif
