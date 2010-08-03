#ifndef _INC_RPG2K__VECTOR_HPP
#define _INC_RPG2K__VECTOR_HPP

#include <cstdarg>
#include <cstring>


namespace rpg2k
{
	typedef float Real;

	template< uint D, typename T = Real >
	class Vector
	{
	private:
		T elm_[D];
	public:
/*
		T& operator[](uint index) { return elm_[index]; }
		T const& operator[](uint index) const { return elm_[index]; }
 */
		static uint dimension() { return D; }

		Vector() {}
		Vector(T start, ...)
		{
			va_list ap;
			T cur = start;

			va_start(ap, start);
			for(uint i = 0; i < D; i++) {
				(*this)[i] = cur;
				cur = va_arg(ap, T);
			}
			va_end(ap);
		}
		Vector(T (&val)[D]) { std::memcpy( elm_, val, sizeof(T)*D ); }
		Vector(Vector< D, T > const& src) { std::memcpy( this->elm_, src.elm_, sizeof(T)*D ); }
		template< typename OtherType >
		Vector(Vector< D, OtherType > const& src)
		{
			for(uint i = 0; i < D; i++) elm_[i] = src[i];
		}

		~Vector() {}

		Vector< D, T >& operator()(T start, ...)
		{
			va_list ap;
			T cur = start;

			va_start(ap, start);
			for(uint i = 0; i < D; i++) {
				(*this)[i] = cur;
				cur = va_arg(ap, T);
			}
			va_end(ap);

			return *this;
		}

		Vector< D, T > const& operator =(Vector< D, T > const& src)
		{
			memcpy( this->elm_, src.elm_, sizeof(T)*D );
			return *this;
		}
		template< typename OtherType >
		Vector< D, T > const& operator =(Vector< D, OtherType > const& src)
		{
			for(uint i = 0; i < D; i++) (*this)[i] = src[i];
			return *this;
		}
		template< typename OtherType >
		Vector< D, T > const& operator =(OtherType src)
		{
			for(uint i = 0; i < D; i++) (*this)[i] = src;
			return *this;
		}

		#define PP_vectorOperator(op) \
			template< typename OtherType > \
			Vector< D, T > const& operator op##=(Vector< D, OtherType > const& src) \
			{ \
				for(uint i = 0; i < D; i++) (*this)[i] op##= src[i]; \
				return *this; \
			} \
			template< typename OtherType > \
			Vector< D, OtherType > operator op(Vector< D, OtherType > const& src) const \
			{ \
				Vector< D, T > ret; \
				for(uint i = 0; i < D; i++) ret[i] = (*this)[i] op src[i]; \
				return ret; \
			} \
			template< typename OtherType > \
			Vector< D, T > const& operator op##=(OtherType src) \
			{ \
				for(uint i = 0; i < D; i++) (*this)[i] op##= src; \
				return *this; \
			} \
			template< typename OtherType > \
			Vector< D, T > operator op(OtherType src) const \
			{ \
				Vector< D, T > ret; \
				for(uint i = 0; i < D; i++) ret[i] = (*this)[i] op src; \
				return ret; \
			}
		PP_vectorOperator(+)
		PP_vectorOperator(-)
		PP_vectorOperator(*)
		PP_vectorOperator(/)
		PP_vectorOperator(%)
		#undef PP_vectorOperator

		bool operator ==(Vector< D, T > const& src)
		{
			for(uint i = 0; i < D; i++) if( (*this)[i] != src[i] ) return false;
			return true;
		}
		bool operator !=(Vector< D, T > const& src)
		{
			for(uint i = 0; i < D; i++) if( (*this)[i] == src[i] ) return false;
			return true;
		}

		operator T const*() const { return elm_; }
		operator T*() { return elm_; }
		T* pointer() { return elm_; }
		T const* pointer() const { return elm_; }
	}; // class Vector

	typedef Vector< 2, int > Vector2D;
	typedef Vector< 3, int > Vector3D;
	typedef Vector< 4, int > Vector4D;
	typedef Vector< 2, uint > Size2D;
	typedef Vector< 3, uint > Size3D;
	typedef Vector< 4, uint > Size4D;

	typedef Vector< 3, Real > ColorRGB;
	typedef Vector< 4, Real > ColorRGBA;
} // namespace rpg2k

#endif
