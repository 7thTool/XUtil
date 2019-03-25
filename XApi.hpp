#ifndef _H_XAPI_HPP_
#define _H_XAPI_HPP_

#pragma once

#include "XUtil.hpp"
#include "XXml.hpp"

#define theApp XUtil::XApp::instance()

namespace XUtil
{
	class XApi
	{
	protected:
		boost::property_tree::ptree cfg_;
		std::string name_;
		boost::filesystem::path work_path_;
		boost::filesystem::path data_path_;
	public:
		XApi() {}
		~XApi() {}

		bool init(char *xml, int xmlflag);
		void term()
		{
		}

		inline const std::string& name() { return name_; }
		inline const boost::filesystem::path& work_path() { return work_path_; }
		inline const boost::filesystem::path& data_path() { return data_path_; }
		inline boost::property_tree::ptree& cfg() { return cfg_; }
	};

	class XApp : public XApi
	{
	protected:
		static XApp* _inst;
	public:
		static XApp& instance() { return *_inst; }

		XApp() { _inst = this; }
		~XApp() { }

		bool init(char *xml, int xmlflag)
		{
			cfg_from_xml(xml, xmlflag, cfg_);
			boost::filesystem::path app_path = boost::dll::program_location();
			name_ = cfg_.get<std::string>("name", "");
			if (name_.empty())
			{
				name_ = app_path.stem().string();
				cfg_.put("name", name_);
			}
			work_path_ = cfg_.get<std::string>("work_path", "");
			if (work_path_.empty())
			{
				work_path_ = app_path.parent_path();
				cfg_.put("work_path", work_path_.string());
			}
			data_path_ = cfg_.get<std::string>("data_path", "");
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

		bool XApi::init(char *xml, int xmlflag)
		{
			cfg_from_xml(xml, xmlflag, cfg_);
			boost::filesystem::path app_path = boost::dll::program_location();
			name_ = cfg_.get<std::string>("name", "");
			work_path_ = cfg_.get<std::string>("work_path", "");
			if (work_path_.empty())
			{
				work_path_ = theApp.work_path();
			}
			data_path_ = cfg_.get<std::string>("data_path", "");
			if (data_path_.empty())
			{
				data_path_ = theApp.data_path();
			}
			return true;
		}

}

#endif//_H_XApI_H_

