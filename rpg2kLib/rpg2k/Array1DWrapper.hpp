#ifndef _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
#define _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP

#include "Array1D.hpp"
#include "Element.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Sound : public Array1D
		{
		public:
			Sound(Element& e) : Array1D(e) {}
			Sound(Element& e, StreamReader& s) : Array1D(e, s) {}
			Sound(Element& e, Binary const& b) : Array1D(e, b) {}

			RPG2kString const& fileName() const { return (*this)[1]; }
			int volume () const { return (*this)[3]; }
			int tempo  () const { return (*this)[4]; }
			int balance() const { return (*this)[5]; }

			operator RPG2kString() const { return fileName(); }
		}; // class Sound

		class Music : public Sound
		{
		public:
			Music(Element& e) : Sound(e) {}
			Music(Element& e, StreamReader& s) : Sound(e, s) {}
			Music(Element& e, Binary const& b) : Sound(e, b) {}

			int fadeInTime() const { return (*this)[2]; }
		}; // class Music

		class EventState : public Array1D
		{
		public:
			EventState(Element& e) : Array1D(e) {}
			EventState(Element& e, StreamReader& s) : Array1D(e, s) {}
			EventState(Element& e, Binary const& b) : Array1D(e, b) {}

			int mapID() const { return (*this)[11]; }
			int x() const { return (*this)[12]; }
			int y() const { return (*this)[13]; }
			Vector2D position() const { return Vector2D( x(), y() ); }

			RPG2kString const& charSet() const { return (*this)[73]; }
			int charSetPos() const { return (*this)[74]; }
			CharSet::Dir::Type charSetDir() const { return CharSet::Dir::Type( (*this)[75].to<int>() ); }
			CharSet::Pat::Type charSetPat() const { return CharSet::Pat::MIDDLE; }

			CharSet::Dir::Type eventDir() const { return CharSet::Dir::Type( (*this)[21].to<int>() ); }
			CharSet::Dir::Type  talkDir() const { return CharSet::Dir::Type( (*this)[22].to<int>() ); }

			// CharSetDir charSetDir() const;
		}; // class EventState
	} // namespace structure
} // namespace rpg2k

#endif // _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
