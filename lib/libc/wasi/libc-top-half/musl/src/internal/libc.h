#ifndef LIBC_H
#define LIBC_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

struct __locale_map;

struct __locale_struct {
	const struct __locale_map *cat[6];
};

struct tls_module {
	struct tls_module *next;
	void *image;
	size_t len, size, align, offset;
};

struct __libc {
#ifdef __wasilibc_unmodified_upstream
	char can_do_threads;
#endif
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
	char threaded;
#endif
#ifdef __wa