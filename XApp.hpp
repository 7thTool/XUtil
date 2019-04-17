#ifndef _H_XAPP_HPP_
#define _H_XAPP_HPP_

#pragma once

#include "XApi.hpp"

#define theApp XUtil::XApp::instance()

namespace XUtil
{
	class XApiEx : public XApi
	{
	public:
		bool init(char *xml, int xmlflag);
	};

	class XApp : public XApiEx
	{
		typedef XApiEx Base;
	protected:
		static XApp* _inst;
	public:
		static XApp& instance() { return *_inst; }

		XApp();
		~XApp();

		bool init(char *xml, int xmlflag);
		void term();
	};
}

#endif//_H_XAPP_HPP_

