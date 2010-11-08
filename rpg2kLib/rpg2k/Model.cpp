#include "Debug.hpp"
#include "Model.hpp"
#include "Stream.hpp"
#include "define/Define.hpp"

#include <algorithm>
#include <sstream>
#include <stack>

#include <cctype>
#include <cstdio>

using rpg2k::structure::Descriptor;
using rpg2k::structure::Element;


namespace rpg2k
{
	namespace model
	{
		bool fileExists(SystemString const& fileName)
		{
			FILE* fp = std::fopen( fileName.c_str(), "rb");

			if(fp == NULL) return false;
			else { std::fclose(fp); return true; }
		}

		Base::Base(SystemString const& dir)
		: fileDir_(dir)
		{
			checkExists();
		}
		Base::Base(SystemString const& dir, SystemString const& name)
		: fileDir_(dir), fileName_(name)
		{
			checkExists();
		}

		void Base::reset()
		{
			boost::ptr_vector<Descriptor> const& info = descriptor();
			for(unsigned int i = 0; i < info.size(); i++) {
				data_.push_back( std::auto_ptr<Element>( new Element( info[i] ) ) );
			}
		}

		Element& Base::operator [](unsigned index)
		{
			return data_.front().toArray1D()[index];
		}
		Element const& Base::operator [](unsigned index) const
		{
			return data_.front().toArray1D()[index];
		}

		boost::ptr_vector<Descriptor> const& Base::descriptor() const
		{
			return DefineLoader::instance().get( header() );
		}

		void Base::checkExists()
		{
			exists_ = fileExists( fullPath() );
		}

		void Base::load()
		{
			if( fileName_.empty() ) fileName_ = defaultName();
			rpg2k_assert( exists() );

			structure::StreamReader s( fullPath() );

			{
				bool res = s.checkHeader( header() );
				if( this->header() == std::string("LcfMapTree") ) {
					// TODO
				} else rpg2k_assert(res);
			}

			boost::ptr_vector<Descriptor> const& info = descriptor();
			for(unsigned int i = 0; i < info.size(); i++) {
				data_.push_back( std::auto_ptr<Element>( new Element(info[i], s) ) );
			}

			rpg2k_assert( s.eof() );

			loadImpl();
		}
		void Base::saveAs(SystemString const& filename)
		{
			exists_ = true;
			saveImpl();
			structure::StreamWriter s(filename);
			serialize(s);
		}
		void Base::serialize(structure::StreamWriter& s)
		{
			s.setHeader( header() );
			for(boost::ptr_vector<Element>::const_iterator it = data_.begin(); it < data_.end(); ++it) {
				it->serialize(s);
			}
		}

