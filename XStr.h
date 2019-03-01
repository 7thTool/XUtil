#ifndef _H_XSTR_H_
#define _H_XSTR_H_

#include "XUtil.h"

#ifndef _T
#ifdef UNICODE
typedef wchar_t TCHAR;
typedef wchar_t tchar;
#define _T(x)	L ## x 
#else
typedef char	TCHAR;
typedef char	tchar;
#define _T(x)	x
#endif//UNICODE 
#endif//_T

#ifdef WIN32

#ifndef snprintf
#define snprintf _snprintf
#endif//snprintf

#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif//vsnprintf

#else

//#ifndef vsprintf
//#define vsprintf vsnprintf
//#endif//vsprintf

#ifndef stricmp
#define stricmp strcasecmp 
#endif//stricmp

#ifndef wcsicmp
#define wcsicmp wcscasecmp 
#endif//wcsicmp

#ifndef strnicmp
#define strnicmp strncasecmp 
#endif//strnicmp

#ifndef wcsnicmp
#define wcsnicmp wcsncasecmp 
#endif//wcsnicmp

#ifndef strupr
static char *_strupr(char *s)
{
	char *str;
	str = s;
	while(*str != '\0')
	{
		if(*str >= 'a' && *str <= 'z') {
			*str += 'A'-'a';
		}
		str++;
	}
	return s;
}
#define strupr _strupr
#endif//strupr

#ifndef strlwr
static char *_strlwr(char *s)
{
	char *str;
	str = s;
	while(*str != '\0')
	{
		if(*str >= 'A' && *str <= 'Z') {
			*str += 'a'-'A';
		}
		str++;
	}
	return s;
}
#define strlwr _strlwr
#endif//strlwr

#endif//

static char chupr(char ch) {
    if(ch >= 'a' && ch <= 'z') {
        ch -= 32; 
    }
    return ch;
}

static char chlwr(char ch) {
    if(ch >= 'A' && ch <= 'Z') {
        ch += 32;
    }
    return ch;
}

//////////////////////////////////////////////////////////////////////////
///注意：反向处理的的源指针要指向最后，正向处理源指向开始位置，原则就是执行最先开始处理的位置

