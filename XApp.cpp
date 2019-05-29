#include "XApp.hpp"

namespace XUtil {

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

	XApp* XApp::_inst = nullptr;

XApp::XApp() { _inst = this; }
		XApp::~XApp() { }

		bool XApp::init(char *xml, int xmlflag)
		{
			Base::init(xml, xmlflag);
			boost::filesystem::path cur_path = boost::filesystem::current_path();
			boost::filesystem::path app_path = boost::dll::program_location();
			if (name_.empty())
			{
				name_ = app_path.stem().string();
				cfg_.put("name", name_);
			}
			if (work_path_.empty())
			{
				work_path_ = cur_path;// app_path.parent_path();
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

		void XApp::term()
		{
			
		}

} // namespace XUtil
