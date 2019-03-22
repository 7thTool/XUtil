#ifndef _H_XAPI_HPP_
#define _H_XAPI_HPP_

#pragma once

#include "XUtil.hpp"
#include "XXml.hpp"

namespace XUtil
{
	class XUTIL_API XApi
	{
	protected:
		std::string name_;
		boost::filesystem::path work_path_;
		boost::filesystem::path data_path_;
		boost::property_tree::ptree cfg_;
	public:
		XApi();
		virtual ~XApi();

		virtual bool init(char* xml, int xmlflag = 0);
		virtual void term();

		inline const std::string& name() { return name_; }
		inline const boost::filesystem::path& work_path() { return work_path_; }
		inline const boost::filesystem::path& data_path() { return data_path_; }
		inline boost::property_tree::ptree& cfg() { return cfg_; }
	};

	class XUTIL_API XApp : public XApi
	{
	public:
		XApp();
		~XApp();

		bool init(char* xml, int xmlflag = 0);
		void term();
	};
}

extern XUtil::XApp* theApp;

#endif//_H_XApI_H_

