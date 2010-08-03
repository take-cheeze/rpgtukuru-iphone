#include "Main.hpp"
#include "KeyListener.hpp"

#include <SDL/SDL.h>

namespace rpg2k
{
	namespace input
	{
		namespace
		{
			namespace PSPKey { enum {
			// 0 ~ 13
				// symbol
				TRIANGLE=0, CIRCLE, CROSS, SQUARE,
				// trigger
				LEFT_TRIGGER, RIGHT_TRIGGER,
				// cursor
				DOWN, LEFT, UP, RIGHT,
				// other
				SELECT, START, HOME, HOLD,
			// sound control
				// F10
				SOUND_PLAY = SDLK_F10, SOUND_PAUSE = SDLK_F10,
				SOUND_PLAY_PAUSE = SDLK_F10,
				// F11
				SOUND_FORWARD = SDLK_F11,
				// F12
				SOUND_BACK = SDLK_F12,
				// F13
				SOUND_VOLUME_UP = SDLK_F13,
				// F14
				SOUND_VOLUME_DOWN = SDLK_F14,
				// F15
				SOUND_HOLD = SDLK_F15,
			}; } // namespace PSPKey
		} // namespace

		KeyListener::KeyListener(Main& m)
		: owner_(m), buffer_(), cursor_()
		{
			clear();
		}

		KeyListener::~KeyListener()
		{
		}

		Key::Type KeyListener::get() const
		{
			if( buffer_.empty() ) return Key::NONE;
			else return buffer_.front();
		}
		Key::Type KeyListener::pop()
		{
			if(buffer_.empty())
			{
				return Key::NONE;
			} else {
				Key::Type ret = buffer_.front();
				buffer_.pop();
				return ret;
			}
		}
		void KeyListener::push(Key::Type key)
		{
			buffer_.push(key);
		}

		Key::Type KeyListener::getCursor()
		{
			if( cursor_.empty() ) {
				lastCursor_ = Key::NONE;
				return Key::NONE;
			} else {
				lastCursor_ = cursor_.top();
				return cursor_.top();
			}
		}

		void KeyListener::removeCursor(Key::Type key)
		{
			std::stack< Key::Type > buf;

			while( !cursor_.empty() ) {
				if(cursor_.top() != key) buf.push( cursor_.top() );
				cursor_.pop();
			}
			while( !buf.empty() ) {
				cursor_.push(buf.top());
				buf.pop();
			}
		}

		void KeyListener::clear()
		{
			clearBuffer();
			clearCursor();
			alt_ = ctrl_ = shift_ = false;
			cancel_ = enter_ = false;
		}
		void KeyListener::clearBuffer()
		{
			while(!buffer_.empty()) buffer_.pop();
		}
		void KeyListener::clearCursor()
		{
			while(!cursor_.empty()) cursor_.pop();
			left_ = up_ = right_ = down_ = false;
		}

