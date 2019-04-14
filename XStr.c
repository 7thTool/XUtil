#include "XStr.h"

#ifdef WIN32
#else
int memicmp(char * src, char * dst, size_t count)
{
	if (src == NULL || dst == NULL || count == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (count--)
		{
			f = toupper(*src++);
			l = toupper(*dst++);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}
#endif

int wmemicmp(wchar_t * src, wchar_t * dst, size_t count)
{
	if (src == NULL || dst == NULL || count == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (count--)
		{
			f = toupper(*src++);
			l = toupper(*dst++);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

int memrcmp(char * src, char * dst, size_t rcount)
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

int wmemrcmp(wchar_t * src, wchar_t * dst, size_t rcount)
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

int memricmp(char * src, char * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = toupper(*src--);
			l = toupper(*dst--);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

int wmemricmp(wchar_t * src, wchar_t * dst, size_t rcount)
{
	if (src == NULL || dst == NULL || rcount == 0) {
		return 0;
	}
	else {
		int f = 0, l = 0;
		while (rcount--)
		{
			f = toupper(*src--);
			l = toupper(*dst--);
			if (f != l) {
				break;
			}
		}
		return (f - l);
	}
}

char* memmem(char * src, size_t srclen, char * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (memcmp(src, dst, dstlen) == 0 ? (char*)src : 0);
	}
	else {
#if 0
		char *p1 = (char*)src, *p2 = (char*)dst;
		__asm
		{
			mov	ecx, p1
			mov	edx, p2
			mov eax, ecx
			add eax, srclen
			mov ebx, edx
			add ebx, dstlen
			StartLoop :
			mov	al, [ecx]
				mov	ah, [edx]
				cmp	ecx, eax
				je	EndLoop		//����0
				cmp	edx, ebx
				je	EndLoop		//����0
				inc	ecx
				inc	edx

				cmp	al, ah
				je	StartLoop	//��� �����Ƚ�
				EndLoop :
			cmp	ecx, eax
				je	Found		//��ȫƥ��
				cmp	edx, ebx
				je	NotFound	//��ƥ��
				inc	p1
				mov	ecx, p1
				mov	edx, p2
				jmp	StartLoop
				NotFound :
			mov	p1, 0
				Found :
		}
		return p1;
#else
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (memcmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
#endif//
	}
	return 0;
}

wchar_t* wmemmem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen)
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

char* memimem(char * src, size_t srclen, char * dst, size_t dstlen)
{
	if (src == NULL || dst == NULL || srclen < dstlen) {
		return (0);
	}
	else if (srclen == dstlen) {
		return (memicmp(src, dst, dstlen) == 0 ? (char*)src : 0);
	}
	else {
#if 0
		char *p1 = (char*)src, *p2 = (char*)dst;
		__asm
		{
			mov	ecx, p1
			mov	edx, p2
			mov eax, ecx
			add eax, srclen
			mov ebx, edx
			add ebx, dstlen
			StartLoop :
			mov	al, [ecx]
				mov	ah, [edx]
				cmp	ecx, eax
				je	EndLoop		//����0
				cmp	edx, ebx
				je	EndLoop		//����0
				inc	ecx
				inc	edx

				//ת����д�Ƚ�
				cmp	al, 'a'
				jb	Next		//С��a
				cmp	al, 'z'
				ja	Next		//����z
				and	al, 0xDF	//ת��д
				Next:
			cmp	ah, 'a'
				jb	Compare		//С��a
				cmp	ah, 'z'
				ja	Compare		//����z
				and	ah, 0xDF	//ת��д
				//			//ת��Сд�Ƚ�
				//			cmp	al,	'A'
				//			jb	Next		//С��A
				//			cmp	al,	'Z'
				//			ja	Next		//����Z
				//			or	al,	0x20	//תСд
				//Next:
				//			cmp	ah,	'A'
				//			jb	Compare		//С��A
				//			cmp	ah,	'Z'
				//			ja	Compare		//����Z
				//			or	ah,	0x20	//תСд
			Compare:
			cmp	al, ah
				je	StartLoop	//��� �����Ƚ�
				EndLoop :
			cmp	ecx, eax
				je	Found		//��ȫƥ��
				cmp	edx, ebx
				je	NotFound	//��ƥ��
				inc	p1
				mov	ecx, p1
				mov	edx, p2
				jmp	StartLoop
				NotFound :
			mov	p1, 0
				Found :
		}
		return p1;
#else
		for (int i = 0, j = srclen - dstlen; i <= j; i++)
		{
			if (memicmp(src + i, dst, dstlen) == 0) {
				return (src + i);
			}
		}
		return (0);
#endif//
	}
}

wchar_t* wmemimem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen)
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

char* memrmem(char * src, size_t srcrlen, char * dst, size_t dstrlen)
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

wchar_t* wmemrmem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen)
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

char* memrimem(char * src, size_t srcrlen, char * dst, size_t dstrlen)
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

wchar_t* wmemrimem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen)
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

char* memrep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen)
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

wchar_t* wmemrep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen)
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

