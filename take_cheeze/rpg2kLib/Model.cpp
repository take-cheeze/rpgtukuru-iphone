#include <stack>

#include "Model.hpp"

using namespace rpg2kLib;
using namespace rpg2kLib::model;

const string DefineLoader::DEFINE_DIR = "define";

BerEnum::BerEnum(Element& e, const Descriptor& info, Stream& s)
{
	init(s);
}
BerEnum::BerEnum(Element& e, const Descriptor& info, Binary& b)
{
	Stream s(b);
	init(s);
}

void BerEnum::init(Stream& s)
{
	uint length = s.getBER();

	resize(length+1);
	for(uint i = 0; i <= length; i++) (*this)[i] = s.getBER();
}

const Binary& BerEnum::toBinary()
{
	BIN_DATA = static_cast< vector< uint > >(*this);
	return BIN_DATA;
}

CharSetDir EventState::charSetDir()
{
	return (CharSetDir) (int)(*this)[30 + talkDir()];
}

Base::Base(string dir, string name) : FILE_DIR(dir), FILE_NAME(name)
{
	if(FILE_NAME == "") FILE_NAME = defaultName();

	checkExists();
}
Base::~Base()
{
}

Element& Base::operator [](uint index)
{
	//clog << getHeader() << "(id = " << dec << ID << "): ";

	return ( (Array1D&)getData()[0] )[index];
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

void Base::open()
{
	if(FILE_NAME == "") FILE_NAME = defaultName();

	if( !exists() )
		throw "\"" + FILE_DIR + PATH_SEPR + FILE_NAME + "\"" + " not found.";

	getStream().open(FILE_DIR + PATH_SEPR + FILE_NAME);
	if( !getStream().checkHeader( getHeader() ) ) throw "Stream Invalid header.";

	Map< uint, Descriptor >& info = getDescriptor();

	cerr << endl << "/* " << getHeader() << " */" << endl;

	for(uint i = 0; i < info.size(); i++)
		DATA.add( i, *new Element( info[i], getStream() ) );

	cerr << "/* " << getHeader() << " */" << endl << endl;

	if( getStream().eof() ) return;
	else {
		cerr << getStream().name() << " didn't end correctly. tell(): "
			<< getStream().tell() << ";" << endl;

		cerr.fill('0');
		while( !getStream().eof() ) {
			cerr.width(2);
			cerr << hex << ( getStream().read() & 0xff ) << " ";
		}
		cerr.fill(' ');
		cerr << endl;

		throw "Base::open(): Didn`t end correctly.";
	}
}
void Base::create()
{
	if(FILE_NAME == "") FILE_NAME = defaultName();

	getStream().open(FILE_DIR + PATH_SEPR + FILE_NAME);
}

void Base::save()
{
	if(!EXISTS) {
		create();
		EXISTS = true;
	}

	getStream().setHeader( getHeader() );
	for(uint i = 0, length = getDescriptor().size(); i < length; i++) {
		getStream().write( DATA[i].toBinary() );
	}
}

DefineLoader::DefineLoader()
{
	IS_ARRAY.insert( map< string, bool >::value_type("Array1D", true) );
	IS_ARRAY.insert( map< string, bool >::value_type("Array2D", true) );
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

Map< uint, Descriptor >& DefineLoader::load(string name)
{
	Map< uint, Descriptor >& ret = *new Map< uint, Descriptor >();
	list< string > token;

	ifstream defines( (DEFINE_DIR + PATH_SEPR + name).c_str() );
	if( defines.good() ) {
		cout << "Define Stream open success. name: " << name << ";" << endl;
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
	const list< string >& token
) {
	bool blockComment = false;
	uint streamComment = 0, line = 1, col = 0, root = 0;
	string typeName;

	TokenType pre = EXP_END;

	stack< Map< uint, Descriptor >* > nest;

// if success continue else error
	for(list< string >::const_iterator it = token.begin(); it != token.end(); ++it) {
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
					cerr << "Defplicated index. ; indexNum = " << col
						<< "; at line : " << line << ";" << endl;
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

		cerr << "Syntax error at line : " << line
			<< "; token = \"" << *it << "\";" << endl;
		throw "Define Stream open error.";
	}

	if(streamComment != 0) {
		cerr << "Stream comment didin't close correctly." << endl;
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

void DefineLoader::toToken(list< string >& token, ifstream& stream)
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
