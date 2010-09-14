#include "../Audio2D.hpp"
#include "../Graphics2D.hpp"
#include "../KeyListener.hpp"
#include "../Main.hpp"
#include "../Project.hpp"


namespace rpg2k
{
	namespace gamemode
	{
		SaveManager::SaveManager(rpg2k::Main& m)
		: GameMode(m)
		{
			model::DataBase const& ldb = getOwner().getProject().getLDB();
			level_   = ldb.vocabulary(128);
			hp_      = ldb.vocabulary(129);
			prefix_  = ldb.vocabulary(148);

			cursorSize_(font::Font::width(prefix_) + font::HALF_FONT_W*2 + 8, CURSOR_H);
			previewCoord_(0, 40);
			previewSize_( SCREEN_SIZE[0], 64 );
			messageCoord_(0, 0);
			messageSize_( SCREEN_SIZE[0], 32 );
		}
		SaveManager::~SaveManager()
		{
		}

		void SaveManager::pass(int val)
		{
			mode_ = val;
			switch(val) {
				case SAVE:
					message_ = getOwner().getProject().getLDB()[21].getArray1D()[146].get_string();
					break;
				case LOAD:
					message_ = getOwner().getProject().getLDB()[21].getArray1D()[147].get_string();
					break;
			}
		}

		void SaveManager::reset()
		{
			clearCursor();
			addCursor(SAVE_DATA_MAX - PREVIEW_NUM + 1, true);
			addCursor(SAVE_DATA_MAX, true);

			cursor() = getOwner().getProject().lastLSD() - ID_MIN;
			cursor(0) = cursor();
			if( cursor(0) > (int)(SAVE_DATA_MAX-PREVIEW_NUM) ) cursor(0) = SAVE_DATA_MAX-PREVIEW_NUM;
		}

		void SaveManager::draw(Graphics2D& g)
		{
			model::Project& proj = getOwner().getProject();

		// wallpaper
			g.drawWallpaper( Vector2D(0, 0), SCREEN_SIZE );
		// message
			g.drawWindow(messageCoord_, messageSize_);
			g.drawString(message_, Vector2D(8, 10), font::FNT_NORMAL);

			Vector2D
				prvOff =  offsetChanged_ ? cursorMove(-previewSize_[1]) : Vector2D(0, 0),
				curOff = !offsetChanged_ ? cursorMove( previewSize_[1]) : Vector2D(0, 0);
		// scroll cursor
			if( cursor(0) > 0 ) {
				g.drawScrollDw( Vector2D( (SCREEN_SIZE[0] - SCROLL_SIZE[0]) / 2, previewCoord_[1] - SCROLL_SIZE[1] ) );
			}
			if( cursor(0) < (int)(SAVE_DATA_MAX-PREVIEW_NUM) ) {
				g.drawScrollUp( Vector2D( (SCREEN_SIZE[0] - SCROLL_SIZE[0]) / 2, SCREEN_SIZE[1] - SCROLL_SIZE[1] ) );
			}
			g.clip(
				Vector2D(0, previewCoord_[1]),
				Size2D(previewSize_[0], previewSize_[1] * PREVIEW_NUM)
			);
		// preview
			std::ostringstream ss;
			for(int
				i = (prvOff[1] > 0) ? -1 : 0,
				len = (prvOff[1] < 0) ? PREVIEW_NUM+1 : PREVIEW_NUM;
				i < len; i++
			) {
				int base = previewCoord_[1] + previewSize_[1] * i + prvOff[1];
			// window
				g.drawWindow( Vector2D(0, base), previewSize_ );

			// check savedata's existance
				uint lsdID = i + cursor(0) + ID_MIN;
				if( proj.getLSD(lsdID).exists() ) {
					structure::Array1D const& pre = proj.getLSD(lsdID)[100];
				// faceSet
					for(uint j = 0; j < MEMBER_MAX; j++) {
						if( pre[21 + j*2].exists() )
							g.drawFaceSet( pre[21 + j*2], pre[22 + j*2].get<uint>(), Vector2D(96 + j*(FACE_SIZE[0]+8), base + 8) );
					}
				// first character name
					g.drawString(pre[11], Vector2D(8, base + 26), font::FNT_NORMAL);
				// level
					g.drawString(level_, Vector2D(8, base + 42), font::FNT_STATUS);
					ss.str("");
					ss << std::setw(2) << pre[12].get<int>();
					g.drawString( ss.str(), Vector2D(8 + font::FULL_FONT_W, base + 42), font::FNT_NORMAL );
				// current HP
					g.drawString(hp_, Vector2D(48, base + 42), font::FNT_STATUS);
					ss.clear(); ss.str("");
					ss << std::setw(3) << pre[13].get<int>();
					g.drawString( ss.str(), Vector2D(48 + font::FULL_FONT_W, base + 42), font::FNT_NORMAL );
				}
			// cursor
				if( i == (int)(cursor() - cursor(0)) ) {
					g.drawCursor(
						previewCoord_ + Vector2D( 4, 8 + previewSize_[1]*(cursor() - cursor(0)) ) + curOff,
						cursorSize_
					);
				}
			// savedata name
				ss.str("");
				ss << prefix_ << std::setw(2) << (cursor(0) + ID_MIN + i);
				switch(mode_) {
					case SAVE:
						g.drawString(ss.str(), previewCoord_ + Vector2D(8, 10 + previewSize_[1]*i) + prvOff, font::FNT_NORMAL);
						break;
					case LOAD:
						g.drawString(
							ss.str(), previewCoord_ + Vector2D(8, 10 + previewSize_[1]*i) + prvOff,
							proj.getLSD(cursor(0) + ID_MIN + i).exists() ? font::FNT_NORMAL : font::FNT_DISABLE
						);
						break;
				}
			}

			g.unclip();
		}
		void SaveManager::run(input::KeyListener& keyList)
		{
			model::Project& proj = getOwner().getProject();

			if( cursorCountLeft() ) return;

			if( keyList.enter() ) switch(mode_) {
				case SAVE:
					proj.saveLSD(cursor() + ID_MIN);
					getOwner().returnGameMode();
					return;
				case LOAD:
					if( proj.getLSD(cursor() + ID_MIN).exists() ) {
						proj.loadLSD(cursor() + ID_MIN);
						getOwner().callGameMode(GameMode::Field);
						return;
					}
					break;
			}
			if( keyList.cancel() ) getOwner().returnGameMode();

			structure::Array1D const& sys = getOwner().getProject().getLDB()[22];
			offsetChanged_ = false;
		// process cursor key
			switch( keyList.getCursor() ) {
				case Key::UP:
					if( decCursor() ) {
						getOwner().getAudio2D().playSound(sys[41]);
						if(cursor() < cursor(0)) {
							decCursor(0);
							offsetChanged_ = true;
						}
					}
					break;
				case Key::DOWN:
					if( incCursor() ) {
						getOwner().getAudio2D().playSound(sys[41]);
						if( (cursor() - cursor(0)) >= (int)PREVIEW_NUM ) {
							incCursor(0);
							offsetChanged_ = true;
						}
					}
					break;
				default:
					break;
			}
		}
		void SaveManager::gameModeChanged()
		{
			reset();
		}
	} // namespace gamemode
} // namespace rpg2k
