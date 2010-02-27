#include <iomanip>
#include <stack>

#include "Model.hpp"

using namespace rpg2kLib::structure;


namespace rpg2kLib
{
	namespace model
	{

using namespace rpg2kLib;

const string DefineLoader::DEFINE_DIR = "define";

BerEnum::BerEnum(Element& e, const Descriptor& info, StreamReader& s)
{
	init(s);
}
BerEnum::BerEnum(Element& e, const Descriptor& info, Binary& b)
{
	StreamReader s(b);
	init(s);
}

void BerEnum::init(StreamReader& s)
{
	uint length = s.getBER();

	resize(length+1);
	for(uint i = 0; i <= length; i++) (*this)[i] = s.getBER();
}

const Binary& BerEnum::toBinary()
{
	BIN_DATA = static_cast< std::vector< uint > >(*this);
	return BIN_DATA;
}

CharSetDir EventState::charSetDir()
{
	return (CharSetDir) (*this)[30 + talkDir()].get_int();
}

Base::Base(string dir) : FILE_DIR(dir), FILE_NAME(defaultName())
{
	checkExists();
}
Base::Base(string dir, string name) : FILE_DIR(dir), FILE_NAME(name)
{
	checkExists();
}
Base::~Base()
{
}

Element& Base::operator [](uint index)
{
	//clog << getHeader() << "(id = " << dec << ID << "): ";

	return getData()[0].getArray1D()[index];
}
const Element& Base::operator [](uint index) const
{
	//clog << getHeader() << "(id = " << dec << ID << "): ";

	return getData()[0].getArray1D()[index];
}

Map< uint, Descriptor >& Base::getDescriptor() const
{
	return DefineLoader::getInstance().get( getHeader() );
}

void Base::checkExists()
{
	FILE* fp = fopen( (FILE_DIR + PATH_SEPR + FILE_NAME).c_str(), "rb");

	if(fp == NULL) EXISTS = false;
	else {
		fclose(fp);
		EXISTS = true;
	}
}

void Base::load()
{
	if(FILE_NAME == "") FILE_NAME = defaultName();

	if( !exists() )
		throw "\"" + getFileName() + "\"" + " not found.";

	StreamReader s( getFileName() );
	if( !s.checkHeader( getHeader() ) ) throw "Stream Invalid header.";

	Map< uint, Descriptor >& info = getDescriptor();

	std::cerr << "\n/* " << getHeader() << " */\n";

	for(uint i = 0; i < info.size(); i++) DATA.add( i, *new Element(info[i], s) );

	std::cerr << "/* " << getHeader() << " */\n\n";

	if( s.eof() ) return;
	else {
		std::cerr << s.name() << " didn't end correctly. tell(): "
			<< s.tell() << ";\n";

		std::cerr.fill('0');
		while( !s.eof() ) {
			std::cerr << std::hex << std::setw(2) << ( s.read() & 0xff ) << " ";
		}
		std::cerr.fill(' ');
		std::cerr << std::endl;

		throw "Base::open(): Didn`t end correctly.";
	}
}
void Base::save()
{
	StreamWriter s( getFileName() );

	if(!EXISTS) EXISTS = true;

	s.setHeader( getHeader() );
	for(uint i = 0, length = getDescriptor().size(); i < length; i++) {
		s.write( DATA[i].toBinary() );
	}
}

DefineLoader::DefineLoader()
{
	IS_ARRAY.insert( std::map< std::string, bool >::value_type("Array1D", true) );
	IS_ARRAY.insert( std::map< std::string, bool >::value_type("Array2D", true) );
}
DefineLoader::~DefineLoader()
{
	// clog << "Disposing DefineLoader." << endl;
}

DefineLoader& DefineLoader::getInstance()
{
	static DefineLoader theDefineLoader;
	return theDefineLoader;
}

Map< uint, Descriptor >& DefineLoader::get(string name)
{
	return DEFINE_BUFF.exists(name) ? DEFINE_BUFF[name] : load(name);
}
ArrayDefine DefineLoader::getArrayDefine(string name)
{
	return get(name).get(0).getArrayDefine();
}

Map< uint, Descriptor >& DefineLoader::load(std::string name)
{
	Map< uint, Descriptor >& ret = *new Map< uint, Descriptor >();
	std::list< std::string > token;

	std::ifstream defines( (DEFINE_DIR + PATH_SEPR + name).c_str() );
	if( defines.good() ) {
		std::cout << "Define Stream open success. name: " << name << ";" << std::endl;
	} else throw "Define Stream open failed. name: " + name + ";";

	toToken(token, defines);
	parse(ret, token);
	DEFINE_BUFF.add(name, ret);

	return ret;
}

// parser for define Stream

#define nextToken(curType) pre = curType; continue

void DefineLoader::parse(
	Map< uint, Descriptor >& res,
	const std::list< std::string >& token
) {
	bool blockComment = false;
	uint streamComment = 0, line = 1, col = 0, root = 0;
	std::string typeName;

	TokenType pre = EXP_END;

	std::stack< Map< uint, Descriptor >* > nest;

// if success continue else error
	for(std::list< std::string >::const_iterator it = token.begin(); it != token.end(); ++it) {
/*
		if(*it == "\n") clog << "\t\t\tline: " << line << endl;
		else clog << "(" << pre << ")" << *it << " ";
 */

		if(*it == "\n") { blockComment = false; line++; continue;
		} else if(blockComment) { continue;
		} else if(streamComment) {
			if( (*it == "*") && ( *(++it) == "/" ) ) { streamComment--; }
			continue;
		} else if(*it == "/") {
			if( *(++it) == "*" ) {
				streamComment++; continue;
			} else if(*it == "/") {
				blockComment = true; continue;
			}
		} else if( nest.empty() ) switch(pre) {
			case TYPE: nextToken(NAME);
			case NAME:
				if(*it == ";") {
					res.add( root++, *new Descriptor(typeName) );
					nextToken(EXP_END);
				} else if( isArray(typeName) && (*it == "{") ) {
					Map< uint, Descriptor >* arrayDef = new Map< uint, Descriptor >();

					res.add( root++, *new Descriptor(typeName, *arrayDef) );
					nest.push(arrayDef);

					nextToken(O_STRUCT);
				}
				break;
			case C_STRUCT:
				if(*it == ";") { nextToken(EXP_END); } else break;
			case EXP_END:
				typeName = *it; nextToken(TYPE);
			default: break;
		} else switch(pre) {
			case O_INDEX:
				col = toNumber(*it);
				if( nest.top()->exists(col) ) {
					std::cerr << "Defplicated index. ; indexNum = " << col
						<< "; at line : " << line << ";" << std::endl;
					throw "Define Stream open error.";
				} else { nextToken(INDEX); }
			case INDEX:
				if(*it == "]") { nextToken(C_INDEX1); } else break;
			case C_INDEX1:
				if(*it == ":") { nextToken(C_INDEX2); } else break;
			case C_INDEX2:
				typeName = *it; nextToken(TYPE);
			case TYPE:
				nextToken(NAME);
			case NAME:
				if(*it == "=") { nextToken(EQUALS);
				} else if(*it == ";") {
					try {
						Map< uint, Descriptor >* arrayDef =
							new Map< uint, Descriptor >( getArrayDefine(typeName) );
						nest.top()->add( col, *new Descriptor(typeName, *arrayDef) );
					} catch(...) {
						nest.top()->add( col, *new Descriptor(typeName) );
					}
					nextToken(EXP_END);
				} else if( (*it == "{") && isArray(typeName) ) {
					Map< uint, Descriptor >* arrayDef = new Map< uint, Descriptor >();

					nest.top()->add( col, *new Descriptor(typeName, *arrayDef) );
					nest.push(arrayDef);

					nextToken(O_STRUCT);
				} else break;
			case EQUALS:
				if( isArray(typeName) ) {
					Map< uint, Descriptor >* arrayDef =
						new Map< uint, Descriptor >( getArrayDefine(*it) );
					nest.top()->add( col, *new Descriptor(typeName, *arrayDef) );
				} else {
					nest.top()->add( col, *new Descriptor(typeName, *it) );
				}
				nextToken(DEFAULT);
			case DEFAULT:
				if(*it == ";") { nextToken(EXP_END); } else break;
			case O_STRUCT:
				if(*it == "[") { nextToken(O_INDEX);
				} else if(*it == "}") { nest.pop(); nextToken(C_STRUCT);
				} else break;
			case C_STRUCT:
				if(*it == ";") { nextToken(EXP_END); } else break;
			case EXP_END:
				if(*it == "[") { nextToken(O_INDEX);
				} else if(*it == "}") { nest.pop(); nextToken(C_STRUCT);
				} else break;
			default: break;
		}

		std::cerr << "Syntax error at line : " << line
			<< "; token = \"" << *it << "\";" << std::endl;
		throw "Define Stream open error.";
	}

	if(streamComment != 0) {
		std::cerr << "Stream comment didin't close correctly." << std::endl;
		throw "Define Stream opesn error.";
	}
}

#undef nextToken

int DefineLoader::toNumber(string str)
{
	const char* data = str.c_str();
	uint length = str.length();
	int ret = 0;

	if(length < 1) throw "Cannot covert to number: " + str;
	else if( (length >= 2) && (*data == '0') && (*(data+1) == 'x') )
			for(uint i = 2; i < length; i++)
	{
		if( ('0' <= *data) && (*data <= '9') )
			ret = ret*0x10 + *data-'0';
		else if( ('a' <= *data) && (*data <= 'f') )
			ret = ret*0x10 + *data-'a' + 10;
		else if( ('A' <= *data) && (*data <= 'F') )
			ret = ret*0x10 + *data-'A' + 10;
		else throw "Cannot covert to number: " + str;

		data++;
	} else for(uint i = 0; i < length; i++) {
		if( ('0' <= *data) && (*data <= '9') ) ret = ret*10 + *data-'0';
		else throw "Failed coverting to number: \"" + str + "\"";

		data++;
	}

	return ret;
}
bool DefineLoader::toBool(string str)
{
	if(str == "true") return true;
	else if(str == "false") return false;
	else throw "Failed coverting to bool: \"" + str + "\"";
}

void DefineLoader::toToken(std::list< std::string >& token, std::ifstream& stream)
{
	char buf, str[1024];
	uint strCount = 0;

	while(true) {
		buf = stream.get();
		if(buf == -1) {
			if(strCount != 0) token.push_back(string(str, strCount));
			return;
		} else if(buf == '\"') {
			if( (strCount != 0) && (str[0] == '\"') ) {
				str[strCount++] = buf;
				token.push_back(string(str, strCount));
				strCount = 0;
			} else {
				if(strCount != 0) {
					token.push_back(string(str, strCount));
					strCount = 0;
				}
				str[strCount++] = buf;
			}
		} else if( (strCount != 0) && (str[0] == '\"') ) { str[strCount++] = buf;
		} else if(
				( ('0' <= buf) && (buf <= '9') ) ||
				( ('a' <= buf) && (buf <= 'z') ) ||
				( ('A' <= buf) && (buf <= 'Z') ) ||
				(buf == '_')
		) {
			str[strCount++] = buf;
		} else {
			if(strCount != 0) {
				token.push_back(string(str, strCount));
				strCount = 0;
			}
			switch(buf) {
				case ' ': case '\t': case '\r': break;
				default: token.push_back(string(&buf, 1));
					break;
			}
		}
	}
}

	}; // namespace model
}; // namespace rpg2kLib
