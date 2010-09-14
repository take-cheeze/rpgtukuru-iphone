#include "../Audio2D.hpp"
#include "../Graphics2D.hpp"
#include "../KeyListener.hpp"
#include "../Main.hpp"
#include "../Project.hpp"


namespace rpg2k
{
	namespace gamemode
	{
		#define PP_upDown() \
			switch( keyList.getCursor() ) { \
				case Key::DOWN: if( incCursor() ) getOwner().getAudio2D().playSound(getOwner().getProject().getLDB()[22].getArray1D()[41]); break; \
				case Key::UP  : if( decCursor() ) getOwner().getAudio2D().playSound(getOwner().getProject().getLDB()[22].getArray1D()[41]); break; \
				default: break; \
			}

		Equip::Equip(rpg2k::Main& m)
		: GameMode(m)
		{
			structure::Array1D& voc = getOwner().getProject().getLDB()[21];
		// status name
			for(int i = 0; i < EQ_PARAM_NUM; i++) status_.push_back( voc[0x84 + i] );
		// equipment name
			for(int i = 0; i < rpg2k::Equip::END; i++) equip_.push_back( voc[0x88 + i] );
		}
		void Equip::draw(Graphics2D& g)
		{
			model::Project& proj = getOwner().getProject();
			model::SaveData const& lsd = proj.getLSD();
			model::DataBase const& ldb = proj.getLDB();

			structure::Array2D const& item = ldb[13];
			int curCharID = lsd.member( cursor(0) );
			structure::Array1D const& curChar = lsd[108].getArray2D()[curCharID];
			std::vector< uint16_t > curCharEquip = curChar[61].getBinary();

		// window
			g.drawWindow( Vector2D(  0,   0), Size2D(SCREEN_SIZE[0], 32) );
			g.drawWindow( Vector2D(  0,  32), Size2D(124, 96) );
			g.drawWindow( Vector2D(124,  32), Size2D(196, 96) );
			g.drawWindow( Vector2D(  0, 128), Size2D(SCREEN_SIZE[0], 124) );
		// cursor
		// comment
			switch( cursorNum() ) {
				case 2:
					g.drawCursor( Vector2D(128, cursor(1)*16+40) + cursorMove(8), Size2D(188, 16) );
					if( curCharEquip[cursor(1)] ) g.drawString( item[ curCharEquip[cursor(1)] ][2], Vector2D(8, 10) );
					break;
				case 3:
					g.drawCursorPassive( Vector2D(128, cursor(1)*16+40), Size2D(188, 16) );
					g.drawString( item[ idList_[cursor(1)] ][2], Vector2D(8, 8) );
					break;
			}
		// status
			g.drawString( proj.name(curCharID), Vector2D(8, 42) );
			for(int i = 0; i < EQ_PARAM_NUM; i++)
				g.drawString(status_[i], Vector2D(8, 58 + CURSOR_H*i), font::FNT_STATUS);
		// equipment
			for(int i = 0; i < rpg2k::Equip::END; i++) {
				g.drawString(equip_[i], Vector2D(132, 42 + CURSOR_H*i), font::FNT_STATUS);
				if( curCharEquip[i] )
					g.drawString( item[ curCharEquip[i] ][1] , Vector2D(132 + font::FULL_FONT_W*5, 42 + CURSOR_H*i) );
			}
		// item list
		}
		void Equip::run(input::KeyListener& keyList)
		{
			if( cursorCountLeft() ) return;

			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();

			switch( cursorNum() ) {
				case 2: // select equip type
					if( keyList.cancel() ) removeLastCursor(); // skip return to source char select
					PP_upDown()
					switch( keyList.getCursor() ) {
						case Key::RIGHT: incCursor(0); break;
						case Key::LEFT : decCursor(0); break;
						default: break;
					}
					break;
				case 3: // select equiping item
					break;
					if( keyList.enter() ) {
						proj.unequip( lsd.member( cursor(0) ),  rpg2k::Equip::Type( cursor(1) ) );
						proj.  equip( lsd.member( cursor(0) ),  idList_[ cursor(2) ] );
						removeLastCursor();
					}
					switch( keyList.getCursor() ) {
						case Key::UP   : decCursor(); cursor()--; break;
						case Key::RIGHT: incCursor(); break;
						case Key::LEFT : decCursor(); break;
						case Key::DOWN : incCursor(); cursor()++; break;
						default: break;
					}
					break;
			}

			if( keyList.cancel() ) {
				if( cursorNum() <= 2 ) getOwner().returnGameMode();
				else removeLastCursor();
			}
		}
		void Equip::reset()
		{
			clearCursor();
		}
		void Equip::gameModeChanged()
		{
		}
		void Equip::pass(int val)
		{
			clearCursor();
			addCursor( getOwner().getProject().getLSD().memberNum() );
			cursor(0) = val;
			addCursor( rpg2k::Equip::END );
		}

