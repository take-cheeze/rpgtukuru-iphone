#include "Array1DWrapper.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Encode.hpp"
#include "Stream.hpp"

#include <cctype>


namespace rpg2k
{
	CharSet::Dir::Type toCharSetDir(EventDir::Type const dir)
	{
		switch(dir) {
			case EventDir::DOWN : return CharSet::Dir::DOWN ;
			case EventDir::LEFT : return CharSet::Dir::LEFT ;
			case EventDir::RIGHT: return CharSet::Dir::RIGHT;
			case EventDir::UP   : return CharSet::Dir::UP   ;
			default: return CharSet::Dir::DOWN;
		}
	}
	EventDir::Type toEventDir(CharSet::Dir::Type const key)
	{
		switch(key) {
			case CharSet::Dir::UP   : return EventDir::UP   ;
			case CharSet::Dir::LEFT : return EventDir::LEFT ;
			case CharSet::Dir::RIGHT: return EventDir::RIGHT;
			case CharSet::Dir::DOWN : return EventDir::DOWN ;
			default: return EventDir::DOWN;
		}
	}

	/*
	 * xor shift random number generator
	 * from http://ja.wikipedia.org/wiki/Xorshift
	 */
	unsigned random()
	{
		static unsigned x=123456789, y=362436069, z=521288629, w=88675123;

		unsigned t = ( x^(x << 11) );
		x=y; y=z; z=w;
		return ( w = ( w^(w >> 19) ) ^ ( t^(t >> 8) ) );
	}
	unsigned random(unsigned const max)
	{
		return ( random() % max );
	}
	int random(int const min, int const max)
	{
		return ( random(max - min + 1) + min );
	}

	SystemString RPG2kString::toSystem() const
	{
		return Encode::instance().toSystem(*this);
	}
	 RPG2kString SystemString::toRPG2k () const
	{
		return Encode::instance().toRPG2k (*this);
	}

	namespace structure
	{
		unsigned berSize(uint32_t num)
		{
			unsigned ret = 0;
			do {
				ret++;
				num >>= BER_BIT;
			} while(num);
			return ret;
		}
	} // namespace structure

	bool Binary::isNumber() const
	{
		if( !size() || ( ( size() > ( sizeof(uint32_t) * CHAR_BIT ) / structure::BER_BIT + 1) ) ) return false;

		const_reverse_iterator it = std::vector<uint8_t>::rbegin();
		if( *it > structure::BER_SIGN ) return false;

		while( ++it < std::vector<uint8_t>::rend() ) if( *it < structure::BER_SIGN ) return false;

		return true;
	}
	bool Binary::isString() const
	{
		for(const_iterator i = begin(); i < end(); i++) if( std::iscntrl(*i) ) return false;
		try {
			this->toString().toSystem();
			return true;
		} catch(debug::AnalyzeException const&) { return false; }
	}

	RPG2kString Binary::toString() const
	{
		// rpg2k_assert( isString() ); // will be so slow if enabled
		return RPG2kString( (char*)pointer(), size() );
	}
	int Binary::toNumber() const
	{
		rpg2k_assert( isNumber() );

		structure::StreamReader s( std::auto_ptr<structure::StreamInterface>(
			new structure::BinaryReaderNoCopy(*this) ) );
		return s.ber();
	}
	bool Binary::toBool() const
	{
		rpg2k_assert( size() == sizeof(bool) );
		switch( toNumber() ) {
			case false: return false;
			case true : return true ;
			default: rpg2k_assert(false);
		}
		return false;
	}
	double Binary::toDouble() const
	{
		rpg2k_assert( size() == sizeof(double) );
		return *( (double*)pointer() );
	}

	void Binary::setString(RPG2kString const& str)
	{
		resize( str.size() );
		std::memcpy( this->pointer(), str.c_str(), str.size() );
	}
	void Binary::setNumber(int32_t num)
	{
		resize( structure::berSize(num) );
		structure::StreamWriter(*this).setBER(num);
	}
	void Binary::setBool(bool b)
	{
		resize( sizeof(bool) );
		(*this)[0] = b;
	}
	void Binary::setDouble(double d)
	{
		resize( sizeof(double) );
		*( (double*)this->pointer() ) = d;
	}

	unsigned Binary::serializedSize() const { return size(); }
	void Binary::serialize(structure::StreamWriter& s) const
	{
		s.write(*this);
	}
} // namespace rpg2k
