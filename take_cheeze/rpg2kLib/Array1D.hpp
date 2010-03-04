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
		class Array2D;
		class Descriptor;
		class Element;

		template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator ++();
		template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator --();
		template< > Map< uint, Element >::Iterator  Map< uint, Element >::begin() const;
		template< > Map< uint, Element >::ReverseIterator& Map< uint, Element >::ReverseIterator::operator ++();
		template< > Map< uint, Element >::ReverseIterator& Map< uint, Element >::ReverseIterator::operator --();
		template< > Map< uint, Element >::ReverseIterator  Map< uint, Element >::rbegin() const;
		template< > uint Map< uint, Element >::size() const;

		class Array1D
		{
		private:
			Binary BIN_DATA;

			Map< uint, Element > DATA;
			ArrayDefine ARRAY_DEFINE;

			Element* THIS;

			bool EXISTS;
			Array2D* OWNER;
			uint INDEX;

			static const uint ARRAY_1D_END = 0;
		protected:
			Array1D();

			void init(StreamReader& s);
		public:
			typedef Map< uint, Element >::Iterator Iterator;
			typedef Map< uint, Element >::ReverseIterator ReverseIterator;

			Array1D(const Array1D& array);

			Array1D(ArrayDefine info);
			Array1D(ArrayDefine info, StreamReader& s);
			Array1D(ArrayDefine info, Binary& b);

			Array1D(Element& e, const Descriptor& info);
			Array1D(Element& e, const Descriptor& info, StreamReader& s);
			Array1D(Element& e, const Descriptor& info, Binary& b);

			Array1D(Array2D& owner, uint index);
			Array1D(Array2D& owner, uint index, StreamReader& f);

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

			bool isElement() const;
			Element& toElement() const;

			operator model::Music&() { return static_cast< model::Music& >(*this); }
			operator model::Sound&() { return static_cast< model::Sound& >(*this); }
			operator model::EventState&() { return static_cast< model::EventState& >(*this); }

			Iterator begin() const { return DATA.begin(); }
			Iterator end  () const { return DATA.end  (); }
			ReverseIterator rbegin() const { return DATA.rbegin(); }
			ReverseIterator rend  () const { return DATA.rend  (); }
		};

	}; // namespace structure

}; // namespace rpg2kLib

#endif
