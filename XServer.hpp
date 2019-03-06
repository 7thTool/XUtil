#pragma once

#include "XApi.hpp"
#include "XLogger.hpp"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace XUtil {

template<class T>
class XServer : public XApp
{
	typedef XApp Base;
public:
	XServer():io_service_()
	{

	}

	virtual ~XServer()
	{
		
	}

	inline boost::asio::io_service& service() { return io_service_; }

	bool init(int argc, char *argv[])
	{
		Base::init(argv[argc - 1], XML_FLAG_JSON_FILE);
		LOG4I("server=%s work_path=%s data_path_=%s", name_.c_str(), work_path_.string().c_str(), data_path_.string().c_str());

		boost::property_tree::ptree &app_cfg = cfg();
		
		return true;
	}

	void term()
	{
		LOG4I("term...\n");
		Base::term();
	}

	void stop()
	{
		//boost::system::error_code ec;
		
		io_service_.stop();
		io_service_.reset();
	}
	
	bool run()
	{
// 有两个信号可以停止进程:SIGTERM和SIGKILL。 SIGTERM比较友好，进程能捕捉这个信号，根据您的需要来关闭程序。在关闭程序之前，您可以结束打开的记录文件和完成正在做的任务。在某些情况下，假如进程正在进行作业而且不能中断，那么进程可以忽略这个SIGTERM信号。
// 对于SIGKILL信号，进程是不能忽略的。这是一个 “我不管您在做什么,立刻停止”的信号。假如您发送SIGKILL信号给进程，Linux就将进程停止在那里。
		// The signal_set is used to register for process termination notifications.
    	boost::asio::signal_set signals(io_service_);
		signals.add(SIGINT);
   		signals.add(SIGTERM);
#if defined(SIGQUIT)
    	signals.add(SIGQUIT);
#endif	
		T* pT = static_cast<T*>(this);
    	signals.async_wait(boost::bind(&T::stop, pT));

		io_service_.run();

		return true;
	}

private:
	boost::asio::io_service io_service_;
};

}
