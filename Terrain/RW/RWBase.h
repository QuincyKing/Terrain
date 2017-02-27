#pragma once

#include <string>

template<typename T>
class RWBase
{
public:
	RWBase() {}
	RWBase(std::string filename) :mFilename(filename) {}

	virtual void Execute() = 0;
	virtual void Execute(T, T, T) {};
	virtual void SetFilename(std::string) = 0;

public:
	std::string mFilename;
};