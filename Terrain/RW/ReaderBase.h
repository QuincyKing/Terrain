#ifndef ReaderBase_h__
#define ReaderBase_h__

#include <string>
#include <vector>
#include <stdint.h>
#include "RWBase.h"
#include "../TerrainModel.h"
#include "../BaseData.h"
#include "WriteLog.h"
#include <fstream>
#include <memory>
#include <sstream>

#pragma region ReaderBase

template<typename T>
class ReaderBase : public RWBase<T>
{
public:
	ReaderBase():RWBase() {};
	ReaderBase(std::string filename):RWBase(filename) {}
	virtual void SetFilename(std::string filename) 
	{ 
		mFilename = filename;  
	}
	virtual T GetData() 
	{ 
		return  mData; 
	}

	virtual void Execute() {};

protected:
	T  mData;
};

#pragma endregion

#pragma region ReaderData

template<>
class ReaderBase<Vertices> : public RWBase<Vertices>
{
public:
	ReaderBase():RWBase() {}
	ReaderBase(std::string filename):RWBase(filename)  { }
	void SetFilename(std::string filename) 
	{ 
		mFilename = filename; 
	}
	Vertices GetData() 
	{ 
		return  mData; 
	}

	void Execute()
	{
		std::string line;
		std::ifstream in(mFilename.c_str());
		if (!in) return;
		double x, y, z;
		getline(in, line);
		while (line != "")
		{
			std::istringstream iss(line);
			iss >> x >> y >> z;
			mData.push(Point3D(x, y, z));
			getline(in, line);
		}
		return;
	}

protected:
	Vertices mData;
};
#pragma endregion

#pragma region ReaderInfo

template<>
class ReaderBase<Info> :public RWBase<Info>
{
public:
	ReaderBase() :RWBase() {}
	ReaderBase(std::string filename) :RWBase(filename)  { }
	void SetFilename(std::string filename) 
	{ 
		mFilename = filename; 
	}
	Info GetData() 
	{ 
		return  mData; 
	}

	bool HasData()
	{
		std::ifstream in(mFilename.c_str());
		string line;
		getline(in, line);
		if (!in || in.eof()) return false;
		return true;
	}

	void Execute()
	{
		std::string line;
		std::ifstream in(mFilename.c_str());
		if (!in) return;
		int countx, county, countz;
		while (!in.eof())
		{
			getline(in, line);
			std::istringstream iss(line);
			iss >> countx >> county >> countz;
			mData.Put(countx, county, countz);
		}
		return;
	}

protected:
	Info mData;
};

#pragma endregion 

#pragma region ReaderPara

template<>
class ReaderBase<Para> :public RWBase<Para>
{
public:
	ReaderBase() :RWBase() {}
	ReaderBase(std::string filename) :RWBase(filename)  { }
	virtual void SetFilename(std::string filename) 
	{ 
		mFilename = filename; 
	}
	virtual Para GetData() 
	{ 
		return  mData; 
	}

	void Execute()
	{
		std::vector<double> vecE, vecV, vecK;
		std::string line;
		double e, v, k;
		std::ifstream in(mFilename.c_str());
		if (!in) return;
		getline(in, line);
		std::istringstream issE(line);
		while (issE >> e)
		{
			vecE.push_back(e);
		}
		mData.PutE(vecE);
		if (in.eof())
		{
			shared_ptr<WriteLogging> wl = make_shared<WriteLogging>();
			wl->Execute(mFilename, " ERROR : data is not enough for V,K");
			return;
		}

		getline(in, line);
		std::istringstream issV(line);
		while (issV >> v)
		{
			vecV.push_back(v);
		}
		mData.PutV(vecV);
		if (in.eof())
		{
			shared_ptr<WriteLogging> wl = make_shared<WriteLogging>();
			wl->Execute(mFilename, " ERROR : data is not enough for V");
			return;
		}

		getline(in, line);
		std::istringstream issK(line);
		while (issK >> k)
		{
			vecK.push_back(k);
		}
		mData.PutK(vecK);
		return;
	}

protected:
	Para mData;
};

#pragma endregion

#pragma region ReaderTerrainData

template<>
class ReaderBase<std::vector<std::vector<Point3D> > > : public RWBase<std::vector<std::vector<Point3D> > >
{
public:
	ReaderBase() :RWBase() {}
	ReaderBase(std::string filename) :RWBase(filename)  { }
	virtual void SetFilename(std::string filename) 
	{ 
		mFilename = filename; 
	}
	virtual std::vector<std::vector<Point3D> > GetData() 
	{ 
		return  mData; 
	}

	void GetLevels(int &levels)
	{
		levels = mData.size();
	}

	virtual void Execute()
	{
		std::string line;
		std::vector<double> vecX;
		std::vector<double> vecY;
		std::ifstream in(mFilename.c_str());
		if (!in) return;
		double x, y, z;
		if (!in.eof())
		{
			getline(in, line);
			std::istringstream issX(line);
			while (issX >> x)
			{
				vecX.push_back(x);
			}
		}
		else
		{
			shared_ptr<WriteLogging> wl = make_shared<WriteLogging>();
			wl->Execute(mFilename, " ERROR : empty");
			return;
		}
		if (!in.eof())
		{
			getline(in, line);
			std::istringstream issY(line);
			while (issY >> y)
			{
				vecY.push_back(y);
			}
		}
		else
		{
			shared_ptr<WriteLogging> wl = make_shared<WriteLogging>();
			wl->Execute(mFilename, " ERROR : empty");
			return;
		}

		while (!in.eof())
		{
			tmp.clear();
			uint16_t count = 0;
			getline(in, line);
			std::istringstream issZ(line);
			while (issZ >> z)
			{
				tmp.push_back(Point3D(vecX[count], vecY[count], z));
				count++;
			}
			mData.push_back(tmp);
		}
		return;
	}

protected:
	std::vector<std::vector<Point3D> >	mData;
	std::vector<Point3D>				tmp;
};

#pragma endregion


typedef ReaderBase<Vertices> ReaderData;

typedef ReaderBase<Info> ReaderInfo;

typedef ReaderBase<Para> ReaderPara;

typedef ReaderBase<std::vector<std::vector<Point3D> > > ReaderTerrainData;

typedef std::vector<std::vector<Point3D> > TerrainData;

#endif
