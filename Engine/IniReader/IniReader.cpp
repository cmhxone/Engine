#include "IniReader.h"

#include <iostream>
#include <format>

IniReader::IniReader()
{
	if (_reader.ParseError() != 0)
	{
		throw std::runtime_error(std::format("failed to parse ini file"));
	}
}

IniReader::~IniReader()
{
}