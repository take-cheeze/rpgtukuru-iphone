#include "../Graphics2D.hpp"
#include "../KeyListener.hpp"
#include "../Main.hpp"
#include "../Project.hpp"


namespace rpg2k
{
	namespace gamemode
	{
		GameOver::GameOver(Main& m)
		: GameMode(m)
		{
		}
		GameOver::~GameOver()
		{
		}
		void GameOver::reset()
		{
		}
		void GameOver::gameModeChanged()
		{
		}
		void GameOver::run(input::KeyListener& keyList)
		{
			if( keyList.enter() || keyList.cancel() ) getOwner().gotoTitle();
		}
		void GameOver::draw(Graphics2D& g)
		{
			RPG2kString filename = getOwner().getProject().getLDB()[22].getArray1D()[18];
			g.drawImage( g.getImage(Material::GameOver, filename), Vector2D(0, 0) );
		}
	} // namespace gamemode
} // namespace rpg2k
