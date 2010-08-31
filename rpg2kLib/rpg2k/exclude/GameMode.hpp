#ifndef _INC__RPG2K__GAME_MODE_HPP
#define _INC__RPG2K__GAME_MODE_HPP

#include "SpecialArray1D.hpp"

#include <vector>


namespace rpg2k
{
	class Graphics2D;
	class Main;

	namespace input
	{
		class KeyListener;
	}

	#define PP_allGameMode(com) \
		com(Battle) \
		com(Debug) \
		com(Equip) \
		com(Field) \
		com(GameOver) \
		com(Hotel) \
		com(Item) \
		com(Menu) \
		com(NameInput) \
		com(SaveManager) \
		com(Shop) \
		com(Skill) \
		com(Title) \

	class GameMode
	{
	public:
		enum Type
		{
			#define PP_enum(name) name,
			PP_allGameMode(PP_enum)
			#undef PP_enum
		};
	private:
		Main& owner_;

		int cursorCount_;
		struct CursorInfo { int point, max; bool stop; };
		std::vector< CursorInfo > cursor_;
	protected:
		Main& getOwner() { return owner_; }

		int& cursor();
		int& cursor(uint index);
		int& cursorMax();
		int& cursorMax(uint index);
		bool cursorCountLeft();
		Vector2D cursorMove(int size = CURSOR_H);
		// int cursorMove(int size = CURSOR_H);
		int cursorNum() const { return cursor_.size(); }
		void addCursor(int max, bool stop = false);
		void removeLastCursor();
		void clearCursor();

		// true if change happened
		bool incCursor();
		bool incCursor(int index);
		// true if change happened
		bool decCursor();
		bool decCursor(int index);
	public:
		GameMode(Main& m);
		virtual ~GameMode();
		virtual void reset() = 0;
		virtual void gameModeChanged() = 0;
		virtual void run(input::KeyListener& keyList) = 0;
		virtual void draw(Graphics2D& g) = 0;
		virtual void pass(int val) {}
	};

	namespace gamemode
	{
		#define PP_defineGameMode(name) \
			class name : public GameMode \
			{ \
			public: \
				name(Main& m); \
				virtual ~name(); \
				virtual void reset(); \
				virtual void gameModeChanged(); \
				virtual void run(input::KeyListener& keyList); \
				virtual void draw(Graphics2D& g); \
			} \

		#define PP_defineDefaultGameMode(name) \
			name::name(Main& m) : GameMode(m) {} \
			name::~name() {} \
			void name::reset() {} \
			void name::gameModeChanged() {} \
			void name::run(input::KeyListener& keyList) {} \
			void name::draw(Graphics2D& g) {} \

	} // namespace gamemode
} // namespace rpg2k

#include "gamemode/Battle.hpp"
#include "gamemode/Debug.hpp"
#include "gamemode/Field.hpp"
#include "gamemode/GameOver.hpp"
#include "gamemode/Menu.hpp"
#include "gamemode/SaveManager.hpp"
#include "gamemode/Title.hpp"

#endif
