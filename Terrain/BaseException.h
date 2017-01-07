#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <stdexcept>
#include <string>
#include <iostream>

#pragma warning( disable : 4290 ) 
#define LOCATION Location(__FILE__, __LINE__)

inline std::string Location(std::string file, int lineNo) {

	char buf[64];
	sprintf_s(buf, " line no. %d", lineNo);

	return file + buf;
}

class BaseException : public std::runtime_error
{
public:
	BaseException() throw() : runtime_error("error") {}
	BaseException(std::string message) throw() : runtime_error(message) {}
	BaseException(std::string message, std::string location) throw() : std::runtime_error(message + location) {}
	void Log(std::ostream& os) throw() { os << what() << std::endl; }
};
#endif
