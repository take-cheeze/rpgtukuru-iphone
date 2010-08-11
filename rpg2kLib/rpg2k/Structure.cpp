#include "Debug.hpp"
#include "Element.hpp"
#include "Encode.hpp"

#include <cctype>


namespace rpg2k
{
	/*
	 * xor shift random number generator
	 * from http://ja.wikipedia.org/wiki/Xorshift
	 */
	uint random()
	{
		static uint x=123456789, y=362436069, z=521288629, w=88675123;

		uint t = ( x^(x << 11) );
		x=y; y=z; z=w;
		return ( w = ( w^(w >> 19) ) ^ ( t^(t >> 8) ) );
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
		uint getBERSize(uint32_t num)
		{
			uint ret = 0;
			do {
				ret++;
				num >>= BER_BIT;
			} while(num);
			// clog << "getBERSize() " << num << " : " << ret << endl;
			return ret;
		}
	} // namespace structure

	Binary::Binary(structure::Element& e, structure::Descriptor const& info)
	{
	}
	Binary::Binary(structure::Element& e, structure::Descriptor const& info, Binary const& b)
	{
		*this = b;
	}
	Binary::Binary(structure::Element& e, structure::Descriptor const& info, structure::StreamReader& s)
	{
		rpg2k_assert(false);
	}

	bool Binary::isNumber() const
	{
		if( !size() ) return false;

		const_reverse_iterator it = rbegin();
		if( *it > structure::BER_SIGN ) return false;

		while( ++it < rend() ) if( *it < structure::BER_SIGN ) return false;

		return true;
	}
	bool Binary::isString() const
	{
		for(uint i = 0; i < size(); i++) if( std::iscntrl( (*this)[i] )  ) return false;
		try {
			this->toString().toSystem();
			return true;
		} catch(std::runtime_error const&) { return false; }
	}

	RPG2kString Binary::toString() const
	{
		// rpg2k_assert( isString() ); // will be so slow if enabled
		return RPG2kString( (char*)pointer(), size() );
	}
	int Binary::toNumber() const
	{
		rpg2k_assert( isNumber() );

		structure::StreamReader s(*this);
		return s.getBER();
	}
	bool Binary::toBool() const
	{
		rpg2k_assert( size() == sizeof(bool) );
		switch( toNumber() ) {
			case false: return false;
			case true : return true ;
			default: throw std::runtime_error("Failed converting Binary to bool.");
		}
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
		resize( structure::getBERSize(num) );
		structure::StreamWriter(*this).setBER(num);
	}
	void Binary::setBool(bool b)
	{
		resize( sizeof(bool) );
		*( this->pointer() ) = b;
	}
	void Binary::setDouble(double d)
	{
		resize( sizeof(double) );
		*( (double*)this->pointer() ) = d;
	}

	uint Binary::serializedSize() const { return size(); }
	void Binary::serialize(structure::StreamWriter& s) const
	{
		s.write(*this);
	}

	void Binary::exchangeEndianIfNeed(uint8_t* dst, uint8_t const* src, uint sizeOf, uint eleNum)
	{
		if(eleNum == 0) return;

		#if RPG2K_IS_BIG_ENDIAN
			for(uint j = 0; j < eleNum; j++) {
				uint8_t const* srcCur = src + sizeOf*j
				uint8_t* dstCur = dst + sizeOf*j;
				for(uint i = 0; i < sizeOf; i++) dstCur[i] = srcCur[sizeOf-i-1];
			}
		#elif RPG2K_IS_LITTLE_ENDIAN
			std::memcpy(dst, src, sizeOf * eleNum);
		#else
			#error unsupported endian
		#endif
	}
} // namespace rpg2k
