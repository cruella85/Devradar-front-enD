	.global _longjmp
	.global longjmp
	.type   _longjmp,@function
	.type   longjmp,@function
_longjmp:
longjmp:
	/*
	 * void longjmp(jmp_buf env, int val);
	 * put val into return register and restore the env saved in setjmp
	 * if va