		Item::Item(rpg2k::Main& m)
		: GameMode(m)
		{
		}
		void Item::draw(Graphics2D& g)
		{
			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();
			model::DataBase& ldb = proj.getLDB();

			std::ostringstream ss;
			switch( cursorNum() ) {
				case 1: { // select item
					std::map< uint16_t, model::SaveData::Item >& itemList = lsd.item();
					structure::Array2D const& item = ldb[13];

					g.drawWindow( Vector2D(0,  0), Size2D(SCREEN_SIZE[0], 32) );

					g.drawWindow( Vector2D(0, 32), SCREEN_SIZE - Size2D(0, 32) );
					g.drawCursor(
						Vector2D(4, 42) + Vector2D( cursor() % 2 * 160,  cursor() / 2 * CURSOR_H ),
						Size2D(152, CURSOR_H)
					);
					int index = 0;
					for(std::map< uint16_t, model::SaveData::Item >::const_iterator it = itemList.begin(); it != itemList.end(); ++it) {
						Vector2D base = Vector2D(8, 42) + Vector2D( index % 2 * 160,  index / 2 * CURSOR_H );
						if( cursor() == index ) {
							g.drawString( item[it->first][2], Vector2D(8, 10) );
						}

						g.drawString(item[it->first][1], base);
						ss.str(""); ss << ":" << std::setw(2) << it->second.num;
						g.drawString( ss.str(), base + Vector2D( font::HALF_FONT_W*21, 0 ) );

						index++;
					}
				} break;
				case 2: // select char
					break;
			}
		}
		void Item::run(input::KeyListener& keyList)
		{
			if( cursorCountLeft() ) return;

			switch( cursorNum() ) { // item
				case 1: // select item
					if( keyList.enter() ) addCursor( getOwner().getProject().getLSD().item().size() + 1 );
					switch( keyList.getCursor() ) {
						case Key::UP   : decCursor(); cursor()--; break;
						case Key::RIGHT: incCursor(); break;
						case Key::LEFT : decCursor(); break;
						case Key::DOWN : incCursor(); cursor()++; break;
						default: break;
					}
					break;
				case 2: // select char
					if( keyList.cancel() ) {
						cursorMax(0) = getOwner().getProject().getLSD().item().size() + 1;
						if( cursor(0) >= cursorMax(0) ) cursor(0) = cursorMax(0) - 1;
					}
					break;
			}

			if( keyList.cancel() ) {
				if( cursorNum() <= 1 ) getOwner().returnGameMode();
				else removeLastCursor();
			}
		}
		void Item::reset()
		{
			clearCursor();
		}
		void Item::gameModeChanged()
		{
			clearCursor();
			addCursor( getOwner().getProject().getLSD().item().size() + 1 );
		}
		void Item::pass(int val)
		{
		}

