#ifndef _INC__RPG2K__MODEL__ARRAY_2D_HPP
#define _INC__RPG2K__MODEL__ARRAY_2D_HPP

#include "Stream.hpp"
#include "Struct.hpp"
#include "Event.hpp"

namespace rpg2kLib
{
	namespace structure
	{
		class Array1D;
		class Element;
		class Descriptor;

		typedef const Map< uint, Descriptor >& ArrayDefine;

		class Array2D;
		template< > Map< uint, Array1D >::Iterator& Map< uint, Array1D >::Iterator::operator ++();
		template< > Map< uint, Array1D >::Iterator& Map< uint, Array1D >::Iterator::operator --();
		template< > uint Map< uint, Array1D >::size() const;
		template< > Map< uint, Array1D >::Iterator Map< uint, Array1D >::begin() const;
		template< > Map< uint, Array1D >::Iterator Map< uint, Array1D >::end  () const;

		class Array2D
		{
		private:
			Binary BIN_DATA;

			Map< uint, Array1D > DATA;
			ArrayDefine ARRAY_DEFINE;

			Element* THIS;
		protected:
			Array2D();
			void init(StreamReader& f);
		public:
			typedef Map< uint, Array1D >::Iterator Iterator;

			Array2D(const Array2D& array);

			Array2D(ArrayDefine info);
			Array2D(ArrayDefine info, StreamReader& s);
			Array2D(ArrayDefine info, Binary& b);

			Array2D(Element& e, const Descriptor& info);
			Array2D(Element& e, const Descriptor& info, StreamReader& f);
			Array2D(Element& e, const Descriptor& info, Binary& b);

			virtual ~Array2D();

			Array2D& operator =(const Array2D& src);

			Array1D& operator [](uint index);
			const Array1D& operator [](uint index) const;

			bool exists(uint index) const;
			bool exists(uint index1, uint index2) const;

			void remove(uint index) { DATA.remove(index); }
			void clear() { DATA.clear(); }

			ArrayDefine getArrayDefine() const { return ARRAY_DEFINE; }

			bool isElement() const { return THIS != NULL; }
			Element& toElement() const;

			uint getSize();
			const Binary& toBinary();

			Iterator begin() const { return DATA.begin(); }
			Iterator end  () const { return DATA.end  (); }
		};

	}; // namespace structure

}; // namespace rpg2kLib

#endif
