#ifndef _INC__RPG2k__GAMEMODE_SAVE
#define _INC__RPG2k__GAMEMODE_SAVE

// #include <GameMode.hpp>


namespace rpg2k
{
	namespace gamemode
	{
		class SaveManager : public GameMode
		{
		public:
			enum { SAVE, LOAD, };
		private:
			bool offsetChanged_;

			Size2D cursorSize_, previewSize_, messageSize_;
			Vector2D previewCoord_, messageCoord_;
			static uint const PREVIEW_NUM = 3;

			RPG2kString level_, hp_, prefix_, message_;

			int mode_;
		public:
			SaveManager(rpg2k::Main& m);
			virtual ~SaveManager();

			void reset();
			void run(input::KeyListener& keyList);
			void gameModeChanged();
			void draw(Graphics2D& g);
			void pass(int val);
		}; // class SaveManager
	} // namespace gamemode
} // namespace rpg2k

#endif
