#include "libm.h"

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double tanhl(long double x)
{
	return tanh(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
long double tanhl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se &