char* memirep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen)
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

wchar_t* wmemirep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen)
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

char* memtrimleft(char* src, int srclen, char* spec, int speclen)
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

wchar_t* wmemtrimleft(wchar_t* src, int srclen, wchar_t* spec, int speclen)
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

char* memtrimright(char* src, int srclen, char* spec, int speclen)
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

wchar_t* wmemtrimright(wchar_t* src, int srclen, wchar_t* spec, int speclen)
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

char* memtrim(char* src, int srclen, char* spec, int speclen)
{
	src = memtrimleft(src, srclen, spec, speclen);
	src = memtrimright(src, srclen, spec, speclen);
	return src;
}

wchar_t* wmemtrim(wchar_t* src, int srclen, wchar_t* spec, int speclen)
{
	src = wmemtrimleft(src, srclen, spec, speclen);
	src = wmemtrimright(src, srclen, spec, speclen);
	return src;
}

char* memskip(char* src, int srclen, char* spec, int speclen)
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

wchar_t* wmemskip(wchar_t* src, int srclen, wchar_t* spec, int speclen)
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

char* memrskip(char* src, int srcrlen, char* spec, int speclen)
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

wchar_t* wmemrskip(wchar_t* src, int srcrlen, wchar_t* spec, int speclen)
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

char* membrk(char* src, int srclen, char* spec, int speclen)
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

wchar_t* wmembrk(wchar_t* src, int srclen, wchar_t* spec, int speclen)
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

char* memrbrk(char* src, int srcrlen, char* spec, int speclen)
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

wchar_t* wmemrbrk(wchar_t* src, int srcrlen, wchar_t* spec, int speclen)
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

const char* strnull()
{
	return _strnull;
}

const wchar_t* wcsnull()
{
	return _wcsnull;
}

char* strnstr(char* string, int len, char* substr)
{
	return memmem(string, len, substr, strlen(substr));
}

wchar_t* wcsnstr(wchar_t* string, int len, wchar_t* substr)
{
	return wmemmem(string, len, substr, wcslen(substr));
}

