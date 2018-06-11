#pragma once

#include <string>
#include <iostream>
#include "RWBase.h"
#include "ReaderBase.h"
#include "WriteBase.h"
#include "WriteData.h"
#include "WriteInfo.h"

template<typename T>
class RWBuilder
{
public:
	RWBuilder(WriteBase<T> *writer);
	RWBuilder(ReaderBase<T> *reader, std::string filename);
	RWBuilder(WriteBase<T> *writer, std::string filename);
	~RWBuilder();

	void			Execute(T, T, T);
	void			Execute();
	RWBase<T>*		GetRWBase();

private:
	RWBase<T>*		mBuilder;
};

#pragma region й╣ож
template<typename T>
RWBuilder<T>::RWBuilder(WriteBase<T> *writer)
{
	mBuilder = writer;
}

template<typename T>
RWBuilder<T>::RWBuilder(ReaderBase<T> *reader, std::string filename)
{
	mBuilder = reader;
	mBuilder->SetFilename(filename);
}

template<typename T>
RWBuilder<T>::RWBuilder(WriteBase<T> *writer, std::string filename)
{
	mBuilder = writer;
	mBuilder->SetFilename(filename);
}

template<typename T>
RWBuilder<T>::~RWBuilder()
{
	delete mBuilder;
}

template<typename T>
void RWBuilder<T>::Execute()
{
	mBuilder->Execute();
}

template<typename T>
void RWBuilder<T>::Execute(T x, T y, T z)
{
	mBuilder->Execute(x, y ,z);
}

template<typename T>
RWBase<T>* RWBuilder<T>::GetRWBase()
{
	return mBuilder;
}

#pragma endregion