		Skill::Skill(rpg2k::Main& m)
		: GameMode(m)
		{
		}
		void Skill::draw(Graphics2D& g)
		{
			model::Project& proj = getOwner().getProject();
			// model::SaveData& lsd = proj.getLSD();
			model::DataBase& ldb = proj.getLDB();
			std::ostringstream ss;

			switch( cursorNum() ) {
				case 2: { // select skill
					structure::Array2D const& skill = ldb[12];

					g.drawWindow( Vector2D(0, 32*0), Size2D(SCREEN_SIZE[0], 32) );
					if( cursor() != (int)idList_.size() ) {
						g.drawString( skill[ idList_[ cursor() ] ][2], Vector2D(8, 10) );
					}

					g.drawWindow( Vector2D(0, 32*1), Size2D(SCREEN_SIZE[0], 32) );
					// name
					Vector2D base(8, 32 + 10);
					g.drawString( proj.name(curCharID_), base );
					// level
					base += Vector2D( font::HALF_FONT_W*14, 0 );
					g.drawString( ldb.vocabulary(128), base, font::FNT_STATUS );
					base += Vector2D( font::FULL_FONT_W*1, 0 );
					ss.str(""); ss << proj.level(curCharID_);
					g.drawString( ss.str(), base );
					// condition
					base += Vector2D( font::HALF_FONT_W*5, 0 );
					g.drawString( proj.condition(curCharID_), base, proj.conditionColor(curCharID_) );
					// HP
					base += Vector2D( font::HALF_FONT_W*10, 0 );
					g.drawString( ldb.vocabulary(129), base, font::FNT_STATUS );
					base += Vector2D( font::FULL_FONT_W*1, 0 );
					ss.str(""); ss << std::setw(3) << proj.hp(curCharID_) << "/" << std::setw(3) << proj.param(curCharID_, Param::HP);
					g.drawString( ss.str(), base );
					// MP
					base += Vector2D( font::HALF_FONT_W*9, 0 );
					g.drawString( ldb.vocabulary(130), base, font::FNT_STATUS );
					base += Vector2D( font::FULL_FONT_W*1, 0 );
					ss.str(""); ss << std::setw(3) << proj.mp(curCharID_) << "/" << std::setw(3) << proj.param(curCharID_, Param::MP);
					g.drawString( ss.str(), base );

					g.drawWindow( Vector2D(0, 32*2), SCREEN_SIZE - Size2D(0, 32*2) );
					g.drawCursor(
						Vector2D(4, 72) + Vector2D( cursor() % 2 * 160,  cursor() / 2 * CURSOR_H ),
						Size2D(152, CURSOR_H)
					);
					int index = 0;
					for(std::vector< uint16_t >::const_iterator it = idList_.begin(); it != idList_.end(); ++it) {
						if( *it == 0 ) break;

						base = Vector2D(8, 74) + Vector2D( index % 2 * 160, index / 2 * CURSOR_H );

						g.drawString(skill[*it][1], base);
						ss.str(""); ss << "-" << std::setw(3) << skill[*it][11].get<int>();
						g.drawString( ss.str(), base + Vector2D( font::HALF_FONT_W*20, 0 ) );

						index++;
					}
				} break;
				case 3: // select char
					break;
			}
		}
		void Skill::run(input::KeyListener& keyList)
		{
			if( cursorCountLeft() ) return;

			switch( cursorNum() ) {
				case 2: // select skill
					if( keyList.cancel() ) removeLastCursor(); // skip return to source char select
					switch( keyList.getCursor() ) {
						case Key::UP   :  --cursor(); decCursor(); break;
						case Key::RIGHT: incCursor(); break;
						case Key::LEFT : decCursor(); break;
						case Key::DOWN :  ++cursor(); incCursor(); break;
						default: break;
					}
					break;
				case 3:  // select destination char
					break;
			}

			if( keyList.cancel() ) {
				if( cursorNum() <= 2 ) getOwner().returnGameMode();
				else removeLastCursor();
			}
		}
		void Skill::reset()
		{
			clearCursor();
		}
		void Skill::gameModeChanged()
		{
		}
		void Skill::pass(int val)
		{
			model::SaveData& lsd = getOwner().getProject().getLSD();

			clearCursor();
			addCursor( lsd.memberNum() );
			cursor(0) = val;

			curCharID_ = lsd.member(val);
			idList_ = lsd[108].getArray2D()[curCharID_][52].getBinary();
			idList_.push_back(INVALID_ID);
			addCursor( idList_.size() + 1 );
		}

