#include "server.h"
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <XStr.h>
#include <XHelper.h>

	server::server():io_service_()
	{

	}

	server::~server()
	{
		
	}

	bool server::run()
	{
// 有两个信号可以停止进程:SIGTERM和SIGKILL。 SIGTERM比较友好，进程能捕捉这个信号，根据您的需要来关闭程序。在关闭程序之前，您可以结束打开的记录文件和完成正在做的任务。在某些情况下，假如进程正在进行作业而且不能中断，那么进程可以忽略这个SIGTERM信号。
// 对于SIGKILL信号，进程是不能忽略的。这是一个 “我不管您在做什么,立刻停止”的信号。假如您发送SIGKILL信号给进程，Linux就将进程停止在那里。
		// The signal_set is used to register for process termination notifications.
    	boost::asio::signal_set signals(*io_service_);
		signals.add(SIGINT);
   		signals.add(SIGTERM);
#if defined(SIGQUIT)
    	signals.add(SIGQUIT);
#endif
    	signals.async_wait(boost::bind(&server::on_term,this));

		io_service_->run();

		return true;
	}

	void server::on_term()
	{
		io_service_.stop();
		io_service_.reset();
	}


	