#ifndef _H_XSTR_HPP_
#define _H_XSTR_HPP_

#include "XStr.h"
#include "XUtil.hpp"
#include <boost/locale.hpp>
//#include <boost/lexical_cast.hpp>  

namespace XUtil {
	
	inline void trimleft(std::string& str)
	{
		std::string::iterator it = std::find_if(str.begin(), str.end(), not1(std::ptr_fun(::isspace)));
		str.erase(str.begin(), it);
	}

	inline void trimright(std::string& str)
	{
		std::string::reverse_iterator rit = std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(::isspace)));
		str.erase(rit.base(), str.end());
	}

	inline void wtrimleft(std::wstring& str)
	{
		std::wstring::iterator it = std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace)));
		str.erase(str.begin(), it);
	}

	inline void wtrimright(std::wstring& str)
	{
		std::wstring::reverse_iterator rit = std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(::isspace)));
		str.erase(rit.base(), str.end());
	}

	struct stringiequal
	{
		bool operator()(const std::string& s1, const std::string& s2) const
		{
			return stricmp(s1.c_str(), s2.c_str()) == 0;
		}
	};
	struct wstringiequal
	{
		bool operator()(const std::wstring& s1, const std::wstring& s2) const
		{
			return wcsicmp(s1.c_str(), s2.c_str()) == 0;
		}
	};
	struct stringiless
	{
		bool operator()(const std::string& s1, const std::string& s2) const
		{
			return stricmp(s1.c_str(), s2.c_str()) < 0;
		}
	};
	struct wstringiless
	{
		bool operator()(const std::wstring& s1, const std::wstring& s2) const
		{
			return wcsicmp(s1.c_str(), s2.c_str()) < 0;
		}
	};

	struct striequal {
		bool operator() (const char* src, const char* dst) const {
			return stricmp(src, dst) == 0;
		}
	};
	struct strequal {
		bool operator() (const char* src, const char* dst) const {
			return strcmp(src, dst) == 0;
		}
	};
	struct wcsiequal {
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcsicmp(src, dst) == 0;
		}
	};
	struct wcsequal {
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcscmp(src, dst) == 0;
		}
	};
	struct striless {
		bool operator() (const char* src, const char* dst) const {
			return stricmp(src, dst) < 0;
		}
	};
	struct strless {
		bool operator() (const char* src, const char* dst) const {
			return strcmp(src, dst) < 0;
		}
	};
	struct wcsiless {
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcsicmp(src, dst) < 0;
		}
	};
	struct wcsless {
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcscmp(src, dst) < 0;
		}
	};

	template<typename Target>
	inline Target strto(const std::string& arg, const Target& def = Target())
	{
		if (!arg.empty()) {
			try
			{
				Target o;
				std::stringstream ss;
				ss << arg;
				ss >> o;
				return o;
			}
			catch(std::exception& e)
			{

			}
			catch (...)
			{

			}
		}
		return def;
	}

	template<typename Target>
	inline Target wcsto(const std::wstring& arg, const Target& def = Target())
	{
		if (!arg.empty()) {
			try 
			{
				Target o;
				std::wstringstream ss;
				ss << arg;
				ss >> o;
				return o;
			}
			catch(std::exception& e)
			{

			}
			catch (...)
			{

			}
		}
		return def;
	}

	template<typename Source>
	inline std::string tostr(const Source& arg)
	{
		std::ostringstream ss;
		ss << arg;
		return ss.str();
	}

	template<typename Source>
	inline std::wstring towcs(const Source& arg)
	{
		std::wostringstream ss;
		ss << arg;
		return ss.str();
	}

	template<typename Source>
	inline std::string tostrex(const Source& arg, int p = -1, int w = -1, char c = '0')
	{
		std::ostringstream ss;
		if (p>=0) {
			ss.setf(std::ios::fixed);
			ss.precision(p);
		}
		if (w>=0) {
			ss.width(w);
			ss.fill(c);
		}
		ss << arg;
		return ss.str();
	}

	template<typename Source>
	inline std::wstring towcsex(const Source& arg, int p = -1, int w = -1, wchar_t c = '0')
	{
		std::wostringstream ss;
		if (p>=0) {
			ss.setf(std::ios::fixed);
			ss.precision(p);
		}
		if (w>=0) {
			ss.width(w);
			ss.fill(c);
		}
		ss << arg;
		return ss.str();
	}

	//"UTF-8"
	//"GBK"
	//"BIG5"

	#ifdef WIN32
	inline std::string get_charset_by_codepage(int cp)
	{
		CPINFOEX cpinfo = {0};
		GetCPInfoEx(cp, 0, &cpinfo);
		cpinfo.CodePageName;
		return "CP" + boost::lexical_cast<std::string>(cpinfo.CodePage);
	}
	#endif//

	inline std::string get_system_charset()
	{
		static std::string codepage;
		if (!codepage.empty()) {
			return codepage;
		}
	#ifdef WIN32 
		codepage = get_charset_by_codepage(CP_OEMCP);
	#else  
		std::locale loc = boost::locale::generator().generate("");
		codepage = std::use_facet<boost::locale::info>(loc).encoding();
	#endif  
		return codepage;
	}

	inline std::string mb2mb(const std::string & str, const std::string& from_charset, const std::string& to_charset)
	{
		return boost::locale::conv::between(str, to_charset, from_charset);
	}

	inline std::string utf82mb(const std::string& str, const std::string& charset) 
	{
		return boost::locale::conv::from_utf(str, charset.empty() ? get_system_charset() : charset);
	}

	inline std::string mb2utf8(const std::string& str, const std::string& charset) 
	{
		return boost::locale::conv::to_utf<char>(str, charset.empty() ? get_system_charset() : charset);
	}

	inline std::string utf82mb(const std::string& str)
	{
		return boost::locale::conv::from_utf(str, get_system_charset());
	}

	inline std::string mb2utf8(const std::string& str)
	{
		return boost::locale::conv::to_utf<char>(str, get_system_charset());
	}
	
	inline std::string wc2mb(const std::wstring& str, std::string const & charset)
	{
		return boost::locale::conv::from_utf(str, charset.empty() ? get_system_charset() : charset);
	}

	inline std::wstring mb2wc(const std::string& str, std::string const & charset)
	{
		return boost::locale::conv::to_utf<wchar_t>(str , charset.empty() ? get_system_charset() : charset);
	}

	inline std::string wc2mb(const std::wstring& str)
	{
		return boost::locale::conv::from_utf(str, get_system_charset());
	}

	inline std::wstring mb2wc(const std::string& str)
	{
		return boost::locale::conv::to_utf<wchar_t>(str, get_system_charset());
	}

	template<class tfrom, class tto>
	class str_conv
	{
	public:
		tfrom from_;
		tto to_;
	#ifdef WIN32
		//
	#else
		iconv_t cd_;
	#endif

		str_conv(tfrom from, tto to):from_(from),to_(to)
		{
	#ifdef WIN32
		//
	#else
			cd_ = iconv_open(to_, from_);
			//if (cd == 0)
			//	return -1;
	#endif
		}
		~str_conv()
		{
	#ifdef WIN32
		//
	#else
			iconv_close(cd_);
	#endif//
		}

		int to(const char* src, int srclen, char* dst, int dstlen)
		{
		#ifdef WIN32
			wchar_t* wdst = 0;
			int wdstlen = 0;
			wchar_t wbuf[1024] = {0};
			if(srclen < 1024) {
				wdst = wbuf;
				wdstlen = 1024;
			} else {
				wdst = (wchar_t*)malloc(srclen*sizeof(wchar_t));
				wdstlen = srclen;
			}
			wdstlen = ::MultiByteToWideChar(from_, 0, src, srclen, wdst, wdstlen);
			dstlen = ::WideCharToMultiByte(to_, 0, wdst, wdstlen, dst, dstlen, NULL, NULL);
			if(srclen < 1024) {
				//
			} else {
				free(wdst);
			}
			return dstlen;
		#else
			size_t srcleft = srclen;
			size_t dstleft = dstlen;
			iconv(cd, &src, &srcleft, &dst, &dstleft);
			return srclen-srcleft;
		#endif
		}
	};

	inline int gbk2utf8(const char* src, int srclen, char* dst, int dstlen)
	{
	#ifdef WIN32
		static str_conv<UINT,UINT> conv_(CP_GBK,CP_UTF8);
	#else
		static str_conv<const char*,const char*> conv_("gbk","utf8");
	#endif
		return conv_.to(src, srclen, dst, dstlen);
	}

	inline int utf82gbk(const char* src, int srclen, char* dst, int dstlen)
	{
	#ifdef WIN32
		static str_conv<UINT,UINT> conv_(CP_UTF8,CP_GBK);
	#else
		static str_conv<const char*,const char*> conv_("utf8","gbk");
	#endif
		return conv_.to(src, srclen, dst, dstlen);
	}

	inline const std::string& buf2hex(const std::string &src, std::string& dst)
	{
		dst.resize(src.size() * 2);
		for (size_t i = 0; i < src.size(); i++)
		{
	#if 0
			buf2hex((const byte*)src.c_str(), src.size(), (char*)dst.c_str(), dst.size());
	#else
			uint8_t cTemp = src[i];
			for (size_t j = 0; j < 2; j++)
			{
				uint8_t cCur = (cTemp & 0x0f);
				if (cCur < 10)
				{
					cCur += '0';
				}
				else
				{
					cCur += ((false ? 'A' : 'a') - 10);
				}
				dst[2 * i + 1 - j] = cCur;
				cTemp >>= 4;
			}
	#endif//
		}
		return dst;
	}

	inline const std::string& hex2buf(const std::string &src, std::string& dst)
	{
		BOOST_ASSERT(src.size() % 2 == 0);
		dst.resize(src.size() / 2);
		for (size_t i = 0; i < dst.size(); i++)
		{
	#if 0
			dst[i] = hex2byte(&src[2 * i]);
	#else
			uint8_t cTemp = 0;
			for (size_t j = 0; j < 2; j++)
			{
				char cCur = src[2 * i + j];
				if (cCur >= '0' && cCur <= '9')
				{
					cTemp = (cTemp << 4) + (cCur - '0');
				}
				else if (cCur >= 'a' && cCur <= 'f')
				{
					cTemp = (cTemp << 4) + (cCur - 'a' + 10);
				}
				else if (cCur >= 'A' && cCur <= 'F')
				{
					cTemp = (cTemp << 4) + (cCur - 'A' + 10);
				}
				else
				{
					BOOST_ASSERT(0);
				}
			}
			dst[i] = cTemp;
	#endif//
		}
		return dst;
	}
}

#endif//_H_XSTR_HPP_