#ifdef WIN32
#else
static int memicmp(char * src, char * dst, size_t count)
{
	if (src == NULL || dst == NULL || count == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (count--)
		{
			f = chupr(*src++);
			l = chupr(*dst++);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}
#endif//
static int wmemicmp(wchar_t * src, wchar_t * dst, size_t count)
{
	if (src == NULL || dst == NULL || count == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (count--)
		{
			f = chupr(*src++);
			l = chupr(*dst++);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

static int memrcmp(char * src, char * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = *src--;
			l = *dst--;
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

static int wmemrcmp(wchar_t * src, wchar_t * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = *src--;
			l = *dst--;
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

static int memricmp(char * src, char * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = chupr(*src--);
			l = chupr(*dst--);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

static int wmemricmp(wchar_t * src, wchar_t * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = chupr(*src--);
			l = chupr(*dst--);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

#define strrcmp(src,dst) memrcmp(src,dst,strlen(dst))
#define strrnicmp(src,dst,len) memricmp(src,dst,len)
#define wcsrcmp(src,dst) wmemrcmp(src,dst,wcslen(dst))
#define wcsrnicmp(src,dst,len) wmemricmp(src,dst,len)
#ifdef UNICODE
#define _tcsrcmp wcsrcmp
#define _tcsrnicmp wcsrnicmp
#else
#define _tcsrcmp strrcmp
#define _tcsrnicmp strrnicmp
#endif//


static char* memmem(char * src, size_t srclen, char * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (memcmp(src, dst, dstlen) == 0 ? (char*)src : 0);
	}
	else {
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (memcmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
	}
	return 0;
}

static wchar_t* wmemmem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (wmemcmp(src, dst, dstlen) == 0 ? (wchar_t*)src : 0);
	}
	else {
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (wmemcmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
	}
}

static char* memimem(char * src, size_t srclen, char * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (memicmp(src, dst, dstlen) == 0 ? (char*)src : 0);
	}
	else {
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (memicmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
	}
}

static wchar_t* wmemimem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (wmemicmp(src, dst, dstlen) == 0 ? (wchar_t*)src : 0);
	}
	else {
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (wmemicmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
	}
}

static char* memrmem(char * src, size_t srcrlen, char * dst, size_t dstrlen)
{
	if (src == NULL || dst == NULL || srcrlen < dstrlen) {
		return (0);
	}
	else if (srcrlen == dstrlen) {
		return (memrcmp(src, dst, dstrlen) == 0 ? (char*)src : 0);
	}
	else {
		for (int i = 0, j = srcrlen - dstrlen; i <= j; i++)
		{
			if (memrcmp(src - i, dst, dstrlen) == 0) {
				return (src - i);
			}
		}
		return (0);
	}
	return 0;
}

static wchar_t* wmemrmem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen)
{
	if (src == NULL || dst == NULL || srcrlen < dstrlen) {
		return (0);
	}
	else if (srcrlen == dstrlen) {
		return (wmemrcmp(src, dst, dstrlen) == 0 ? (wchar_t*)src : 0);
	}
	else {
		for (int i = 0, j = srcrlen - dstrlen; i <= j; i++)
		{
			if (wmemrcmp(src - i, dst, dstrlen) == 0) {
				return (src - i);
			}
		}
		return (0);
	}
}

static char* memrimem(char * src, size_t srcrlen, char * dst, size_t dstrlen)
{
	if (src == NULL || dst == NULL || srcrlen < dstrlen) {
		return (0);
	}
	else if (srcrlen == dstrlen) {
		return (memricmp(src, dst, dstrlen) == 0 ? (char*)src : 0);
	}
	else {
		for (int i = 0, j = srcrlen - dstrlen; i <= j; i++)
		{
			if (memricmp(src - i, dst, dstrlen) == 0) {
				return (src - i);
			}
		}
		return (0);
	}
}

static wchar_t* wmemrimem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen)
{
	if (src == NULL || dst == NULL || srcrlen < dstrlen) {
		return (0);
	}
	else if (srcrlen == dstrlen) {
		return (wmemricmp(src, dst, dstrlen) == 0 ? (wchar_t*)src : 0);
	}
	else {
		for (int i = 0, j = srcrlen - dstrlen; i <= j; i++)
		{
			if (wmemricmp(src - i, dst, dstrlen) == 0) {
				return (src - i);
			}
		}
		return (0);
	}
}

static char* memrep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen)
{
	int brk = 0;
	int src_cnt = 0;
	int dst_cnt = 0;
	while (src_cnt < srclen && dst_cnt < dstlen)
	{
		brk = 1;
		char* temp = memmem(src + src_cnt, srclen - src_cnt, r, rlen);
		if (temp) {
			int move_cnt = temp - (src + src_cnt);
			if (move_cnt <= (dstlen - dst_cnt)) {
				memmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;

				if (tlen <= dstlen - dst_cnt) {
					memmove(dst + dst_cnt, t, tlen);
					src_cnt += rlen;
					dst_cnt += tlen;
					brk = 0;
				}
			}
		}
		else {
			int move_cnt = srclen - src_cnt;
			if (move_cnt <= (dstlen - dst_cnt)) {
				memmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;
			}
		}
		if (brk) {
			break;
		}
	}
	return (brk ? 0 : dst);
}

static wchar_t* wmemrep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen)
{
	int brk = 0;
	int src_cnt = 0;
	int dst_cnt = 0;
	while (src_cnt < srclen && dst_cnt < dstlen)
	{
		brk = 1;
		wchar_t* temp = wmemmem(src + src_cnt, srclen - src_cnt, r, rlen);
		if (temp) {
			int move_cnt = temp - (src + src_cnt);
			if (move_cnt <= (dstlen - dst_cnt)) {
				wmemmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;

				if (tlen <= dstlen - dst_cnt) {
					wmemmove(dst + dst_cnt, t, tlen);
					src_cnt += rlen;
					dst_cnt += tlen;
					brk = 0;
				}
			}
		}
		else {
			int move_cnt = srclen - src_cnt;
			if (move_cnt <= (dstlen - dst_cnt)) {
				wmemmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;
			}
		}
		if (brk) {
			break;
		}
	}
	return (brk ? 0 : dst);
}

static char* memirep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen)
{
	int brk = 0;
	int src_cnt = 0;
	int dst_cnt = 0;
	while (src_cnt < srclen && dst_cnt < dstlen)
	{
		brk = 1;
		char* temp = memimem(src + src_cnt, srclen - src_cnt, r, rlen);
		if (temp) {
			int move_cnt = temp - (src + src_cnt);
			if (move_cnt <= (dstlen - dst_cnt)) {
				memmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;

				if (tlen <= dstlen - dst_cnt) {
					memmove(dst + dst_cnt, t, tlen);
					src_cnt += rlen;
					dst_cnt += tlen;
					brk = 0;
				}
			}
		}
		else {
			int move_cnt = srclen - src_cnt;
			if (move_cnt <= (dstlen - dst_cnt)) {
				memmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;
			}
		}
		if (brk) {
			break;
		}
	}
	return (brk ? 0 : dst);
}

static wchar_t* wmemirep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen)
{
	int brk = 0;
	int src_cnt = 0;
	int dst_cnt = 0;
	while (src_cnt < srclen && dst_cnt < dstlen)
	{
		brk = 1;
		wchar_t* temp = wmemimem(src + src_cnt, srclen - src_cnt, r, rlen);
		if (temp) {
			int move_cnt = temp - (src + src_cnt);
			if (move_cnt <= (dstlen - dst_cnt)) {
				wmemmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;

				if (tlen <= dstlen - dst_cnt) {
					wmemmove(dst + dst_cnt, t, tlen);
					src_cnt += rlen;
					dst_cnt += tlen;
					brk = 0;
				}
			}
		}
		else {
			int move_cnt = srclen - src_cnt;
			if (move_cnt <= (dstlen - dst_cnt)) {
				wmemmove(dst + dst_cnt, src + src_cnt, move_cnt);
				src_cnt += move_cnt;
				dst_cnt += move_cnt;
			}
		}
		if (brk) {
			break;
		}
	}
	return (brk ? 0 : dst);
}

static char* memtrimleft(char* src, int srclen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (!memmem(spec, speclen, src + i, 1)) {
				if (i != 0) {
					memmove(src, src + i, srclen - i);
				}
				return src;
			}
		}
		return (0);
	}
}

