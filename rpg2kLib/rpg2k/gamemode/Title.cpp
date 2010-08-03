#include "../Audio2D.hpp"
#include "../Graphics2D.hpp"
#include "../KeyListener.hpp"
#include "../Main.hpp"
#include "../Project.hpp"


namespace rpg2k
{
	namespace gamemode
	{
		Title::Title(Main& m)
		: GameMode(m)
		{
			structure::Array1D const& voc = getOwner().getProject().getLDB()[21];

			command_.push_back( voc[0x72] );
			command_.push_back( voc[0x73] );
			command_.push_back( voc[0x75] );

			int
				max = font::Font::width(command_[0]),
				cmp = font::Font::width(command_[1]);
			if(max < cmp) max = cmp;
			cmp = font::Font::width(command_[2]);
			if(max < cmp) max = cmp;

			menuSize_(max + 8*2, 64);
			menuCoord_( (SCREEN_SIZE[0]-menuSize_[0]) / 2, getOwner().hideTitle() ? 88 : 148 );
			cursorSize_(menuSize_[0] - 4*2, CURSOR_H);
			cursorCoord_(menuCoord_[0] + 4, menuCoord_[1] + 8);
		}
		Title::~Title()
		{
		}
		void Title::reset()
		{
			clearCursor();
			addCursor(3);
		}
		void Title::pass(int val)
		{
		}
		void Title::gameModeChanged()
		{
			structure::Array1D& sys = getOwner().getProject().getLDB()[22];
		// check save data exists
			noContinue_ = true;
			for(uint i = ID_MIN; i <= SAVE_DATA_MAX; i++) {
				if( getOwner().getProject().getLSD(i).exists() ) {
					noContinue_ = false;
					break;
				}
			}
		// play bgm
			getOwner().getAudio2D().playMusic(sys[31]);
		}
		void Title::run(input::KeyListener& keyList)
		{
			if( cursorCountLeft() ) return;

			structure::Array1D const& sys = getOwner().getProject().getLDB()[22];
			Audio2D& audio = getOwner().getAudio2D();
		// process key input
			if( keyList.enter() ) {
				switch( cursor() ) {
					case TO_NEW_GAME:
						audio.playSound(sys[42]);
						getOwner().callGameMode(GameMode::Field);
						break;
					case TO_LOAD:
						if(!noContinue_) {
							audio.playSound(sys[42]);
							getOwner().passToGameMode( GameMode::SaveManager, SaveManager::LOAD );
							getOwner().callGameMode(GameMode::SaveManager);
						}
						break;
					case TO_QUIT:
						audio.playSound(sys[42]);
						getOwner().quit();
						break;
					default: break;
				}
			}
		// process cursor key
			switch( keyList.getCursor() ) {
				case Key::UP  : if( decCursor() ) audio.playSound(sys[41]); break;
				case Key::DOWN: if( incCursor() ) audio.playSound(sys[41]); break;
				default: break;
			}
		}
		void Title::draw(Graphics2D& g)
		{
		// background
			if( getOwner().hideTitle() ) g.drawWallpaper( Vector2D(0, 0), SCREEN_SIZE );
			else g.drawImage(g.getImage(Material::Title, getOwner().getProject().getLDB()[22].getArray1D()[17]), Vector2D(0, 0) );
		// menu window
			g.drawWindow(menuCoord_, menuSize_);
		// menu cursor
			g.drawCursor(
				Vector2D( cursorCoord_[0], cursorCoord_[1] + cursorSize_[1]*cursor() ) + cursorMove(),
				cursorSize_
			);
		// menu string
			g.drawString(command_[0], Vector2D(menuCoord_[0] + 8, menuCoord_[1] + 10 + 16*0), font::FNT_NORMAL);
			g.drawString(command_[1], Vector2D(menuCoord_[0] + 8, menuCoord_[1] + 10 + 16*1), noContinue_ ? font::FNT_DISABLE : font::FNT_NORMAL);
			g.drawString(command_[2], Vector2D(menuCoord_[0] + 8, menuCoord_[1] + 10 + 16*2), font::FNT_NORMAL);
		}
	} // namespace gamemode
} // namespace rpg2k
