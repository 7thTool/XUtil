#include "XController.hpp"

#if defined(__cplusplus)
extern "C" {
#endif//

    void XController_SetHandler(const char *name, void *handler)
    {
        XUtil::XController::instance().SetHandler(name, handler);
    }

    void *XController_GetHandler(const char *name)
    {
        return XUtil::XController::instance().GetHandler(name);
    }

    void XController_SetThreadHandler(const char *name, void *handler)
    {
        XUtil::XController::instance().SetThreadHandler(name, handler);
    }

    void *XController_GetThreadHandler(const char *name)
    {
        return XUtil::XController::instance().GetThreadHandler(name);
    }

    void XController_SetData(const char *name, void *data)
    {
        XUtil::XController::instance().SetData(name, data);
    }

    void *XController_GetData(const char *name)
    {
        return XUtil::XController::instance().GetData(name);
    }

    void XController_SetThreadData(const char *name, void *data)
    {
        XUtil::XController::instance().SetThreadData(name, data);
    }

    void *XController_GetThreadData(const char *name)
    {
        return XUtil::XController::instance().GetThreadData(name);
    }

#if defined(__cplusplus)
}

namespace XUtil {

XController &XController::instance()
{
    static XControllerImpl _inst;
    return _inst;
}

thread_local std::map<std::string, void *> XControllerImpl::_map_handlers;
thread_local std::map<std::string, void *> XControllerImpl::_map_datas;

void XControllerImpl::start(char* xml, int xmlflag)
{
    cfg_from_xml(xml, xmlflag, cfg_);
	name_ = cfg_.get<std::string>("name", "");
	work_path_ = cfg_.get<std::string>("work_path", "");
	data_path_ = cfg_.get<std::string>("data_path", "");
    auto opt_module = cfg_.get_child_optional("module");
    if (opt_module)
    {
        boost::property_tree::ptree &cfg_module = opt_module.get();
        BOOST_FOREACH (const boost::property_tree::ptree::value_type &cfgi, cfg_module)
        {
            try
            {
                boost::filesystem::path path_module = work_path_;
                std::string name = cfgi.second.get<std::string>("name", "");
                std::string data = cfgi.second.get<std::string>("data", "");
                path_module.append(name);
                std::shared_ptr<boost::dll::shared_library> lib = std::make_shared<boost::dll::shared_library>(path_module);
                if (lib->is_loaded())
                {
                    modules_.emplace_back(lib);
                    if (lib->has("Start")) {
                        auto& Start = lib->get<void(char*)>("Start");
                        Start((char*)data.c_str());
                    }
                }
            }
            catch (boost::system::system_error &ex)
            {
                PRINTFL("%s", ex.what());
            }
            catch (std::bad_alloc &ex)
            {
                PRINTFL("%s", ex.what());
            }
            catch (...)
            {
            }
        }
    }
}

void XControllerImpl::stop()
{
    for (size_t i = 0; i < modules_.size(); i++)
	{
        std::shared_ptr<boost::dll::shared_library> lib = modules_[i];
        if (lib->is_loaded()) {
			if (lib->has("Stop")) {
				auto& Stop = lib->get<void()>("Stop");
				Stop();
			}
		}
	}
    //modules_.clear();
}

const char* XControllerImpl::work_path()
{
    return work_path_.c_str();
}

const char* XControllerImpl::data_path()
{
    return data_path_.c_str();
}

void XControllerImpl::SetHandler(const char *name, void *handler)
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (handler)  {
        map_handlers_.erase(name);
    } else {
        map_handlers_[name] = handler;
    }
}

void *XControllerImpl::GetHandler(const char *name)
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    auto it = map_handlers_.find(name);
    if (it != map_handlers_.end())  {
        return it->second;
    }
    return nullptr;
}

void XControllerImpl::SetThreadHandler(const char *name, void *handler)
{
    // if(!_p_map_handlers.get()) {
    //     std::unique_lock<std::recursive_mutex> lock(mutex_);
    //     _p_map_handlers.reset(&map_thread_handlers_[std::this_thread::get_id()]);
    // }
    // auto & _map_handlers = *_p_map_handlers.get();
    if (handler) {
        _map_handlers.erase(name);
    } else {
        _map_handlers[name] = handler;
    }
}

void *XControllerImpl::GetThreadHandler(const char *name)
{
    // auto p = _p_map_handlers.get();
    // if(p) {
    //     auto & _map_handlers = *p;
        auto it = _map_handlers.find(name);
        if (it != _map_handlers.end()) {
            return it->second;
        }
    // }
    return nullptr;
}

void XControllerImpl::SetData(const char *name, void *data)
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (data) {
        map_datas_.erase(name);
    } else {
        map_datas_[name] = data;
    }
}

void *XControllerImpl::GetData(const char *name)
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    auto it = map_datas_.find(name);
    if (it != map_datas_.end()) {
        return it->second;
    }
    return nullptr;
}

void XControllerImpl::SetThreadData(const char *name, void *data)
{
    // if(!_p_map_datas.get()) {
    //     std::unique_lock<std::recursive_mutex> lock(mutex_);
    //     _p_map_datas.reset(&map_thread_datas_[std::this_thread::get_id()]);
    // }
    // auto & _map_datas = *_p_map_datas.get();
    if (data) {
        _map_datas.erase(name);
    } else {
        _map_datas[name] = data;
    }
}

void *XControllerImpl::GetThreadData(const char *name)
{
    // auto p = _p_map_datas.get();
    // if(p) {
    //     auto & _map_datas = *p;
        auto it = _map_datas.find(name);
        if (it != _map_datas.end()) {
            return it->second;
        }
    // }
    return nullptr;
}

}

#endif//__cplusplus