		Menu::Menu(rpg2k::Main& m)
		: GameMode(m)
		{
			structure::Array1D& voc = getOwner().getProject().getLDB()[21];
		// top command
			for(int i = 0; i < 3; i++) command_.push_back( voc[0x6a + i] );
			// save
			command_.push_back( voc[0x6e] );
			// quit game
			command_.push_back( voc[0x70] );
		// quit things
			quitMessage_ = voc[0x97].get_string();
			yes_ = voc[0x98].get_string();
			no_  = voc[0x99].get_string();

			quitMessageW_ = font::Font::width(quitMessage_);
			yesNoW_ = yes_.length() > no_.length() ? font::Font::width(yes_) : font::Font::width(no_);
		}
		void Menu::reset()
		{
			gameModeChanged();
		}
		void Menu::draw(Graphics2D& g)
		{
			g.drawWallpaper( Vector2D(0, 0), SCREEN_SIZE );

			// model::Project& proj = getOwner().getProject();
			// model::DataBase& ldb = proj.getLDB();
			// model::SaveData& lsd = proj.getLSD();

			if( cursorNum() == 1 ) drawMainMenu(g);
			else switch( cursor(0) ) {
				case MENU_SKILL:
				case MENU_EQUIP:
					drawMainMenu(g, true);
					break;
				case MENU_QUIT:
					g.drawWindow( Vector2D( (SCREEN_SIZE[0]-quitMessageW_-16)/2, 72 ), Size2D(quitMessageW_+16, 32) );
					g.drawString(quitMessage_, Vector2D( (SCREEN_SIZE[0]-quitMessageW_)/2, 82 ), font::FNT_NORMAL);

					g.drawWindow( Vector2D( (SCREEN_SIZE[0]-yesNoW_-16)/2, 120 ), Size2D(yesNoW_+16, 48) );
					g.drawCursor(
						Vector2D( (SCREEN_SIZE[0]-yesNoW_-8)/2, cursor()*CURSOR_H + 128 ) + cursorMove(),
						Size2D(yesNoW_+8, 16)
					);
					g.drawString(yes_, Vector2D( (SCREEN_SIZE[0]-yesNoW_)/2, 130 ), font::FNT_NORMAL);
					g.drawString(no_ , Vector2D( (SCREEN_SIZE[0]-yesNoW_)/2, 146 ), font::FNT_NORMAL);
					break;
				default: break;
			}
		}
		void Menu::run(input::KeyListener& keyList)
		{
			if( cursorCountLeft() ) return;

			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();

			if( cursorNum() == 1 ) PP_upDown()
			switch( cursor(0) ) {
				case 0: switch( cursorNum() ) { // item
					case 1: // to item select
						if( keyList.enter() ) getOwner().callGameMode( GameMode::Item );
						break;
				} break;
				case 1: switch( cursorNum() ) { // skill
					case 1: // to source char select
						if( keyList.enter() ) addCursor( lsd.memberNum() );
						break;
					case 2: // select source char
						if( keyList.enter() ) {
							getOwner().passToGameMode( GameMode::Skill, cursor() );
							removeLastCursor();
							getOwner().callGameMode( GameMode::Skill );
							return;
						}
						PP_upDown()
						break;
				} break;
				case 2: switch( cursorNum() ) { // equip
					case 1: // to char select
						if( keyList.enter() ) addCursor( lsd.memberNum() );
						break;
					case 2: // select char
						if( keyList.enter() ) {
							getOwner().passToGameMode( GameMode::Equip, cursor() );
							removeLastCursor();
							getOwner().callGameMode( GameMode::Equip );
							return;
						}
						PP_upDown()
						break;
				} break;
				case 3: switch( cursorNum() ) { // save
					case 1:
						if( keyList.enter() && proj.canSave() ) {
							getOwner().passToGameMode( GameMode::SaveManager, SaveManager::SAVE );
							getOwner().callGameMode(GameMode::SaveManager);
							return;
						}
						break;
				} break;
				case 4: switch( cursorNum() ) { // quit
					case 1:
						if( keyList.enter() ) addCursor(2);
						break;
					case 2:
						if( keyList.enter() ) getOwner().gotoTitle();
						else PP_upDown()
						break;
				} break;
			}

			if( keyList.cancel() ) {
				if( cursorNum() <= 1 ) getOwner().returnGameMode();
				else removeLastCursor();
			}
		}
		void Menu::gameModeChanged()
		{
			if( (cursorNum() != 1) || (cursor() != 4) ) {
				clearCursor();
				addCursor( command_.size() );
			}
		}
		void Menu::drawMainMenu(Graphics2D& g, bool charSelect)
		{
			model::Project& proj = getOwner().getProject();
			model::SaveData& lsd = proj.getLSD();
			model::DataBase& ldb = proj.getLDB();
			std::ostringstream ss;
		// main menu
			g.drawWindow( Vector2D(0, 0), Size2D(88, 96) );
			// cursor
			if(charSelect) g.drawCursorPassive( Vector2D(4, cursor(0)*16 + 8), Size2D(80, CURSOR_H) );
			else g.drawCursor( Vector2D(4, cursor(0)*16 + 8) + cursorMove(), Size2D(80, CURSOR_H) );
			// command
			font::FontColor c = lsd.memberNum() != 0 ? font::FNT_ENABLE : font::FNT_DISABLE;
			g.drawString(command_[0], Vector2D(8, 10 + CURSOR_H*0), font::FNT_ENABLE);
			g.drawString(command_[1], Vector2D(8, 10 + CURSOR_H*1), c);
			g.drawString(command_[2], Vector2D(8, 10 + CURSOR_H*2), c);
			g.drawString(command_[3], Vector2D(8, 10 + CURSOR_H*3), proj.canSave() ? font::FNT_ENABLE : font::FNT_DISABLE);
			g.drawString(command_[4], Vector2D(8, 10 + CURSOR_H*4), font::FNT_ENABLE);
		// money
			g.drawMoneyWindow( Vector2D( 0, 208) );
		// char info
			g.drawWindow( Vector2D(88,   0), Size2D(232, 240) );
			// char cursor
			if(charSelect) g.drawCursor( Vector2D(92, cursor(1)*58 + 4) + cursorMove(), Size2D(224, 56) );
			std::vector< uint16_t >& mem = lsd.member();
			for(uint i = 0; i < mem.size(); i++) {
				uint charID = mem[i];

				Vector2D base(88 + 8, 8 + (FACE_SIZE[1] + 10)*i);
				g.drawFaceSet( proj.faceSet(charID), proj.faceSetPos(mem[i]), base );

				base += Vector2D(FACE_SIZE[0] + 8, 3);
				g.drawString( proj.name(charID), base);
				g.drawString( proj.title(charID), base + Vector2D(font::HALF_FONT_W*15, 0) );

				base += Vector2D(0, font::FONT_H + 3);
				// level
				g.drawString(ldb.vocabulary(128), base, font::FNT_STATUS);
				ss.str(""); ss << std::setw(2) << proj.level(charID);
				g.drawString( ss.str(), base + Vector2D(font::FULL_FONT_W, 0) );
				// condition
				g.drawString( proj.condition(charID), base + Vector2D(font::HALF_FONT_W*7, 0), proj.conditionColor(charID) );
				// HP
				g.drawString( ldb.vocabulary(129), base + Vector2D(font::HALF_FONT_W*18, 0), font::FNT_STATUS );
				ss.str(""); ss << std::setw(3) << proj.hp(charID) << "/" << std::setw(3) << proj.param(charID, Param::HP);
				g.drawString( ss.str(), base + Vector2D(font::HALF_FONT_W*20, 0) );

				base += Vector2D(0, font::FONT_H + 3);
				// experience
				g.drawString(ldb.vocabulary(127), base, font::FNT_STATUS);
				ss.str("");
				if( proj.level(charID) >= LV_MAX ) ss << RPG2kString(6, '-') << "/" << RPG2kString(6, '-');
				else ss << std::setw(6) << proj.exp(charID) << "/" << std::setw(6) << proj.nextLevelExp(charID);
				g.drawString( ss.str(), base + Vector2D(font::FULL_FONT_W, 0) );
				// MP
				g.drawString( ldb.vocabulary(130), base + Vector2D(font::HALF_FONT_W*18, 0), font::FNT_STATUS );
				ss.str(""); ss << std::setw(3) << proj.mp(charID) << "/" << std::setw(3) << proj.param(charID, Param::MP);
				g.drawString( ss.str(), base + Vector2D(font::HALF_FONT_W*20, 0) );
			}
		}
		void Menu::pass(int val)
		{
		}
	} // namespace gamemode
} // namespace rpg2k
