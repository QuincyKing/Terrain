#ifndef MATH_H_
#define MATH_H_

#include <cmath>
#include <memory>
#include <cassert>

#define ALIGNED_16  __declspec(align(16))
#define ALIGNED_4   __declspec(align(4))

#define isnan(f) _isnan((f))
#define isinf(f) (!_finite((f)))

namespace BasicLib
{
	enum ContainmentType
	{
		CT_Disjoint,
		CT_Contains,
		CT_Intersects
	};

	template<typename Real>
	class Math
	{
	public:
		static const Real EPSILON;
		static const Real ZERO_TOLERANCE;
		static const Real MAX_REAL;
		static const Real INFINITY1;
		static const Real PI;
		static const Real TWO_PI;
		static const Real FOUR_PI;
		static const Real HALF_PI;
		static const Real INV_PI;
		static const Real INV_TWO_PI;
		static const Real INV_FOUR_PI;
		static const Real DEG_TO_RAD;
		static const Real RAD_TO_DEG;
		static const Real LN_2;
		static const Real LN_10;
		static const Real INV_LN_2;
		static const Real INV_LN_10;
		static const Real SQRT_2;
		static const Real INV_SQRT_2;
		static const Real SQRT_3;
		static const Real INV_SQRT_3;
	};

	typedef Math<float> Mathf;
	typedef Math<double> Mathd;

	template<> const float Mathf::EPSILON = FLT_EPSILON;
	template<> const float Mathf::ZERO_TOLERANCE = 1e-06f;
	template<> const float Mathf::MAX_REAL = FLT_MAX;
	template<> const float Mathf::INFINITY1 = std::numeric_limits<float>::infinity();
	template<> const float Mathf::PI = (float)(4.0*atan(1.0));
	template<> const float Mathf::TWO_PI = 2.0f*Mathf::PI;
	template<> const float Mathf::FOUR_PI = 4.0f*Mathf::PI;
	template<> const float Mathf::HALF_PI = 0.5f*Mathf::PI;
	template<> const float Mathf::INV_PI = 1.0f / Mathf::PI;
	template<> const float Mathf::INV_TWO_PI = 1.0f / Mathf::TWO_PI;
	template<> const float Mathf::INV_FOUR_PI = 1.0f / Mathf::FOUR_PI;
	template<> const float Mathf::DEG_TO_RAD = Mathf::PI / 180.0f;
	template<> const float Mathf::RAD_TO_DEG = 180.0f/Mathf::PI;
   
	template<> const double Mathd::EPSILON = DBL_EPSILON;
	template<> const double Mathd::ZERO_TOLERANCE = 1e-08;
	template<> const double Mathd::MAX_REAL = DBL_MAX;
	template<> const double Mathd::INFINITY1 = std::numeric_limits<double>::infinity();
	template<> const double Mathd::PI = (double)(4.0*atan(1.0));
	template<> const double Mathd::TWO_PI = 2.0*Mathd::PI;
	template<> const double Mathd::FOUR_PI = 4.0*Mathd::PI;
	template<> const double Mathd::HALF_PI = 0.5*Mathd::PI;
	template<> const double Mathd::INV_PI = 1.0 / Mathd::PI;
	template<> const double Mathd::INV_TWO_PI = 1.0 / Mathd::TWO_PI;
	template<> const double Mathd::INV_FOUR_PI = 1.0 / Mathd::FOUR_PI;
	template<> const double Mathd::DEG_TO_RAD = Mathd::PI / 180.0;
	template<> const double Mathd::RAD_TO_DEG = 180.0 / Mathd::PI;

	template<typename Real>
	inline Real ToDegree(Real radian)
	{
		return radian*Math<Real>::RAD_TO_DEG;
	}

	template<typename Real>
	inline Real ToRadian(Real degree)
	{
		return degree*Math<Real>::DEG_TO_RAD;
	}

	template<typename Real>
	inline Real Clamp(Real val, Real min, Real max)
	{
		if (val > max) val = max;
		if (val < min) val = min;
		return val;
	}

	template<typename Real>
	inline Real Log2(Real x)
	{
		static Real invLog2 = Real(1) / log(Real(2));
		return log(x) * invLog2;
	}

	template<typename Real>
	inline Real Lerp(Real a, Real b, float t)
	{
		return a + (b - a)*t;
	}
}

#endif