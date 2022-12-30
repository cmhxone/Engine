#ifndef _ENGINE_INIREADER_HEADER_
#define _ENGINE_INIREADER_HEADER_

#include <ini.h>
#include <INIReader.h>

#include "../Prototype/Singleton.hpp"

class IniReader : public Singleton<IniReader>
{
public:
	IniReader();
	~IniReader();

	/**
	* get actual INIReader
	*/
	const INIReader getReader() { return _reader;  }

protected:

private:
	INIReader _reader = INIReader("./config.ini");
};

#endif // !_ENGINE_INIREADER_HEADER_
