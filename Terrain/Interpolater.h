#ifndef INTERPOLATER_H
#define INTERPOLATER_H
#include <vector>
#include <cassert>
using namespace std;

#include "BaseException.h"
#include "BaseData.h"

class InterpolaterException : public BaseException
{
public:
	InterpolaterException(string message) throw() : BaseException(message) {}
	InterpolaterException(string message, string location) throw() : BaseException(message, location) {}
};

template<class ForwardIterator>
class TInterpolater
{
public:
	TInterpolater() {}
	virtual ~TInterpolater() {}
};

typedef TInterpolater<vector<Point3D>::iterator> Interpolater;
#endif