		DefineLoader::DefineLoader()
		{
			isArray_.insert("Music");
			isArray_.insert("Sound");
			isArray_.insert("EventState");

			isArray_.insert("Array1D");
			isArray_.insert("Array2D");

			#define PP_insert(arg) \
				defineText_.insert( std::make_pair( RPG2kString( #arg ), define::arg ) )
			PP_insert(EventState);
			PP_insert(LcfDataBase);
			PP_insert(LcfMapTree);
			PP_insert(LcfMapUnit);
			PP_insert(LcfSaveData);
			PP_insert(Music);
			PP_insert(Sound);
			#undef PP_insert
		}

		DefineLoader& DefineLoader::instance()
		{
			static DefineLoader theDefineLoader;
			return theDefineLoader;
		}

		boost::ptr_vector<Descriptor> const& DefineLoader::get(RPG2kString const& name)
		{
			DefineBuffer::const_iterator it = defineBuff_.find(name);
			if( it == defineBuff_.end() ) {
				boost::ptr_vector<Descriptor>& ret = defineBuff_[name];
				load(ret, name);
				return ret;
			} else return it->second;
		}
		structure::ArrayDefine DefineLoader::arrayDefine(RPG2kString const& name)
		{
			return get(name).front().arrayDefine();
		}

		void DefineLoader::load(boost::ptr_vector<structure::Descriptor>& dst, RPG2kString const& name)
		{
			DefineText::const_iterator it = defineText_.find(name);
			rpg2k_assert( it != defineText_.end() );
			std::istringstream stream(it->second);
			std::deque<RPG2kString> token;
			toToken(token, stream);
			parse(dst, token);
		}

		// parser for define Stream

		#define nextToken(curType) prev = curType; continue

		void DefineLoader::parse(boost::ptr_vector<structure::Descriptor>& dst
		, std::deque<RPG2kString> const& token)
		{
			bool blockComment = false;
			unsigned int streamComment = 0, line = 1, col = 0;
			RPG2kString typeName;

			enum TokenType
			{
				OPEN_INDEX = 0, INDEX, CLOSE_INDEX1, CLOSE_INDEX2,
				TYPE, NAME, EQUALS, DEFAULT,
				OPEN_STRUCT, CLOSE_STRUCT,
				EXP_END,
			} prev = EXP_END;

			using structure::ArrayDefineType;
			std::stack< ArrayDefineType* > nest;

			// if success continue else error
			for(std::deque<RPG2kString>::const_iterator it = token.begin(); it < token.end(); ++it) {
				if(*it == "\n") { blockComment = false; line++; continue;
				} else if(blockComment) { continue;
				} else if(streamComment) {
					if( (*it == "*") && ( *(++it) == "/" ) ) { streamComment--; }
					continue;
				} else if(*it == "/") {
					++it;
					if(*it == "*") { streamComment++; continue; }
					else if(*it == "/") { blockComment = true; continue; }
				} else if( nest.empty() ) switch(prev) {
					case TYPE: nextToken(NAME);
					case NAME:
						if(*it == ";") {
							dst.push_back( std::auto_ptr<Descriptor>( new Descriptor(typeName) ) );
							nextToken(EXP_END);
						} else if( isArray(typeName) && (*it == "{") ) {
							structure::ArrayDefinePointer arrayDef(new ArrayDefineType);
							ArrayDefineType* p = arrayDef.get();

							dst.push_back(
								std::auto_ptr<Descriptor>( new Descriptor(typeName, arrayDef) ) );
							nest.push(p);

							nextToken(OPEN_STRUCT);
						}
						break;
					case CLOSE_STRUCT:
						if(*it == ";") { nextToken(EXP_END); } else break;
					case EXP_END:
						typeName = *it;
						// cout << typeName << endl;
						nextToken(TYPE);
					default: break;
				} else switch(prev) {
					case OPEN_INDEX: {
						std::istringstream ss(*it);
						ss >> col;
						rpg2k_assert( nest.top()->find(col) == nest.top()->end() );
						nextToken(INDEX);
					}
					case INDEX:
						if(*it == "]") { nextToken(CLOSE_INDEX1); } else break;
					case CLOSE_INDEX1:
						if(*it == ":") { nextToken(CLOSE_INDEX2); } else break;
					case CLOSE_INDEX2:
						typeName = *it;
						nextToken(TYPE);
					case TYPE:
						nextToken(NAME);
					case NAME:
						if(*it == "=") { nextToken(EQUALS);
						} else if(*it == ";") {
							if( isArray(typeName) ) {
								boost::ptr_vector<Descriptor> const& def = get(typeName);
								nest.top()->insert( col,
									std::auto_ptr<Descriptor>( new Descriptor(
										structure::ElementType::instance().toString( def[0].type() ),
										structure::ArrayDefinePointer( new ArrayDefineType( def[0].arrayDefine() ) ) ) ) );
							} else nest.top()->insert( col, std::auto_ptr<Descriptor>( new Descriptor(typeName) ) );

							nextToken(EXP_END);
						} else if( (*it == "{") && isArray(typeName) ) {
							structure::ArrayDefinePointer arrayDef(new ArrayDefineType);
							ArrayDefineType* p = arrayDef.get();

							nest.top()->insert( col, std::auto_ptr<Descriptor>( new  Descriptor(typeName, arrayDef) ) );
							nest.push(p);

							nextToken(OPEN_STRUCT);
						} else break;
					case EQUALS:
						if( isArray(typeName) )
							nest.top()->insert( col,
								std::auto_ptr<Descriptor>( new Descriptor( typeName,
								structure::ArrayDefinePointer( new ArrayDefineType( arrayDefine(*it) ) ) ) ) );
						else nest.top()->insert( col, std::auto_ptr<Descriptor>( new Descriptor(typeName, *it) ) );
						nextToken(DEFAULT);
					case DEFAULT:
						if(*it == ";") { nextToken(EXP_END); } else break;
					case OPEN_STRUCT:
						if(*it == "[") { nextToken(OPEN_INDEX); }
						else if(*it == "}") { nest.pop(); nextToken(CLOSE_STRUCT); }
						else break;
					case CLOSE_STRUCT:
						if(*it == ";") { nextToken(EXP_END); } else break;
					case EXP_END:
						if(*it == "[") { nextToken(OPEN_INDEX); }
						else if(*it == "}") { nest.pop(); nextToken(CLOSE_STRUCT); }
						else break;
					default: break;
				}

				cerr << "Error at line: " << line << endl;
				rpg2k_assert(false);
			}

			rpg2k_assert(streamComment == 0);
		}

		#undef nextToken

		void DefineLoader::toToken(std::deque<RPG2kString>& token, std::istream& stream)
		{
			RPG2kString strBuf;

			while(true) {
				int buf = stream.get();

				if(buf == -1) {
					if( !strBuf.empty() ) token.push_back(strBuf);
					break;
				} else if(buf == '\"') {
					if( !strBuf.empty() && (*strBuf.begin() == '\"') ) {
						strBuf.push_back(buf);
						token.push_back(strBuf);
						strBuf.clear();
					} else {
						if( !strBuf.empty() ) {
							token.push_back(strBuf);
							strBuf.clear();
						}
						strBuf.push_back(buf);
					}
				} else if( !strBuf.empty() && (*strBuf.begin() == '\"') ) { strBuf.push_back(buf);
				} else if( std::isalpha(buf) || std::isdigit(buf) || (buf == '_') ) {
					strBuf.push_back(buf);
				} else {
					if( !strBuf.empty() ) {
						token.push_back(strBuf);
						strBuf.clear();
					}
					switch(buf) {
						case ' ': case '\t': case '\r': break;
						default:
							token.push_back( RPG2kString(1, buf) );
							break;
					}
				}
			}
		}
	} // namespace model
} // namespace rpg2k
