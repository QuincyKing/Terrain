#ifndef WriteBase_h__
#define WriteBase_h__
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

template<typename T>
class WriteBase
{
public:
	WriteBase(std::string filename):m_out(filename.c_str(), std::ofstream::ate) {}
	void Write(T, T, T);

protected:
	std::string m_filename;
	std::ofstream m_out;
};
#endif