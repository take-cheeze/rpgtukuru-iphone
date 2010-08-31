#ifndef _INC__RGP2K__ENCODE__ENCODE_HPP
#define _INC__RGP2K__ENCODE__ENCODE_HPP

#include "Define.hpp"

#include <iconv.h>

namespace rpg2k
{
	class Encode
	{
	private:
		static int const BUFF_SIZE = 1024;

		static std::string const SYSTEM_ENCODE;
		static std::string const RPG2K_ENCODE;

		::iconv_t toSystem_, toRPG2k_;
		std::string sysEncode_;
	protected:
		Encode();
		Encode(Encode const& e);
		Encode& operator =(Encode const& e);

		~Encode();

		static std::string convertString(std::string const& src, iconv_t cd);
		static iconv_t openConverter(std::string const& to, std::string const& from);
	public:
		static Encode& instance();

		SystemString toSystem( RPG2kString const& src) const { return convertString(src, toSystem_); }
		 RPG2kString toRPG2k (SystemString const& src) const { return convertString(src, toRPG2k_ ); }

		std::string const& systemEncoding() const { return sysEncode_; }
	}; // class Encode
} // namespace rpg2k

#endif
