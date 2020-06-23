#include <stdlib.h>
#include <langinfo.h>
#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>

char *strptime(const char *restrict s, const char *restrict f, struct tm *restrict tm)
{
	int i, w, neg, adj, min, range, *dest, dummy;
	const char *ex;
	size_t len;
	int want_century = 0, century = 0, relyear = 0;
	while (*f) {
		if (*f != '%') {
			if (isspace(*f)) for (; *s && isspace(*s); s++);
			else if (*s != *f) return 0;
			else s++;
			f++;
			continue;
		}
		f++;
		if (*f == '+') f++;
		if (isdigit(*f)) {
			char *new_f;
			w=strtoul(f, &new_f, 10);
			f = new_f;
		} else {
			w=-1;
		}
		adj=0;
		switch (*f++) {
		case 'a': case 'A':
			dest = &tm->tm_wday;
			min = ABDAY_1;
			range = 7;
			goto symbolic_range;
		case 'b': case 'B': case 'h':
			dest = &tm->tm_mon;
			min = ABMON_1;
			range = 12;
			goto symbolic_range;
		case 'c':
			s = strptime(s, nl_langinfo(D_T_FMT), tm);
			if (!s) return 0;
			break;
		case 'C':
			dest = &century;
			if (w<0) w=2;
			want_century |= 2;
			goto numeric_digits;
		case 'd': case 'e':
			dest = &tm->tm_mday;
			min = 1;
			range = 31;
			goto numeric_range;
		case 'D':
			s = strptime(s, "%m/%d/%y", tm);
			if (!s) return 0;
			break;
		case 'H':
			dest = &tm->tm_hour;
			min = 0;
			range = 24;
			goto numeric_range;
		case 'I':
			dest = &tm->tm_hour;
			min = 1;
			range = 12;
			goto numeric_range;
		case 'j':
			dest = &tm->tm_yday;
			min = 1;
			range = 366;
			adj = 1;
			goto numeric_range;
		case 'm':
			dest = &tm->tm_mon;
			min = 1;
			range = 12;
			adj = 1;
			goto numeric_range;
		case 'M':
			dest = &tm->tm_min;
			min = 0;
			range = 60;
			goto numeric_range;
		case 'n': case 't':
			for (; *s && isspace(*s); s++);
			break;
		case 'p':
			ex = nl_langinfo(AM_STR);