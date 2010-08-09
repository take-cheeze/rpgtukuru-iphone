#include "Debug.hpp"
#include "Model.hpp"
#include "define/Define.hpp"

#include <algorithm>
#include <stack>

#include <cctype>
#include <cstdio>


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
		: fileDir_(dir), fileName_( defaultName() )
		{
			checkExists();
		}
		Base::Base(SystemString const& dir, SystemString const& name)
		: fileDir_(dir), fileName_(name)
		{
			checkExists();
		}
		Base::~Base()
		{
		}

		void Base::reset()
		{
			std::deque< DescriptorPointer > const& info = getDescriptor();
			data_.resize( info.size() );
			for(unsigned int i = 0; i < info.size(); i++) {
				rpg2k_assert( info[i].get() );
				data_[i] = ElementPointer( structure::Element::create( *(info[i]) ).release() );
			}
		}

		structure::Element& Base::operator [](uint index)
		{
			return data_.front()->getArray1D()[index];
		}
		structure::Element const& Base::operator [](uint index) const
		{
			return data_.front()->getArray1D()[index];
		}

		std::deque< DescriptorPointer > const& Base::getDescriptor() const
		{
			return DefineLoader::instance().get( getHeader() );
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
			bool res = s.checkHeader( getHeader() ); rpg2k_assert(res);

			std::deque< DescriptorPointer > const& info = getDescriptor();
			data_.resize( info.size() );
			for(unsigned int i = 0; i < info.size(); i++) {
				rpg2k_assert( info[i].get() );
				data_[i].reset( structure::Element::create(*(info[i]), s).release() );
			}

			rpg2k_assert( s.eof() );
/*
				cerr << s.name() << " didn't end correctly. tell(): " << s.tell() << ";" << endl;

				cerr << std::setfill('0');
				while( !s.eof() ) {
					cerr << std::hex << std::setw(2) << ( s.read() & 0xff ) << " ";
				}
				cerr << std::setfill(' ') << endl;

				throw std::runtime_error("Base::open(): Didn`t end correctly.");
 */
		}
		void Base::save()
		{
			structure::StreamWriter s( fullPath() );

			if(!exists_) exists_ = true;

			s.setHeader( getHeader() );
			for(std::deque< ElementPointer >::const_iterator it = data_.begin(); it < data_.end(); ++it) {
				(*it)->serialize(s);
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
		DefineLoader::~DefineLoader()
		{
		}

		DefineLoader& DefineLoader::instance()
		{
			static DefineLoader theDefineLoader;
			return theDefineLoader;
		}

		std::deque< DescriptorPointer > const& DefineLoader::get(RPG2kString const& name)
		{
			if( defineBuff_.find(name) == defineBuff_.end() ) {
				bool res = defineBuff_.insert( std::make_pair( name, load(name) ) ).second; rpg2k_assert(res);
			}
			return defineBuff_.find(name)->second;
		}
		structure::ArrayDefine DefineLoader::getArrayDefine(RPG2kString const& name)
		{
			return get(name).front()->getArrayDefine();
		}

		std::deque< DescriptorPointer > DefineLoader::load(RPG2kString const& name)
		{
			std::deque< RPG2kString > token;

			rpg2k_assert( defineText_.find(name) != defineText_.end() );

			std::istringstream stream(defineText_.find(name)->second);
			toToken(token, stream);
			return parse(token);
		}

		// parser for define Stream

		#define nextToken(curType) prev = curType; continue

		std::deque< DescriptorPointer >
		DefineLoader::parse(std::deque< RPG2kString > const& token)
		{
			std::deque< DescriptorPointer > ret;

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

			std::stack< ArrayDefineIntern* > nest;

			// if success continue else error
			for(std::deque< RPG2kString >::const_iterator it = token.begin(); it < token.end(); ++it) {
		/*
				if(*it == "\n") clog << "\t\t\tline: " << line << endl;
				else clog << "(" << prev << ")" << *it << " ";
		 */

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
							ret.push_back( DescriptorPointer( structure::Descriptor::create(typeName).release() ) );
							nextToken(EXP_END);
						} else if( isArray(typeName) && (*it == "{") ) {
							structure::ArrayDefinePointer arrayDef(new ArrayDefineIntern);
							ArrayDefineIntern* p = arrayDef.get();

							ret.push_back( DescriptorPointer( structure::Descriptor::create(typeName, arrayDef).release() ) );
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
						if( nest.top()->exists(col) ) {
							cerr << "Defplicated index. ; indexNum = " << col
								<< "; at line : " << line << ";" << endl;
							rpg2k_assert(false);
						} else { nextToken(INDEX); }
					}
					case INDEX:
						if(*it == "]") { nextToken(CLOSE_INDEX1); } else break;
					case CLOSE_INDEX1:
						if(*it == ":") { nextToken(CLOSE_INDEX2); } else break;
					case CLOSE_INDEX2:
						typeName = *it;
						// cout << typeName << endl;
						nextToken(TYPE);
					case TYPE:
						nextToken(NAME);
					case NAME:
						if(*it == "=") { nextToken(EQUALS);
						} else if(*it == ";") {
							if( isArray(typeName) ) nest.top()->addPointer( col,
								structure::Descriptor::create( typeName,
								structure::ArrayDefinePointer( new ArrayDefineIntern( getArrayDefine(typeName) ) ) ) );
							else nest.top()->addPointer( col, structure::Descriptor::create(typeName) );

							nextToken(EXP_END);
						} else if( (*it == "{") && isArray(typeName) ) {
							structure::ArrayDefinePointer arrayDef(new ArrayDefineIntern);
							ArrayDefineIntern* p = arrayDef.get();

							nest.top()->addPointer(col, structure::Descriptor::create(typeName, arrayDef));
							nest.push(p);

							nextToken(OPEN_STRUCT);
						} else break;
					case EQUALS:
						if( isArray(typeName) )
							nest.top()->addPointer( col,
								structure::Descriptor::create( typeName,
								structure::ArrayDefinePointer( new ArrayDefineIntern( getArrayDefine(*it) ) ) ) );
						else nest.top()->addPointer(col, structure::Descriptor::create(typeName, *it));
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

				cerr << "Syntax error at line : " << line
					<< "; token = \"" << *it << "\";" << endl;
				rpg2k_assert(false);
			}

			rpg2k_assert(streamComment == 0);

			return ret;
		}

		#undef nextToken

		void DefineLoader::toToken(std::deque< RPG2kString >& token, std::istream& stream)
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
