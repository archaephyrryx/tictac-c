#ifndef MEM_H
#define MEM_H

extern void xfree(void *);
extern void *xmalloc(size_t);
extern void *xcalloc(size_t, size_t);
extern void *xrealloc(void *, size_t);
extern char *xstrdup(const char *);
extern char *xstrndup(const char *, size_t);

#endif