		void KeyListener::load()
		{
			SDL_Event event;

			enter_ = cancel_ = false;

			while(SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_QUIT: owner_.quit();
						break;
					case SDL_KEYDOWN:
						switch(event.key.keysym.sym) {
							case SDLK_ESCAPE:
							case SDLK_x: case SDLK_c: case SDLK_v: case SDLK_b:
							case SDLK_n: case SDLK_KP0:
								cancel_ = true;
								// push(Key::CANCEL);
								break;
							case SDLK_RETURN:
								if(alt_) {
									break;
								}
							case SDLK_SPACE: case SDLK_z:
								enter_ = true;
								push(Key::ENTER);
								break;
							case SDLK_UP:    case SDLK_k: case SDLK_KP8:
								if(!up_) cursor_.push(Key::UP);
								up_ = true;
								break;
							case SDLK_DOWN:  case SDLK_j: case SDLK_KP2:
								if(!down_) cursor_.push(Key::DOWN);
								down_ = true;
								break;
							case SDLK_LEFT:  case SDLK_h: case SDLK_KP4:
								if(!left_) cursor_.push(Key::LEFT);
								left_ = true;
								break;
							case SDLK_RIGHT: case SDLK_l: case SDLK_KP6:
								if(!right_) cursor_.push(Key::RIGHT);
								right_ = true;
								break;
							case SDLK_LALT: case SDLK_RALT:
								alt_ = true;
								break;
							case SDLK_LCTRL: case SDLK_RCTRL:
								push(Key::IGNORE_BLOCK);
								break;
							case SDLK_LSHIFT: case SDLK_RSHIFT:
								shift_ = true;
								break;
							case SDLK_F3: // switch to/from always on top window
								break;
							case SDLK_F4: // switch to/from full screen
								break;
							case SDLK_F5: // switch window size
								break;
						#if RPG2K_IS_PSP
							case PSPKey::SOUND_PLAY_PAUSE:
								break;
							case PSPKey::SOUND_FORWARD:
								break;
							case PSPKey::SOUND_BACK:
								break;
							case PSPKey::SOUND_VOLUME_UP:
								break;
							case PSPKey::SOUND_VOLUME_DOWN:
								break;
							case PSPKey::SOUND_HOLD:
								break;
						#else
							case SDLK_F10: // interrupt event
								push(Key::CLOSE_EVENT);
								break;
							case SDLK_F12: // goto title
								owner_.gotoTitle();
								break;
						#endif
							default: break;
						}
						break;
					case SDL_KEYUP:
						switch(event.key.keysym.sym) {
							case SDLK_UP:    case SDLK_k: case SDLK_KP8:
								removeCursor(Key::UP);
								up_ = false;
								break;
							case SDLK_DOWN:  case SDLK_j: case SDLK_KP2:
								removeCursor(Key::DOWN);
								down_ = false;
								break;
							case SDLK_LEFT:  case SDLK_h: case SDLK_KP4:
								removeCursor(Key::LEFT);
								left_ = false;
								break;
							case SDLK_RIGHT: case SDLK_l: case SDLK_KP6:
								removeCursor(Key::RIGHT);
								right_ = false;
								break;
							case SDLK_LALT: case SDLK_RALT:
								alt_ = false;
								break;
							case SDLK_LCTRL: case SDLK_RCTRL:
								ctrl_ = false;
								break;
							case SDLK_LSHIFT: case SDLK_RSHIFT:
								shift_ = false;
								break;
							case SDLK_F13:
								break;
							case SDLK_F14:
								break;
							default:
								break;
						}
						break;
				#if RPG2K_IS_PSP
					case SDL_JOYBUTTONDOWN:
						switch(event.jbutton.button) {
							case PSPKey::TRIANGLE:
								break;
							case PSPKey::CIRCLE:
								enter_ = true;
								// push(Key::ENTER);
								break;
							case PSPKey::CROSS:
								cancel_ = true;
								// push(Key::CANCEL);
								break;
							case PSPKey::SQUARE:
								break;
							case PSPKey::LEFT_TRIGGER: case PSPKey::RIGHT_TRIGGER:
								shift_ = true;
								break;
							case PSPKey::DOWN:
								if(!down_) cursor_.push(Key::DOWN);
								down_ = true;
								break;
							case PSPKey::LEFT:
								if(!left_) cursor_.push(Key::LEFT);
								left_ = true;
								break;
							case PSPKey::UP:
								if(!up_) cursor_.push(Key::UP);
								up_ = true;
								break;
							case PSPKey::RIGHT:
								if(!right_) cursor_.push(Key::RIGHT);
								right_ = true;
								break;
							case PSPKey::SELECT:
								push(Key::QUICK_LOAD);
								break;
							case PSPKey::START:
								owner_.gotoTitle();
								break;
							case PSPKey::HOME:
								owner_.quit();
								break;
							case PSPKey::HOLD:
								break;
							default:
								break;
						}
						break;
					case SDL_JOYBUTTONUP:
						switch(event.jbutton.button) {
							case PSPKey::LEFT_TRIGGER: case PSPKey::RIGHT_TRIGGER:
								shift_ = false;
								break;
							case PSPKey::DOWN:
								removeCursor(Key::DOWN);
								down_ = false;
								break;
							case PSPKey::LEFT:
								removeCursor(Key::LEFT);
								left_ = false;
								break;
							case PSPKey::UP:
								removeCursor(Key::UP);
								up_ = false;
								break;
							case PSPKey::RIGHT:
								removeCursor(Key::RIGHT);
								right_ = false;
								break;
							default:
								break;
						}
						break;
				#endif
					default:
						break;
				}
			}
		}
	} // namespace input
} // namespace rpg2k
