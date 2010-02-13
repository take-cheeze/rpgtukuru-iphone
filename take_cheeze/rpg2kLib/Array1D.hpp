#ifndef _INC__RPG2K__MODEL__ARRAY_1D_HPP
#define _INC__RPG2K__MODEL__ARRAY_1D_HPP

#include "Array2D.hpp"

namespace rpg2kLib
{
	namespace model
	{
		class Sound;
		class Music;
		class EventState;
	};

	namespace structure
	{
		using namespace rpg2kLib::model;
		using namespace std;

		class Array2D;
		class Descriptor;
		class Element;

		class Array1D;
		template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator ++();
		template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator --();
		template< > uint Map< uint, Element >::size() const;
		template< > Map< uint, Element >::Iterator Map< uint, Element >::begin() const;
		template< > Map< uint, Element >::Iterator Map< uint, Element >::end  () const;

		class Array1D
		{
		private:
			Binary BIN_DATA;

			Map< uint, Element > DATA;
			ArrayDefine ARRAY_DEFINE;

			Element& THIS;

			bool EXISTS;
			Array2D* OWNER;
			uint INDEX;

			static const uint ARRAY_1D_END = 0;
		protected:
			Array1D();

			void init(Stream& s);
		public:
			typedef Map< uint, Element >::Iterator Iterator;

			Array1D(const Array1D& array);

			Array1D(Element& e, const Descriptor& info);
			Array1D(Element& e, const Descriptor& info, Stream& f);
			Array1D(Element& e, const Descriptor& info, Binary& b);

			Array1D(Array2D& owner, uint index);
			Array1D(Array2D& owner, uint index, Stream& f);

			virtual ~Array1D();

			Array1D& operator =(const Array1D& src);

		// throws exception if it's not a part of Array2D
			bool isArray2D() const { return OWNER != NULL; }
			uint getIndex() const;
			Array2D& getOwner() const;

			Element& operator [](uint index);
			const Element& operator [](uint index) const;

			bool exists() const;
			bool exists(uint index) const;

			uint getSize();
			const Binary& toBinary();

			void substantiate();
			void remove(uint index) { DATA.remove(index); }
			void clear() { EXISTS = false; DATA.clear(); }

			ArrayDefine getArrayDefine() const { return ARRAY_DEFINE; }
			Element& toElement() const { return THIS; }

			operator Music&() { return static_cast< Music& >(*this); }
			operator Sound&() { return static_cast< Sound& >(*this); }
			operator EventState&() { return static_cast< EventState& >(*this); }

			Iterator begin() const { return DATA.begin(); }
			Iterator end  () const { return DATA.end  (); }
		};
	}; // namespace structure

}; // namespace rpg2kLib

#endif