char* stristr(char* str1, char* str2)
{
	char *cp = (char *)str1;
	char *s1, *s2;

	if (!*str2)
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;

		while (*s1 && *s2 && !(toupper(*s1) - toupper(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);
}

wchar_t* wcsistr(wchar_t * str1, wchar_t* str2)
{
	wchar_t *cp = (wchar_t *)str1;
	wchar_t *s1, *s2;

	if (!*str2)
		return((wchar_t *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (wchar_t *)str2;

		while (*s1 && *s2 && !(toupper(*s1) - toupper(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);
}

char* strnistr(char* string, int len, char* spec)
{
	return memimem(string, len, spec, strlen(spec));
}

wchar_t* wcsnistr(wchar_t* string, int len, wchar_t* spec)
{
	return wmemimem(string, len, spec, wcslen(spec));
}

char* strrstr(char* str1, char* str2)
{
	int len = strlen(str1);
	return strnrstr(str1 + len - 1, len, str2);
}

wchar_t* wcsrstr(wchar_t * str1, wchar_t* str2)
{
	int len = wcslen(str1);
	return wcsnrstr(str1 + len - 1, len, str2);
}

char* strnrstr(char* string, int rlen, char* substr)
{
	return memrmem(string, rlen, substr, strlen(substr));
}

wchar_t* wcsnrstr(wchar_t* string, int rlen, wchar_t* substr)
{
	return wmemrmem(string, rlen, substr, wcslen(substr));
}

char* strirstr(char* str1, char* str2)
{
	int len = strlen(str1);
	return strnirstr(str1 + len - 1, len, str2);
}

wchar_t* wcsirstr(wchar_t * str1, wchar_t* str2)
{
	int len = wcslen(str1);
	return wcsnirstr(str1 + len - 1, len, str2);
}

char* strnirstr(char* string, int len, char* spec)
{
	return memrimem(string, len, spec, strlen(spec));
}

wchar_t* wcsnirstr(wchar_t* string, int len, wchar_t* spec)
{
	return wmemrimem(string, len, spec, wcslen(spec));
}

char* strichr(char * string, int ch)
{
	while (*string && (toupper(*string) != toupper(ch)))
		string++;

	if (toupper(*string) == toupper(ch))
		return string;
	return(NULL);
}

wchar_t* wcsichr(wchar_t * string, int ch)
{
	while (*string && (toupper(*string) != toupper(ch)))
		string++;

	if (toupper(*string) == toupper(ch))
		return string;
	return(NULL);
}


char* strnchr(char* string, int len, char ch)
{
	while (len-- > 0)
	{
		if (*string != ch) {
			string++;
		}
		else {
			break;
		}
	}
	return ((len >= 0) ? string : 0);
}

wchar_t* wcsnchr(wchar_t* string, int len, wchar_t ch)
{
	while (len-- > 0)
	{
		if (*string != ch) {
			string++;
		}
		else {
			break;
		}
	}
	return ((len >= 0) ? string : 0);
}

char* strnichr(char* string, int len, char ch)
{
	while (len-- > 0)
	{
		if (toupper(*string) != toupper(ch)) {
			string++;
		}
		else {
			break;
		}
	}
	return ((len >= 0) ? string : 0);
}

wchar_t* wcsnichr(wchar_t* string, int len, wchar_t ch)
{
	while (len-- > 0)
	{
		if (toupper(*string) != toupper(ch)) {
			string++;
		}
		else {
			break;
		}
	}
	return ((len >= 0) ? string : 0);
}

char* strtrimleft(char* string, char* spec)
{
	char* str = string;
	while (*str && strchr(spec, *str))
		str++;

	if (str != string)
		memmove(string, str, strlen(string) - (str - string));

	return string;
}

wchar_t* wcstrimleft(wchar_t* string, wchar_t* spec)
{
	wchar_t* str = string;
	while (*str && wcschr(spec, *str))
		str++;

	if (str != string)
		wmemmove(string, str, wcslen(string) - (str - string));

	return string;
}

char* strntrimleft(char* string, int len, char* spec)
{
	return memtrimleft(string, len, spec, strlen(spec));
}

wchar_t* wcsntrimleft(wchar_t* string, int len, wchar_t* spec)
{
	return wmemtrimleft(string, len, spec, wcslen(spec));
}

char* strtrimright(char* string, char* spec)
{
	int len = strlen(string);
	char* str = string + len;
	while (str != string && strchr(spec, *str))
		str--;

	if (str - string < len)
		*str = 0;

	return string;
}

wchar_t* wcstrimright(wchar_t* string, wchar_t* spec)
{
	int len = wcslen(string);
	wchar_t* str = string + len;
	while (str != string && wcschr(spec, *str))
		str--;

	if (str - string < len)
		*str = 0;

	return string;
}

char* strntrimright(char* string, int len, char* spec)
{
	return memtrimright(string, len, spec, strlen(spec));
}

wchar_t* wcsntrimright(wchar_t* string, int len, wchar_t* spec)
{
	return wmemtrimright(string, len, spec, wcslen(spec));
}

char* strtrim(char* string, char* spec)
{
	return strtrimright(strtrimleft(string, spec), spec);
}

wchar_t* wcstrim(wchar_t* string, wchar_t* spec)
{
	return wcstrimright(wcstrimleft(string, spec), spec);
}

char* strntrim(char* string, int len, char* spec)
{
	return memtrim(string, len, spec, strlen(spec));
}

wchar_t* wcsntrim(wchar_t* string, int len, wchar_t* spec)
{
	return wmemtrim(string, len, spec, wcslen(spec));
}

char* strrep(char* string, char* r, char* t)
{
	int rlen = strlen(r);
	int tlen = strlen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		char* str = string;
		while (*str)
		{
			if (strncmp(str, r, rlen) == 0) {
				strncpy(str, t, tlen);
				if (rtspace) {
					strcpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

wchar_t* wcsrep(wchar_t* string, wchar_t* r, wchar_t* t)
{
	int rlen = wcslen(r);
	int tlen = wcslen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		wchar_t* str = string;
		while (*str)
		{
			if (wcsncmp(str, r, rlen) == 0) {
				wcsncpy(str, t, tlen);
				if (rtspace) {
					wcscpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

char* strnrep(char* string, int len, char* r, char* t)
{
	int rlen = strlen(r);
	int tlen = strlen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		char* str = string;
		while (len-- > 0)
		{
			if (strncmp(str, r, rlen) == 0) {
				strncpy(str, t, tlen);
				if (rtspace) {
					strcpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

wchar_t* wcsnrep(wchar_t* string, int len, wchar_t* r, wchar_t* t)
{
	int rlen = wcslen(r);
	int tlen = wcslen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		wchar_t* str = string;
		while (len-- > 0)
		{
			if (wcsncmp(str, r, rlen) == 0) {
				wcsncpy(str, t, tlen);
				if (rtspace) {
					wcscpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

char* strirep(char* string, char* r, char* t)
{
	int rlen = strlen(r);
	int tlen = strlen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		char* str = string;
		while (*str)
		{
			if (strnicmp(str, r, rlen) == 0) {
				strncpy(str, t, tlen);
				if (rtspace) {
					strcpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

wchar_t* wcsirep(wchar_t* string, wchar_t* r, wchar_t* t)
{
	int rlen = wcslen(r);
	int tlen = wcslen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		wchar_t* str = string;
		while (*str)
		{
			if (wcsnicmp(str, r, rlen) == 0) {
				wcsncpy(str, t, tlen);
				if (rtspace) {
					wcscpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

char* strnirep(char* string, int len, char* r, char* t)
{
	int rlen = strlen(r);
	int tlen = strlen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		char* str = string;
		while (len-- > 0)
		{
			if (strnicmp(str, r, rlen) == 0) {
				strncpy(str, t, tlen);
				if (rtspace) {
					strcpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

wchar_t* wcsnirep(wchar_t* string, int len, wchar_t* r, wchar_t* t)
{
	int rlen = wcslen(r);
	int tlen = wcslen(t);
	int rtspace = rlen - tlen;
	if (rtspace < 0) {
		return 0;
	}
	else {
		wchar_t* str = string;
		while (len-- > 0)
		{
			if (wcsnicmp(str, r, rlen) == 0) {
				wcsncpy(str, t, tlen);
				if (rtspace) {
					wcscpy(str + tlen, str + rlen);
				}
				str += tlen;
			}
			else {
				str++;
			}
		}
		return string;
	}
}

char* strrepchr(char* string, char r, char t)
{
	char* str = string;
	while (*str)
	{
		if (*str == r) {
			*str = t;
		}
		str++;
	}
	return string;
}

wchar_t* wcsrepchr(wchar_t* string, wchar_t r, wchar_t t)
{
	wchar_t* str = string;
	while (*str)
	{
		if (*str == r) {
			*str = t;
		}
		str++;
	}
	return string;
}

char* strnrepchr(char* string, int len, char r, char t)
{
	char* str = string;
	while (len-- > 0)
	{
		if (*str == r) {
			*str = t;
		}
		str++;
	}
	return string;
}

wchar_t* wcsnrepchr(wchar_t* string, int len, wchar_t r, wchar_t t)
{
	wchar_t* str = string;
	while (len-- > 0)
	{
		if (*str == r) {
			*str = t;
		}
		str++;
	}
	return string;
}

char* strirepchr(char* string, char r, char t)
{
	char* str = string;
	while (*str)
	{
		if (toupper(*str) == toupper(r)) {
			*str = t;
		}
		str++;
	}
	return string;
}

wchar_t* wcsirepchr(wchar_t* string, wchar_t r, wchar_t t)
{
	wchar_t* str = string;
	while (*str)
	{
		if (toupper(*str) == toupper(r)) {
			*str = t;
		}
		str++;
	}
	return string;
}

char* strnirepchr(char* string, int len, char r, char t)
{
	char* str = string;
	while (len-- > 0)
	{
		if (toupper(*str) == toupper(r)) {
			*str = t;
		}
		str++;
	}
	return string;
}

wchar_t* wcsnirepchr(wchar_t* string, int len, wchar_t r, wchar_t t)
{
	wchar_t* str = string;
	while (len-- > 0)
	{
		if (toupper(*str) == toupper(r)) {
			*str = t;
		}
		str++;
	}
	return string;
}

char* strskipchr(char* string, char spec)
{
	while (*string && spec == *string)
		string++;

	return (*string ? string : 0);
}

wchar_t* wcsskipchr(wchar_t* string, wchar_t spec)
{
	while (*string && spec == *string)
		string++;

	return (*string ? string : 0);
}

char* strnskipchr(char* string, int len, char spec)
{
	while (len-- && spec == *string)
		string++;

	return ((len >= 0) ? string : 0);
}

wchar_t* wcsnskipchr(wchar_t* string, int len, wchar_t spec)
{
	while (len-- && spec == *string)
		string++;

	return ((len >= 0) ? string : 0);
}

char* strskip(char* string, char* spec)
{
	while (*string && strchr(spec, *string))
		string++;

	return (*string ? string : 0);
}

wchar_t* wcsskip(wchar_t* string, wchar_t* spec)
{
	while (*string && wcschr(spec, *string))
		string++;

	return (*string ? string : 0);
}

char* strnskip(char* string, int len, char* spec)
{
	return memskip(string, len, spec, strlen(spec));
}

wchar_t* wcsnskip(wchar_t* string, int len, wchar_t* spec)
{
	return wmemskip(string, len, spec, wcslen(spec));
}

char* strrskip(char* string, int rlen, char* spec)
{
	return memrskip(string, rlen, spec, strlen(spec));
}

wchar_t* wcsrskip(wchar_t* string, int rlen, wchar_t* spec)
{
	return wmemrskip(string, rlen, spec, wcslen(spec));
}

char* strbrkchr(char* string, char spec)
{
	while (*string && spec != *string)
		string++;

	return (*string ? string : 0);
}

wchar_t* wcsbrkchr(wchar_t* string, wchar_t spec)
{
	while (*string && spec != *string)
		string++;

	return (*string ? string : 0);
}

char* strnbrkchr(char* string, int len, char spec)
{
	while (len-- && spec != *string)
		string++;

	return ((len >= 0) ? string : 0);
}

wchar_t* wcsnbrkchr(wchar_t* string, int len, wchar_t spec)
{
	while (len-- && spec != *string)
		string++;

	return ((len >= 0) ? string : 0);
}

char* strrbrkchr(char* string, int rlen, char spec)
{
	while (rlen-- && spec != *string)
		string--;

	return ((rlen >= 0) ? string : 0);
}

wchar_t* wcsrbrkchr(wchar_t* string, int rlen, wchar_t spec)
{
	while (rlen-- && spec != *string)
		string--;

	return ((rlen >= 0) ? string : 0);
}

char* strbrk(char* string, char* spec)
{
	while (*string && !strchr(spec, *string))
		string++;

	return (*string ? string : 0);
}

wchar_t* wcsbrk(wchar_t* string, wchar_t* spec)
{
	while (*string && !wcschr(spec, *string))
		string++;

	return (*string ? string : 0);
}

char* strnbrk(char* string, int len, char* spec)
{
	return membrk(string, len, spec, strlen(spec));
}

wchar_t* wcsnbrk(wchar_t* string, int len, wchar_t* spec)
{
	return wmembrk(string, len, spec, wcslen(spec));
}

char* strrbrk(char* string, char* spec)
{
	int len = strlen(string);
	return memrbrk(string + len - 1, len, spec, strlen(spec));
}

wchar_t* wcsrbrk(wchar_t* string, wchar_t* spec)
{
	int len = wcslen(string);
	return wmemrbrk(string + len - 1, len, spec, wcslen(spec));
}

char* strnrbrk(char* string, int rlen, char* spec)
{
	return memrbrk(string, rlen, spec, strlen(spec));
}

wchar_t* wcsnrbrk(wchar_t* string, int rlen, wchar_t* spec)
{
	return wmemrbrk(string, rlen, spec, wcslen(spec));
}

#ifdef USE_REGEX
#include <boost/regex/v4/regex.hpp>

bool strnregex(char*src, int srclen, char *dst, int dstlen)
{
	std::string str(src, srclen);
	boost::regex reg(dst, dstlen);
	return boost::regex_match(str, reg);
}

bool wcsnregex(wchar_t*src, int srclen, wchar_t *dst, int dstlen)
{
	std::wstring str(src, srclen);
	boost::wregex reg(dst, dstlen);
	return boost::regex_match(str, reg);
}

bool strregex(char*src, char *dst)
{
	boost::regex reg;
	reg.assign(dst);
	return boost::regex_match(src, reg);
	return false;
}

bool wcsregex(wchar_t*src, wchar_t *dst)
{
	boost::wregex reg;
	reg.assign(dst);
	return boost::regex_match(src, reg);
	return false;
}

#endif//

int mb2wc(const char* src, int srclen, wchar_t* dst, int dstlen, unsigned int cp)
{
#ifdef WIN32
	return ::MultiByteToWideChar(cp, 0, src, srclen, dst, dstlen);
#else
	mbstowcs(dst, src, srclen);
	return dstlen;
#endif//
}
int wc2mb(const wchar_t* src, int srclen, char* dst, int dstlen, unsigned int cp)
{
#ifdef WIN32
	return ::WideCharToMultiByte(cp, 0, src, srclen, dst, dstlen, NULL, NULL);
#else
	//switch (cp)
	//{
	//case CP_ACP:
	//	setlocale(LC_ALL, "C");     //C
	//	break;
	//case CP_OEM:
	//	setlocale(LC_ALL, "");     //设置本地默认Locale
	//	break;
	//case CP_UTF8:
	//	setlocale(LC_ALL, "zh_CN.utf8");
	//	break;
	//case CP_GBK:
	//	setlocale(LC_ALL, "zh_CN.gbk");     //设置本地默认Locale
	//	break;
	//}
	wcstombs(dst, src, srclen);
	//setlocale(LC_ALL, "C");     //C
	//setlocale(LC_ALL, "");     //设置本地默认Locale
	return dstlen;
#endif//
}

void byte2hex(byte by, char* hex)
{
	for (size_t j = 0; j < 2; j++)
	{
		char c = (by & 0x0f);
		if (c < 10)
		{
			c += '0';
		}
		else
		{
			c += ((false ? 'A' : 'a') - 10);
		}
		hex[1 - j] = c;
		by >>= 4;
	}
}

void buf2hex(const byte* src, int srclen, char* dst, int dstlen)
{
	BOOST_ASSERT(dstlen >= 2 * srclen);
	for (size_t i = 0; i < srclen; i++)
	{
		byte2hex(src[i], dst + 2 * i);
	}
}

byte hex2byte(const char* hex)
{
	byte by = 0;
	for (size_t j = 0; j < 2; j++)
	{
		char c = hex[j];
		if (c >= '0' && c <= '9')
		{
			by = (by << 4) + (c - '0');
		}
		else if (c >= 'a' && c <= 'f')
		{
			by = (by << 4) + (c - 'a' + 10);
		}
		else if (c >= 'A' && c <= 'F')
		{
			by = (by << 4) + (c - 'A' + 10);
		}
		else
		{
			BOOST_ASSERT(0);
		}
	}
	return by;
}
//
//BOOL RectInRect(LPRECT lprc1, LPRECT lprc2)
//{
//	if (lprc1->left <= lprc2->left &&
//		lprc1->right >= lprc2->right &&
//		lprc1->top <= lprc2->top &&
//		lprc1->bottom >= lprc2->bottom) {
//		return TRUE;
//	}
//	return FALSE;
//}
//
//RECT String2RectA(const char* lpszRect)
//{
//	ASSERT(lpszRect);
//	RECT rc = { 0 };
//	char* psz = (char*)lpszRect;
//	char* pszNext = strbrk(psz, ", \t");
//	if (pszNext) {
//		rc.left = atoi(psz);
//		psz = strskip(pszNext, ", \t");
//		rc.top = atoi(psz);
//		pszNext = strbrk(psz, ", \t");
//		psz = strskip(pszNext, ", \t");
//		rc.right = atoi(psz);
//		pszNext = strbrk(psz, ", \t");
//		psz = strskip(pszNext, ", \t");
//		rc.bottom = atoi(psz);
//	}
//	return rc;
//}
//
//RECT String2RectW(const wchar_t* lpszRect)
//{
//	ASSERT(lpszRect);
//	RECT rc = { 0 };
//	wchar_t* psz = (wchar_t*)lpszRect;
//	wchar_t* pszNext = wcsbrk(psz, L", \t");
//	if (pszNext) {
//		rc.left = _wtoi(psz);
//		psz = wcsskip(pszNext, L", \t");
//		rc.top = _wtoi(psz);
//		pszNext = wcsbrk(psz, L", \t");
//		psz = wcsskip(pszNext, L", \t");
//		rc.right = _wtoi(psz);
//		pszNext = wcsbrk(psz, L", \t");
//		psz = wcsskip(pszNext, L", \t");
//		rc.bottom = _wtoi(psz);
//	}
//	return rc;
//}
//
//char* Rect2StringA(char* str, LPCRECT lpRect)
//{
//	if (!lpRect || !str) {
//		return NULL;
//	}
//	sprintf(str, "%d,%d,%d,%d", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
//	return str;
//}
//
//wchar_t* Rect2StringW(wchar_t* str, LPCRECT lpRect)
//{
//	if (!lpRect || !str) {
//		return NULL;
//	}
//	swprintf(str, L"%d,%d,%d,%d", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
//	return str;
//}

uint32_t strtocolor(const char* str)
{
	ASSERT(str);
	char* psz = (char*)str;
	char* pszNext = strbrk(psz, ", \t");
	if (pszNext) {
		int Pos[3] = { 0 };
		Pos[0] = atoi(psz);
		psz = strskip(pszNext, ", \t");
		Pos[1] = atoi(psz);
		pszNext = strbrk(psz, ", \t");
		psz = strskip(pszNext, ", \t");
		Pos[2] = atoi(psz);
		return rgb(Pos[0], Pos[1], Pos[2]);
	}
	return atoi(psz);
}

uint32_t wcstocolor(const wchar_t* str)
{
#ifdef WIN32
	ASSERT(str);
	wchar_t* psz = (wchar_t*)str;
	wchar_t* pszNext = wcsbrk(psz, L", \t");
	if (pszNext) {
		int Pos[3] = { 0 };
		Pos[0] = _wtoi(psz);
		psz = wcsskip(pszNext, L", \t");
		Pos[1] = _wtoi(psz);
		pszNext = wcsbrk(psz, L", \t");
		psz = wcsskip(pszNext, L", \t");
		Pos[2] = _wtoi(psz);
		return rgb(Pos[0], Pos[1], Pos[2]);
	}
	return _wtoi(psz);
#else
	return 0;
#endif
}

char* colortostr(char* str, uint32_t cr)
{
	if (!str) {
		return NULL;
	}
	sprintf(str, "%d,%d,%d", rgbr(cr), rgbg(cr), rgbb(cr));
	return str;
}

wchar_t* colortowcs(wchar_t* str, uint32_t cr)
{
	if (!str) {
		return NULL;
	}
	swprintf(str, L"%d,%d,%d", rgbr(cr), rgbg(cr), rgbb(cr));
	return str;
}

uint32_t strhextouint32(const char* val, int len)
{
	uint32_t uval = 0;
	const char* pch = val;
	while (*pch && len != 0)
	{
		uval <<= 4;

		if ('a' <= *pch && 'f' >= *pch)
			uval |= (*pch - 'a' + 10);
		else if ('A' <= *pch && 'F' >= *pch)
			uval |= (*pch - 'A' + 10);
		else if ('0' <= *pch && '9' >= *pch)
			uval |= (*pch - '0');
		else
			return 0;

		++pch;
		--len;
	}
	return uval;
}

uint32_t wcshextouint32(const wchar_t* val, int len)
{
	uint32_t uval = 0;
	const wchar_t* pch = val;
	while (*pch && len != 0)
	{
		uval <<= 4;

		if (L'a' <= *pch && L'f' >= *pch)
			uval |= (*pch - L'a' + 10);
		else if (L'A' <= *pch && L'F' >= *pch)
			uval |= (*pch - L'A' + 10);
		else if (L'0' <= *pch && L'9' >= *pch)
			uval |= (*pch - L'0');
		else
			return 0;

		++pch;
		--len;
	}
	return uval;
}

uint32_t strhextouint32(const char* val)
{
	if (strnicmp(val, "0x", 2) == 0) {
		return strhextouint32(val + 2, strlen(val + 2));
	}
	else {
		return atoi(val);
	}
}

uint32_t wcshextouint32(const wchar_t* val)
{
	if (wcsnicmp(val, L"0x", 2) == 0) {
		return wcshextouint32(val + 2, wcslen(val + 2));
	}
	else {
#ifdef WIN32
		return _wtoi(val);
#else
		return 0;
#endif
	}
}

char* uint32tostrhex(char* str, uint32_t val)
{
	if (!str) {
		return NULL;
	}
	sprintf(str, "0x%x", val);
	return str;
}

wchar_t* uint32towcshex(wchar_t* str, uint32_t val)
{
	if (!str) {
		return NULL;
	}
	swprintf(str, L"0x%x", val);
	return str;
}

uint32_t strhextocolor(const char* val)
{
	if (strnicmp(val, "0x", 2) == 0) {
		return rgb(
			strhextouint32(val + 2, 2),
			strhextouint32(val + 4, 2),
			strhextouint32(val + 6, 2)
		);
	}
	else {
		return atoi(val);
	}
}

uint32_t wcshextocolor(const wchar_t* val)
{
	if (wcsnicmp(val, L"0x", 2) == 0) {
		return rgb(
			wcshextouint32(val + 2, 2),
			wcshextouint32(val + 4, 2),
			wcshextouint32(val + 6, 2)
		);
	}
	else {
#ifdef WIN32
		return _wtoi(val);
#else
		return 0;
#endif
	}
}

