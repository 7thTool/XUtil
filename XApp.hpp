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

		XApp() { _inst = this; }
		~XApp() { }

		bool init(char *xml, int xmlflag)
		{
			Base::init(xml, xmlflag);
			boost::filesystem::path app_path = boost::dll::program_location();
			if (name_.empty())
			{
				name_ = app_path.stem().string();
				cfg_.put("name", name_);
			}
			if (work_path_.empty())
			{
				work_path_ = app_path.parent_path();
				cfg_.put("work_path", work_path_.string());
			}
			if (data_path_.empty())
			{
				data_path_ = work_path_;
				data_path_.append("data");
				if (!boost::filesystem::exists(data_path_))
				{
					boost::filesystem::create_directory(data_path_);
				}
				cfg_.put("data_path", data_path_.string());
			}
			return true;
		}

		void term()
		{
			
		}
	};

	XApp* XApp::_inst = nullptr;

		bool XApiEx::init(char *xml, int xmlflag)
		{
			XApi::init(xml, xmlflag);
			if (work_path_.empty())
			{
				work_path_ = theApp.work_path();
			}
			if (data_path_.empty())
			{
				data_path_ = theApp.data_path();
			}
			return true;
		}

}

#endif//_H_XAPP_HPP_

