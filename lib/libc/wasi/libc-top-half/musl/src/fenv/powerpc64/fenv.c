#define _GNU_SOURCE
#include <fenv.h>
#include <features.h>

static inline double get_fpscr_f(void)
{
	double d;
	__asm__ __volatile__("mffs %0" : "=d"(d));
	return d;
}

static inline long get_fpscr(void)
{
	return (union {double f; long i;}) {get_fpscr_f()}.i;
}

static inline void set_fpscr_f(double fpscr)
{
	__asm__ __volatile__("mtfsf 255, %0" : : "d"(fpscr));
}

static void set_fpscr(long fpscr)
{
	set_fpscr_f((union {long i; double f;}) {fpscr}.f);
}

int feclearexcept(int mask)
{
	mask &= FE_ALL_EXCEPT