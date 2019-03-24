#pragma once
#ifndef _H_XCONTROLLER_IMPL_HPP_
#define _H_XCONTROLLER_IMPL_HPP_

#include "XController.h"
#include "XUtil.hpp"
#include "XXml.hpp"

namespace XUtil
{
	class XControllerImpl : public XController
	{
	protected:
		boost::property_tree::ptree cfg_;
		std::string name_;
		std::string work_path_;
		std::string data_path_;
		std::vector<std::shared_ptr<boost::dll::shared_library>> modules_;
		std::map<std::string,void*> map_handlers_;
		std::map<std::string,void*> map_datas_;
		//std::map<std::thread::id,std::map<std::string,void*>> map_thread_handlers_;
		//std::map<std::thread::id,std::map<std::string,void*>> map_thread_datas_;
		std::recursive_mutex mutex_;
		static thread_local std::map<std::string, void *> _map_handlers;
		static thread_local std::map<std::string, void *> _map_datas;
		//boost::thread_specific_ptr<std::map<std::string, void *>> _p_map_datas;
	public:

		void start(char* xml, int xmlflag = 0);
		void stop();

		const char* work_path();
		const char* data_path();
    
		void SetHandler(const char* name, void* handler);
		void* GetHandler(const char* name);

		void SetThreadHandler(const char* name, void* handler);
		void* GetThreadHandler(const char* name);

		void SetData(const char* name, void* data);
		void* GetData(const char* name);

		void SetThreadData(const char* name, void* data);
		void* GetThreadData(const char* name);
	};
}

#endif//_H_XCONTROLLER_IMPL_HPP_

