#ifndef Sag_h__
#define Sag_h__

#include <cstdint>
#include <algorithm>
#include <math.h>
#include <vector>

class Sag
{
public:
	Sag(void) {}
	Sag(const std::vector<double> &P0, const double P, const double Q, const double K, 
		 const double m, const double q, const double beta);

	double Deflection(double x, double y, uint16_t level);
	
private:
	void SetAlpha(uint16_t level);

	double mAlpha;
	std::vector<double> mP0;
	double mP;
	double mQ;
	double mK;
	double mM;
	double mq;
	double mBeta;
	double mResult;
};

Sag::Sag(const std::vector<double> &P0, const double P, const double Q, const double K,
	                                        const double m, const double q, const double beta)
								:mP(P), mQ(Q), mK(K), mM(m), mq(q), mBeta(beta)
{
	mP0 = P0;
}
 
double Sag::Deflection(double x, double y, uint16_t level)
{
	SetAlpha(level);

	double sign = pow(-1, ceil(2 - mAlpha));
	double invFactor = 1/(mAlpha * (mAlpha - 1) * (3 * mP + 3 * mQ + mP*mQ + mK*mP*mQ));
	double tmp = x*x / (2 * mP) + y*y / (2 * mQ) - 1;
	double factor = mP0[level] * mP * mQ * pow(tmp, ceil(mAlpha));
	mResult = factor * sign * invFactor;
	return mResult;
}

void Sag::SetAlpha(uint16_t level)
{
	double invW0 = 1 / mM * mq * cos(mBeta);
	double invFactor = 1 / (3 * mP + 3 * mQ + mP*mQ + mK*mP*mQ);
	mAlpha = 1 / 2 + sqrt(1 + 4 * mP0[level] * mP*mQ * invFactor * invW0) / 2;
}

#endif