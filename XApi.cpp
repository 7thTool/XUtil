#include "XApi.hpp"

namespace XUtil {

XApp *theApp = nullptr;

XApi::XApi() {}
XApi::~XApi() {}

bool XApi::init(char *xml, int xmlflag)
{
    cfg_from_xml(xml, xmlflag, cfg_);
    boost::filesystem::path app_path = boost::dll::program_location();
    name_ = cfg_.get<std::string>("name", "");
    work_path_ = cfg_.get<std::string>("work_path", "");
    if (work_path_.empty())
    {
        work_path_ = theApp->work_path();
    }
    data_path_ = cfg_.get<std::string>("data_path", "");
    if (data_path_.empty())
    {
        data_path_ = theApp->data_path();
    }
    return true;
}

void XApi::term()
{
}

XApp::XApp() {}
XApp::~XApp() {}

bool XApp::init(char *xml, int xmlflag)
{
    cfg_from_xml(xml, xmlflag, cfg_);
    boost::filesystem::path app_path = boost::dll::program_location();
    name_ = cfg_.get<std::string>("name", "");
    if (name_.empty())
    {
        name_ = app_path.stem().string();
    }
    work_path_ = cfg_.get<std::string>("work_path", "");
    if (work_path_.empty())
    {
        work_path_ = app_path.parent_path();
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
    }

    return true;
}

void XApp::term()
{
}

} // namespace XUtil
