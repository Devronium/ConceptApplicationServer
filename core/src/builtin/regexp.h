#ifndef regexp_h
#define regexp_h

typedef struct Reprog Reprog;
typedef struct Resub Resub;

Reprog *JS_regcomp(const char *pattern, int cflags, const char **errorp);
int JS_regexec(Reprog *prog, const char *string, Resub *sub, int eflags);
void JS_regfree(Reprog *prog);
int JS_reglastindex(Reprog *prog);
void JS_setreglastindex(Reprog *prog, int offset);

enum {
	/* regcomp flags */
	REG_ICASE = 1,
	REG_NEWLINE = 2,
	REG_GLOBAL = 8,

	/* regexec flags */
	REG_NOTBOL = 4,

	/* limits */
	REG_MAXSUB = 32
};

struct Resub {
	unsigned int nsub;
	struct {
		const char *sp;
		const char *ep;
	} sub[REG_MAXSUB];
};

#endif
