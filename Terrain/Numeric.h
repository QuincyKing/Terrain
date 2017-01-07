#ifndef NUMERIC_H
#define NUMERIC_H
#include "Matrix.h"
#include "BaseException.h"
#include <vector>

#define TINY_VALUE			1.5e-16

class NumericException : public BaseException
{
public:
	NumericException() throw() : BaseException("Numeric Exception") {}
	NumericException(std::string message) throw() : BaseException(message) {}
};

template<class T>
void LUDecompose(TMatrix<T>& A, std::vector<int>& Permutation, int& d) throw(NumericException)
{
	int n = A.GetHeight();
	vector<T> vv(n);
	Permutation.resize(n);

	d = 1;

	T amax;
	for (int i = 0; i<n; i++) {
		amax = 0.0;
		for (int j = 0; j<n; j++)
		if (fabs(A(i, j)) > amax)
			amax = fabs(A(i, j));

		if (amax < TINY_VALUE)
			throw NumericException();

		vv[i] = 1.0 / amax;
	}

	T sum, dum;
	int imax;
	for (int j = 0; j<n; j++) {
		for (int i = 0; i<j; i++) {
			sum = A(i, j);
			for (int k = 0; k<i; k++)
				sum -= A(i, k) * A(k, j);
			A(i, j) = sum;
		}
		amax = 0.0;

		for (int i = j; i<n; i++) {
			sum = A(i, j);
			for (int k = 0; k<j; k++)
				sum -= A(i, k) * A(k, j);

			A(i, j) = sum;
			dum = vv[i] * fabs(sum);

			if (dum >= amax) {
				imax = i;
				amax = dum;
			}
		}

		if (j != imax) {
			for (int k = 0; k<n; k++) {
				dum = A(imax, k);
				A(imax, k) = A(j, k);
				A(j, k) = dum;
			}
			d = -d;
			vv[imax] = vv[j];
		}
		Permutation[j] = imax;

		if (fabs(A(j, j)) < TINY_VALUE)
			A(j, j) = TINY_VALUE;

		if (j != n) {
			dum = 1.0 / A(j, j);
			for (int i = j + 1; i<n; i++)
				A(i, j) *= dum;
		}
	}
}

template<class T>
void LUBackSub(TMatrix<T>& A, std::vector<int>& Permutation, std::vector<T>& B) throw(NumericException)
{
	int n = A.GetHeight();
	T sum;
	int ii = 0;
	int ll;
	for (int i = 0; i<n; i++) {
		ll = Permutation[i];
		sum = B[ll];
		B[ll] = B[i];
		if (ii != 0)
		for (int j = ii; j<i; j++)
			sum -= A(i, j) * B[j];
		else if (sum != 0.0)\
			ii = i;
		B[i] = sum;
	}

	for (int i = n - 1; i >= 0; i--) {
		sum = B[i];
		if (i< n) {
			for (int j = i + 1; j<n; j++)
				sum -= A(i, j) * B[j];
		}
		B[i] = sum / A(i, i);
	}
}
#endif  