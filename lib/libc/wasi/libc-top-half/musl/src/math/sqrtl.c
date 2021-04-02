#include <stdint.h>
#include <math.h>
#include <float.h>
#include "libm.h"

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double sqrtl(long double x)
{
	return sqrt(x);
}
#elif (LDBL_MANT_DIG == 113 || LDBL_MANT_DIG == 64) && LDBL_MAX_EXP == 16384
#include "sqrt_data.h"

#define FENV_SUPPORT 1

typedef struct {
	uint64_t hi;
	uint64_t lo;
} u128;

/* top: 16 bit sign+exponent, x: significand.  */
static inline long double mkldbl(uint64_t top, u128 x)
{
	union ldshape u;
#if LDBL_MANT_DIG == 113
	u.i2.hi = x.hi;
	u.i2.lo = x.lo;
	u.i2.hi &= 0x0000ffffffffffff;
	u.i2.hi |= top << 48;
#elif LDBL_MANT_DIG == 64
	u.i.se = top;
	u.i.m = x.lo;
	/* force the top bit on non-zero (and non-subnormal) results.  */
	if (top & 0x7fff)
		u.i.m |= 0x8000000000000000;
#endif
	return u.f;
}

/* return: top 16 bit is sign+exp and following bits are the significand.  */
static inline u128 asu128(long double x)
{
	union ldshape u = {.f=x};
	u128 r;
#if LDBL_MANT_DIG == 113
	r.hi = u.i2.hi;
	r.lo = u.i2.lo;
#elif LDBL_MANT_DIG == 64
	r.lo = u.i.m<<49;
	/* ignore the top bit: pseudo numbers are not handled. */
	r.hi = u.i.m>>15;
	r.hi &= 0x0000ffffffffffff;
	r.hi |= (uint64_t)u.i.se << 48;
#endif
	return r;
}

/* returns a*b*2^-32 - e, with error 0 <= e < 1.  */
static inline uint32_t mul32(uint32_t a, uint32_t b)
{
	return (uint64_t)a*b >> 32;
}

/* returns a*b*2^-64 - e, with error 0 <= e < 3.  */
static inline uint64_t mul64(uint64_t a, uint64_t b)
{
	uint64_t ahi = a>>32;
	uint64_t alo = a&0xffffffff;
	uint64_t bhi = b>>32;
	uint64_t blo = b&0xffffffff;
	return ahi*bhi + (ahi*blo >> 32) + (alo*bhi >> 32);
}

static inline u128 add64(u128 a, uint64_t b)
{
	u128 r;
	r.lo = a.lo + b;
	r.hi = a.hi;
	if (r.lo < a.lo)
		r.hi++;
	return r;
}

static inline u128 add128(u128 a, u128 b)
{
	u128 r;
	r.lo = a.lo + b.lo;
	r.hi = a.hi + b.hi;
	if (r.lo < a.lo)
		r.hi++;
	return r;
}

static inline u128 sub64(u128 a, uint64_t b)
{
	u128 r;
	r.lo = a.lo - b;
	r.hi = a.hi;
	if (a.lo < b)
		r.hi--;
	return r;
}

static inline u128 sub128(u128 a, u128 b)
{
	u128 r;
	r.lo = a.lo - b.lo;
	r.hi = a.hi - b.hi;
	if (a.lo < b.lo)
		r.hi--;
	return r;
}

/* a<<n, 0 <= n <= 127 */
static inline u128 lsh(u128 a, int n)
{
	if (n == 0)
		return a;
	if (n >= 64) {
		a.hi = a.lo<<(n-64);
		a.lo = 0;
	} else {
		a.hi = (a.hi<<n) | (a.lo>>(64-n));
		a.lo = a.lo<<n;
	}
	return a;
}

/* a>>n, 0 <= n <= 127 */
static inline u128 rsh(u128 a, int n)
{
	if (n == 0)
		return a;
	if (n >= 64) {
		a.lo = a.hi>>(n-64);
		a.hi = 0;
	} else {
		a.lo = (a.lo>>n) | (a.hi<<(64-n));
		a.hi = a.hi>>n;
