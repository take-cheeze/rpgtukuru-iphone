#ifndef _INC__RPG2k__GAMEMODE_TITLE
#define _INC__RPG2k__GAMEMODE_TITLE

// #include <rpg2k/GameMode.hpp>


namespace rpg2k
{
	namespace gamemode
	{
		class Title : public GameMode
		{
		private:
			Size2D menuSize_, cursorSize_;
			Vector2D menuCoord_, cursorCoord_;

			static uint const MENU_H = 64;

			enum { TO_NEW_GAME = 0, TO_LOAD, TO_QUIT, COMMAND_END };
			std::vector< RPG2kString > command_;
			bool noContinue_;
		public:
			Title(Main& m);
			virtual ~Title();
			virtual void reset();
			virtual void gameModeChanged();
			virtual void run(input::KeyListener& keyList);
			virtual void draw(Graphics2D& g);
			virtual void pass(int val);
		};
	} // namespace gamemode
} // namespace rpg2k

#endif