static wchar_t* wmemtrimleft(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (!wmemmem(spec, speclen, src + i, 1)) {
				if (i != 0) {
					wmemmove(src, src + i, srclen - i);
				}
				return src;
			}
		}
		return (0);
	}
}

static char* memtrimright(char* src, int srclen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = srclen - 1, j = 0; i >= j; i--)
		{
			if (!memmem(spec, speclen, src + i, 1)) {
				break;
			}
			src[i] = 0;
		}
		return src;
	}
}

static wchar_t* wmemtrimright(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = srclen - 1, j = 0; i >= j; i--)
		{
			if (!wmemmem(spec, speclen, src + i, 1)) {
				break;
			}
			src[i] = 0;
		}
		return src;
	}
}

static char* memtrim(char* src, int srclen, char* spec, int speclen)
{
	src = memtrimleft(src, srclen, spec, speclen);
	src = memtrimright(src, srclen, spec, speclen);
	return src;
}

static wchar_t* wmemtrim(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	src = wmemtrimleft(src, srclen, spec, speclen);
	src = wmemtrimright(src, srclen, spec, speclen);
	return src;
}

static char* memskip(char* src, int srclen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (!memmem(spec, speclen, src + i, 1)) {
				return (src + i);
			}
		}
		return (0);
	}
}

static wchar_t* wmemskip(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (!wmemmem(spec, speclen, src + i, 1)) {
				return (src + i);
			}
		}
		return (0);
	}
}

static char* memrskip(char* src, int srcrlen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srcrlen; i < j; i++)
		{
			if (!memmem(spec, speclen, src - i, 1)) {
				return (src - i);
			}
		}
		return (0);
	}
}

static wchar_t* wmemrskip(wchar_t* src, int srcrlen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srcrlen; i < j; i++)
		{
			if (!wmemmem(spec, speclen, src - i, 1)) {
				return (src - i);
			}
		}
		return (0);
	}
}

static char* membrk(char* src, int srclen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (memmem(spec, speclen, src + i, 1)) {
				return (src + i);
			}
		}
		return (0);
	}
}

static wchar_t* wmembrk(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srclen; i < j; i++)
		{
			if (wmemmem(spec, speclen, src + i, 1)) {
				return (src + i);
			}
		}
		return (0);
	}
}

static char* memrbrk(char* src, int srcrlen, char* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srcrlen; i < j; i++)
		{
			if (memmem(spec, speclen, src - i, 1)) {
				return (src - i);
			}
		}
		return (0);
	}
}

static wchar_t* wmemrbrk(wchar_t* src, int srcrlen, wchar_t* spec, int speclen)
{
	if (src == NULL || spec == NULL) {
		return (0);
	}
	else {
		for (int i = 0, j = srcrlen; i < j; i++)
		{
			if (wmemmem(spec, speclen, src - i, 1)) {
				return (src - i);
			}
		}
		return (0);
	}
}

