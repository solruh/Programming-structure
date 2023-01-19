#ifndef _STR_H_
#define _STR_H_
#include <unistd.h>
size_t StrGetLength(const char* pcSrc);
int StrCompare(const char* pcS1, const char* pcS2);
char *StrCopy(char *pcDest, const char* pcS2);
char *StrSearch(const char* pcHaystack, const char *pcNeedle);
char *StrConcat(char *pcDest, const char* pcSrc);
#endif