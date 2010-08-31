#ifndef _INC__RPG2K__MODEL__ELEMENT_HPP
#define _INC__RPG2K__MODEL__ELEMENT_HPP

#include "Descriptor.hpp"
#include "Stream.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Element
		{
		private:
			Descriptor const* descriptor_;
			Binary binData_;

			bool exists_;

			Element* owner_;
			int index1_, index2_;
		protected:
			Binary& getData() { return binData_; }

			class Factory;

			Element();
			Element(Binary const& b);

			Element(Descriptor const& info);
			Element(Descriptor const& info, Binary const& b);
			Element(Descriptor const& info, StreamReader& s);
		public:
			bool isDefined() const { return descriptor_ != NULL; }
			bool hasOwner() const { return owner_ != NULL; }
			bool exists() const { return exists_; }

			virtual ~Element();

			Descriptor const& getDescriptor() const;

			virtual uint serializedSize() const { return binData_.size(); }
			virtual void serialize(StreamWriter& s) const { s.write(binData_); }
			Binary serialize() const { return structure::serialize(*this); }

			#define PP_castOperator(type) \
				virtual operator type const&() const; \
				operator type&() { return const_cast<type&>(static_cast<type const&>(static_cast<Element const&>(*this))); }
			#define PP_castOperatorRef(type) \
				PP_castOperator(type) \
				type& get##type() { return get<type>(); } \
				type const& get##type() const { return get<type>(); }
			PP_allType(PP_castOperator)
			#undef PP_castOperator
			#undef PP_castOperatorRef

			#define PP_castOperator(type) \
				operator type const&() const { return reinterpret_cast<type const&>(getArray1D()); } \
				operator type&() { return const_cast<type&>(static_cast<type const&>(static_cast<Element const&>(*this))); } \
				type& get##type() { return get<type>(); } \
				type const& get##type() const { return get<type>(); }
			PP_castOperator(Music)
			PP_castOperator(Sound)
			PP_castOperator(EventState)
			#undef PP_castOperator

			string& get_string() { return get<string>(); }
			string const& get_string() const { return get<string>(); }

			void substantiate();

			template< typename T >
			T& get() { return static_cast< T& >(*this); }
			template< typename T >
			T const& get() const { return static_cast< T const& >(*this); }
			template< typename T >
			void set(T const& src) 
			{
				this->substantiate();
				static_cast< T& >(*this) = src;
			}
			template< typename T >
			T& operator =(T const& src) { this->set(src); return static_cast< T& >(*this); }

			operator uint&() { return reinterpret_cast< uint& >( get< int >() ); }

			Element& operator =(Element const& src);

			template< typename T >
			Binary& operator =(std::vector< T > const& src)
			{
				this->substantiate();
				this->get< Binary >() = src;
				return this->get< Binary >();
			}
			template< typename T >
			operator std::vector<T>() const { static_cast< std::vector<T> >( this->getBinary() ); }

			Element& getOwner() const;

			uint getIndex1() const;
			uint getIndex2() const;

			static std::auto_ptr<Element> copy(Element const& src);

			static std::auto_ptr<Element> create(Descriptor const& info);
			static std::auto_ptr<Element> create(Descriptor const& info, Binary const& b);
			static std::auto_ptr<Element> create(Descriptor const& info, StreamReader& s);

			static std::auto_ptr<Element> create(Array1D const& owner, uint index);
			static std::auto_ptr<Element> create(Array1D const& owner, uint index , Binary const& b);
			static std::auto_ptr<Element> create(Array2D const& owner, uint index1, uint index2);
			static std::auto_ptr<Element> create(Array2D const& owner, uint index1, uint index2, Binary const& b);
		}; // class Element

		template< > inline
		uint& Element::operator =(uint const& num) { (*this) = int(num); return *this; }
		template< > inline
		uint const& Element::get< uint >() const { return reinterpret_cast< uint const& >( get< int >() ); }
		template< > inline
		uint& Element::get< uint >() { return reinterpret_cast< uint& >( get< int >() ); }

		#define PP_operator(retType, op, type) \
			inline retType operator op(Element& e, type in) { return static_cast< type >(e) op in; } \
			inline retType operator op(type in, Element& e) { return in op static_cast< type >(e); }
		PP_operator(bool, ==, RPG2kString const&)
		PP_operator(bool, !=, RPG2kString const&)
		#undef PP_operator

		class BerEnum : public std::vector< uint >
		{
		private:
			Binary binData_;
		protected:
			void init(StreamReader& s);
		public:
			BerEnum(Element& e, Descriptor const& info) {}
			BerEnum(Element& e, Descriptor const& info, StreamReader& s);
			BerEnum(Element& e, Descriptor const& info, Binary const& b);

			virtual ~BerEnum() {}

			uint serializedSize() const;
			void serialize(StreamWriter& s) const;
		}; // class BerEnum
	} // namespace structure
} // namespace rpg2k

#endif