static const char* _strnull = "";
static const wchar_t* _wcsnull = L"";
static const char* strnull()
{
	return _strnull;
}
static const wchar_t* wcsnull()
{
	return _wcsnull;
}
#ifdef UNICODE
#define _tcsnull wcsnull
#else
#define _tcsnull strnull
#endif//

static char* stristr(char* str1, char* str2)
{
	char *cp = (char *)str1;
	char *s1, *s2;

	if (!*str2)
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;

		while (*s1 && *s2 && !(chupr(*s1) - chupr(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);
}
static wchar_t* wcsistr(wchar_t * str1, wchar_t* str2)
{
	wchar_t *cp = (wchar_t *)str1;
	wchar_t *s1, *s2;

	if (!*str2)
		return((wchar_t *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (wchar_t *)str2;

		while (*s1 && *s2 && !(chupr(*s1) - chupr(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);
}
#define strnstr(src,len,dst) memnmem(src,len,dst,strlen(dst))
#define wcsrnstr(src,len,dst) wmemrimem(src,len,dst,strlen(dst))
#define strrnistr(src,len,dst) memimem(src,dst,wcslen(dst))
#define wcsnstr(src,len,dst) wmemnmem(src,len,dst,wcslen(dst))
#ifdef UNICODE
#define _tcsnstr wcsnstr
#define _tcsistr wcsistr
#define _tcsnistr wcsnistr
#else
#define _tcsnstr strnstr
#define _tcsistr stristr
#define _tcsnistr strnistr
#endif//
/*
XBOOST_API char* strrstr(char* str1, char* str2);
XBOOST_API wchar_t* wcsrstr(wchar_t * str1, wchar_t* str2);
#ifdef UNICODE
#define _tcsrstr wcsrstr
#else
#define _tcsrstr strrstr
#endif//

XBOOST_API char* strnrstr(char* string, int rlen, char* spec);
XBOOST_API wchar_t* wcsnrstr(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsnrstr wcsnrstr
#else
#define _tcsnrstr strnrstr
#endif//

XBOOST_API char* strirstr(char* str1, char* str2);
XBOOST_API wchar_t* wcsirstr(wchar_t * str1, wchar_t* str2);
#ifdef UNICODE
#define _tcsirstr wcsirstr
#else
#define _tcsirstr strirstr
#endif//

XBOOST_API char* strnirstr(char* string, int rlen, char* spec);
XBOOST_API wchar_t* wcsnirstr(wchar_t* string, int rlen, wchar_t* spec);
//#define strnistr memimem
//#define wcsnistr wmemimem
#ifdef UNICODE
#define _tcsnirstr wcsnirstr
#else
#define _tcsnirstr strnirstr
#endif//

XBOOST_API char* strichr(char * string, int ch);
XBOOST_API wchar_t* wcsichr(wchar_t * string, int ch);
#ifdef UNICODE
#define _tcsichr wcsichr
#else
#define _tcsichr strichr
#endif//

XBOOST_API char* strnchr(char* string, int len, char ch);
XBOOST_API wchar_t* wcsnchr(wchar_t* string, int len, wchar_t ch);
#ifdef UNICODE
#define _tcsnchr wcsnchr
#else
#define _tcsnchr strnchr
#endif//

XBOOST_API char* strnichr(char* string, int len, char ch);
XBOOST_API wchar_t* wcsnichr(wchar_t* string, int len, wchar_t ch);
#ifdef UNICODE
#define _tcsnichr wcsnichr
#else
#define _tcsnichr strnichr
#endif//

char* strtrimleft(char* string, char* spec);
wchar_t* wcstrimleft(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrimleft wcstrimleft
#else
#define _tcstrimleft strtrimleft
#endif//

char* strntrimleft(char* string, int len, char* spec);
wchar_t* wcsntrimleft(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsntrimleft wcsntrimleft
#else
#define _tcsntrimleft strntrimleft
#endif//

char* strtrimright(char* string, char* spec);
wchar_t* wcstrimright(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrimright wcstrimright
#else
#define _tcstrimright strtrimright
#endif//

char* strntrimright(char* string, int len, char* spec);
wchar_t* wcsntrimright(wchar_t* string, int len, wchar_t* spec);
//#define strntrimright memtrimright
//#define wcsntrimright wmemtrimright
#ifdef UNICODE
#define _tcsntrimright wcsntrimright
#else
#define _tcsntrimright strntrimright
#endif//

char* strtrim(char* string, char* spec);
wchar_t* wcstrim(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrim wcstrim
#else
#define _tcstrim strtrim
#endif//

char* strntrim(char* string, int len, char* spec);
wchar_t* wcsntrim(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsntrim wcsntrim
#else
#define _tcsntrim strntrim
#endif//

XBOOST_API char* strrep(char* string, char* r, char* t);
XBOOST_API wchar_t* wcsrep(wchar_t* string, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsrep wcsrep
#else
#define _tcsrep strrep
#endif//

XBOOST_API char* strnrep(char* string, int len, char* r, char* t);
XBOOST_API wchar_t* wcsnrep(wchar_t* string, int len, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsnrep wcsnrep
#else
#define _tcsnrep strnrep
#endif//

XBOOST_API char* strirep(char* string, char* r, char* t);
XBOOST_API wchar_t* wcsirep(wchar_t* string, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsirep wcsirep
#else
#define _tcsirep strirep
#endif//

XBOOST_API char* strnirep(char* string, int len, char* r, char* t);
XBOOST_API wchar_t* wcsnirep(wchar_t* string, int len, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsnirep wcsnirep
#else
#define _tcsnirep strnirep
#endif//

XBOOST_API char* strrepchr(char* string, char r, char t);
XBOOST_API wchar_t* wcsrepchr(wchar_t* string, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsrepchr wcsrepchr
#else
#define _tcsrepchr strrepchr
#endif//

XBOOST_API char* strnrepchr(char* string, int len, char r, char t);
XBOOST_API wchar_t* wcsnrepchr(wchar_t* string, int len, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsnrepchr wcsnrepchr
#else
#define _tcsnrepchr strnrepchr
#endif//

XBOOST_API char* strirepchr(char* string, char r, char t);
XBOOST_API wchar_t* wcsirepchr(wchar_t* string, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsirepchr wcsirepchr
#else
#define _tcsirepchr strirepchr
#endif//

XBOOST_API char* strnirepchr(char* string, int len, char r, char t);
XBOOST_API wchar_t* wcsnirepchr(wchar_t* string, int len, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsnirepchr wcsnirepchr
#else
#define _tcsnirepchr strnirepchr
#endif//

//skip left spec
XBOOST_API char* strskipchr(char* string, char spec);
XBOOST_API wchar_t* wcsskipchr(wchar_t* string, wchar_t spec);
#ifdef UNICODE
#define _tcsskipchr wcsskipchr
#else
#define _tcsskipchr strskipchr
#endif//

XBOOST_API char* strnskipchr(char* string, int len, char spec);
XBOOST_API wchar_t* wcsnskipchr(wchar_t* string, int len, wchar_t spec);
#ifdef UNICODE
#define _tcsnskipchr wcsnskipchr
#else
#define _tcsnskipchr strnskipchr
#endif//

XBOOST_API char* strskip(char* string, char* spec);
XBOOST_API wchar_t* wcsskip(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsskip wcsskip
#else
#define _tcsskip strskip
#endif//

XBOOST_API char* strnskip(char* string, int len, char* spec);
XBOOST_API wchar_t* wcsnskip(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsnskip wcsnskip
#else
#define _tcsnskip strnskip
#endif//

XBOOST_API char* strrskip(char* string, int rlen, char* spec);
XBOOST_API wchar_t* wcsrskip(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsrskip wcsrskip
#else
#define _tcsrskip strrskip
#endif//

XBOOST_API char* strbrkchr(char* string, char spec);
XBOOST_API wchar_t* wcsbrkchr(wchar_t* string, wchar_t spec);
#ifdef UNICODE
#define _tcsbrkchr wcsbrkchr
#else
#define _tcsbrkchr strbrkchr
#endif//

XBOOST_API char* strnbrkchr(char* string, int len, char spec);
XBOOST_API wchar_t* wcsnbrkchr(wchar_t* string, int len, wchar_t spec);
#ifdef UNICODE
#define _tcsnbrkchr wcsnbrkchr
#else
#define _tcsnbrkchr strnbrkchr
#endif//

XBOOST_API char* strrbrkchr(char* string, int rlen, char spec);
XBOOST_API wchar_t* wcsrbrkchr(wchar_t* string, int rlen, wchar_t spec);
#ifdef UNICODE
#define _tcsrbrkchr wcsrbrkchr
#else
#define _tcsrbrkchr strrbrkchr
#endif//

XBOOST_API char* strbrk(char* string, char* spec);
XBOOST_API wchar_t* wcsbrk(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsbrk wcsbrk
#else
#define _tcsbrk strbrk
#endif//

XBOOST_API char* strnbrk(char* string, int len, char* spec);
XBOOST_API wchar_t* wcsnbrk(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsnbrk wcsnbrk
#else
#define _tcsnbrk strnbrk
#endif//

XBOOST_API char* strrbrk(char* string, char* spec);
XBOOST_API wchar_t* wcsrbrk(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsrbrk wcsrbrk
#else
#define _tcsrbrk strrbrk
#endif//

XBOOST_API char* strnrbrk(char* string, int rlen, char* spec);
XBOOST_API wchar_t* wcsnrbrk(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsnrbrk wcsnrbrk
#else
#define _tcsnrbrk strnrbrk
#endif//

#if _MSC_VER>1200
#ifdef USE_REGEX
XBOOST_API bool strnregex(char*src, int srclen, char *dst, int dstlen);
XBOOST_API bool wcsnregex(wchar_t*src, int srclen, wchar_t *dst, int dstlen);
#ifdef UNICODE
#define _tcsnregex wcsnregex
#else
#define _tcsnregex strnregex
#endif//

XBOOST_API bool strregex(char*src, char *dst);
XBOOST_API bool wcsregex(wchar_t*src, wchar_t *dst);
#ifdef UNICODE
#define _tcsregex wcsregex
#else
#define _tcsregex strregex
#endif//
#endif//
#endif//

#ifdef WIN32
#else
#define CP_ACP	0
#define CP_OEM	1
#define CP_UTF8 65001
#define CP_GBK  2312
#endif//

XBOOST_API int mb2wc(const char* src, int srclen, wchar_t* dst, int dstLen, unsigned int cp = 0);
XBOOST_API int wc2mb(const wchar_t* src, int srclen, char* dst, int dstLen, unsigned int cp = 0);

XBOOST_API void byte2hex(byte by, char* hex);
XBOOST_API void buf2hex(const byte* buf, int srclen, char* dst, int dstlen);
XBOOST_API byte hex2byte(const char* hex);

//XBOOST_API BOOL RectInRect(LPRECT lprc1, LPRECT lprc2);
//XBOOST_API RECT String2RectA(const char* lpszRect);
//XBOOST_API RECT String2RectW(const wchar_t* lpszRect);
//XBOOST_API char* Rect2StringA(char* str, LPCRECT lpRect);
//XBOOST_API wchar_t* Rect2StringW(wchar_t* str, LPCRECT lpRect);
//#ifdef UNICODE
//#define _RECT(x)	String2RectW(x)
//#define _RECTSTR(s,x)	Rect2StringW(s,x)
//#else
//#define _RECT(x)	String2RectA(x)
//#define _RECTSTR(s,x)	Rect2StringA(s,x)
//#endif//

XBOOST_API uint32_t strtocolor(const char* str);
XBOOST_API uint32_t wcstocolor(const wchar_t* str);
XBOOST_API char* colortostr(char* str, uint32_t cr);
XBOOST_API wchar_t* colortowcs(wchar_t* str, uint32_t cr);
#ifdef UNICODE
#define _rgbfromstr(x)	wcstocolor(x)
#define _rgbtostr(s,x) colortowcs(s,x)
#else
#define _rgbfromstr(x)	strtocolor(x)
#define _rgbtostr(s,x) colortostr(s,x)
#endif//

XBOOST_API uint32_t strhextouint32(const char* val);
XBOOST_API uint32_t wcshextouint32(const wchar_t* val);
XBOOST_API char* uint32tostrhex(char* str, uint32_t val);
XBOOST_API wchar_t* uint32towcshex(wchar_t* str, uint32_t val);
#ifdef UNICODE
#define _hexuint32fromhexstr(x)	wcshextouint32(x)
#define _uint32tohexstr(s,x)	uint32towcshex(s,x)
#else
#define _uint32fromhexstr(x)	strhextouint32(x)
#define _uint32tohexstr(s,x)	uint32tostrhex(s,x)
#endif//

XBOOST_API uint32_t strhextocolor(const char* val);
XBOOST_API uint32_t wcshextocolor(const wchar_t* val);
#ifdef UNICODE
#define _hexcolorfromstr(x)	wcshextocolor(x)
#else
#define _hexcolorfromstr(x)	strhextocolor(x)
#endif//*/

#endif//_H_XSTR_H_