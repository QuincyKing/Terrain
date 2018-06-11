#ifndef WriteBase_h__
#define WriteBase_h__
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "RWBase.h"

#pragma region WriteBase

template<typename T>
class WriteBase : public RWBase<T>
{
public:
	WriteBase():mOut() {}
	WriteBase(std::string filename):mOut(filename.c_str(), std::ofstream::ate) {}
	virtual void Execute(T, T, T) {};
	virtual void SetFilename(std::string filename) { mFilename = filename; }
	void Execute() {};

protected:
	std::ofstream mOut;
};

#pragma endregion

#endif