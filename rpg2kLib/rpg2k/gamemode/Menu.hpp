#ifndef _INC__RPG2k__GAMEMODE_MENU_HPP
#define _INC__RPG2k__GAMEMODE_MENU_HPP

// #include <GameMode.hpp>


namespace rpg2k
{
	namespace gamemode
	{
		class Menu : public GameMode
		{
		private:
			std::vector< RPG2kString > command_;

			RPG2kString quitMessage_, yes_, no_;
			uint quitMessageW_, yesNoW_;

			enum { MENU_ITEM = 0, MENU_SKILL, MENU_EQUIP, MENU_SAVE, MENU_QUIT };
		protected:
			void drawMainMenu(Graphics2D& g, bool charSelect = false);
		public:
			Menu(rpg2k::Main& m);

			virtual void reset();
			virtual void run(input::KeyListener& keyList);
			virtual void gameModeChanged();
			virtual void draw(Graphics2D& g);
			virtual void pass(int val);
		}; // class Menu

		class Item : public GameMode
		{
		public:
			Item(rpg2k::Main& m);

			virtual void reset();
			virtual void run(input::KeyListener& keyList);
			virtual void gameModeChanged();
			virtual void draw(Graphics2D& g);
			virtual void pass(int val);
		}; // class Item

		class Equip : public GameMode
		{
		private:
			std::vector< uint > idList_;
		private:
			std::vector< RPG2kString > equip_, status_;
		public:
			Equip(rpg2k::Main& m);

			virtual void reset();
			virtual void run(input::KeyListener& keyList);
			virtual void gameModeChanged();
			virtual void draw(Graphics2D& g);
		// sets target member index
			virtual void pass(int val);
		}; // class Equip

		class Skill : public GameMode
		{
		private:
			int curCharID_;
			std::vector< uint16_t > idList_;
		public:
			Skill(rpg2k::Main& m);

			virtual void reset();
			virtual void run(input::KeyListener& keyList);
			virtual void gameModeChanged();
			virtual void draw(Graphics2D& g);
		// sets skill using member index
			virtual void pass(int val);
		}; // class Skill
	} // namespace gamemode
} // namespace rpg2k

#endif
