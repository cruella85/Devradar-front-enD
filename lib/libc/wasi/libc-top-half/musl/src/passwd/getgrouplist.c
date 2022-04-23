#define _GNU_SOURCE
#include "pwf.h"
#include <grp.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include <errno.h>
#include "nscd.h"

int getgrouplist(const char *user, gid_t gid, gid_t *groups, int *ngroups)
{
	int rv, nlim, ret = -1;
	ssize_t i, n = 1;
	struct group gr;
	struct group *res;
	FILE *f;
	int swap = 0;
	int32_t resp[INITGR_LEN];
	uint32_t *nscdbuf = 0;
	char *buf = 0;
	char **mem = 0;
	size_t nmem = 0;
	size_t size;
	nlim = *ng