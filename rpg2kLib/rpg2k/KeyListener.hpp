#ifndef _INC__RPG2K__KEY_BUFFER
#define _INC__RPG2K__KEY_BUFFER

#include <queue>
#include <stack>
#include "Define.hpp"

namespace rpg2k
{
	class Main;

	namespace input
	{
		class KeyListener
		{
		private:
			Main& owner_;

			std::queue< Key::Type > buffer_;
			std::stack< Key::Type > cursor_;

			Key::Type lastCursor_;

			bool down_, right_, up_, left_;
			bool alt_, ctrl_, shift_;
			bool enter_, cancel_;
		protected:
			void removeCursor(Key::Type key);
		public:
			KeyListener(Main& m);
			~KeyListener();

			Key::Type get() const;
			Key::Type pop();
			void push(Key::Type key);

			Key::Type getCursor();
			Key::Type lastCursor() const { return lastCursor_; }

			bool left () const { return left_ ; }
			bool up   () const { return up_   ; }
			bool right() const { return right_; }
			bool down () const { return down_ ; }

			bool alt  () const { return alt_  ; }
			bool ctrl () const { return ctrl_ ; }
			bool shift() const { return shift_; }

			bool enter () const { return enter_ ; }
			bool cancel() const { return cancel_; }

			void clear();
			void clearBuffer();
			void clearCursor();

			void load();
		}; // class KeyListener
	} // namespace input
} // namespace rpg2k

#endif
