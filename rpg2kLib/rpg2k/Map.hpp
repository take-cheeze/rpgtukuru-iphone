#ifndef _INC__RPG2K__MAP_HPP
#define _INC__RPG2K__MAP_HPP

#include "Define.hpp"

#include <map>
#include <utility>


namespace rpg2k
{
	namespace structure
	{
		template< typename Key, typename T >
		class Map
		{
		public:
			typedef std::auto_ptr<T> Pointer;
		private:
			std::multimap< Key, T* > data_;
			typedef typename std::multimap< Key, T* >::const_iterator iterator;
			typedef typename std::multimap< Key, T* >::const_reverse_iterator reverse_iterator;
		public:
			class Iterator
			{
				friend class Map< Key, T >;
			private:
				Map< Key, T > const& owner_;
				iterator it_;
			protected:
				Iterator();
				Iterator(Map< Key, T > const& owner, iterator it)
				: owner_(owner), it_(it)
				{
				}
			public:
				bool operator !=(Iterator const& it) const { return this->it_ != it.it_; }
				bool operator ==(Iterator const& it) const { return this->it_ == it.it_; }

				Key const& first() const { return it_->first; }
				T& second() { return *(it_->second); }

				Iterator& operator ++() { ++it_; return *this; }
				Iterator& operator --() { --it_; return *this; }
			};
			class ReverseIterator
			{
				friend class Map< Key, T >;
			private:
				Map< Key, T > const& owner_;
				reverse_iterator it_;
			protected:
				ReverseIterator();
				ReverseIterator(Map< Key, T > const& owner, reverse_iterator it)
				: owner_(owner), it_(it)
				{
				}
			public:
				bool operator !=(ReverseIterator const& it) const { return this->it_ != it.it_; }
				bool operator ==(ReverseIterator const& it) const { return this->it_ == it.it_; }

				Key const& first() const { return it_->first; }
				T& second() { return *(it_->second); }

				ReverseIterator& operator ++() { ++it_; return *this; }
				ReverseIterator& operator --() { --it_; return *this; }
			};

			Map() : data_() {}
			Map(Map< Key, T > const& src) : data_()
			{
				for(iterator it = src.data_.begin(); it != src.data_.end(); ++it) {
					data_.insert( std::make_pair( it->first, T::copy( *(it->second) ).release() ) );
				}
			}

			Map< Key, T >& operator =(Map< Key, T > const& src)
			{
				clear();

				for(iterator it = src.data_.begin(); it != src.data_.end(); ++it) {
					data_.insert( std::make_pair( it->first, T::copy( *(it->second) ).release() ) );
				}

				return *this;
			}

			void clear()
			{
				for(iterator it = data_.begin(); it != data_.end(); ++it) delete it->second;
				data_.clear();
			}

			~Map() { clear(); }

			uint size() const { return data_.size(); }

			bool exists(Key const& key) const { return data_.find(key) != data_.end(); }

			T& get(Key const& key) const
			{
				rpg2k_assert( exists(key) );
				return *( data_.find(key)->second );
			}
			T& operator [](Key const& key) const
			{
				rpg2k_assert( exists(key) );
				return *( data_.find(key)->second );
			}

			// skips remove if doesn't exist
			void remove(Key const& key)
			{
				if( exists(key) ) {
					iterator it = data_.find(key);
					delete it->second;
					data_.erase(key);
				}
			}

			bool empty() const { return data_.empty(); }

			void addPointer(Key const& key, Pointer p)
			{
				data_.insert( std::make_pair( key, p.release() ) );
			}

			void add(Key const& key) { data_.insert( std::make_pair( key, new T() ) ); }
			template< typename Arg1 >
			void add(Key const& key, Arg1& a1)
			{
				data_.insert( std::make_pair( key, new T(a1) ) );
			}
			template< typename Arg1, typename Arg2 >
			void add(Key const& key, Arg1& a1, Arg2& a2)
			{
				data_.insert( std::make_pair( key, new T(a1, a2) ) );
			}
			template< typename Arg1, typename Arg2, typename Arg3 >
			void add(Key const& key, Arg1& a1, Arg2& a2, Arg3& a3)
			{
				data_.insert( std::make_pair( key, new T(a1, a2, a3) ) );
			}
			template< typename Arg1, typename Arg2, typename Arg3, typename Arg4 >
			void add(Key const& key, Arg1& a1, Arg2& a2, Arg3& a3, Arg4& a4)
			{
				data_.insert( std::make_pair( key, new T(a1, a2, a3, a4) ) );
			}
			template< typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
			void add(Key const& key, Arg1& a1, Arg2& a2, Arg3& a3, Arg4& a4, Arg5& a5)
			{
				data_.insert( std::make_pair( key, new T(a1, a2, a3, a4, a5) ) );
			}

			Iterator begin() const { return Iterator( *this, data_.begin() ); }
			Iterator end  () const { return Iterator( *this, data_.end  () ); }

			ReverseIterator rbegin() const { return ReverseIterator( *this, data_.rbegin() ); }
			ReverseIterator rend  () const { return ReverseIterator( *this, data_.rend  () ); }
		}; // class Map

		class Descriptor;
		typedef Map< uint, Descriptor > const& ArrayDefine;
		typedef std::auto_ptr< Map< uint, Descriptor > > ArrayDefinePointer;
	} // namespace structure

} // namespace rpg2k

#endif // _INC__RPG2K__MAP_HPP
