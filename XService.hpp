#ifndef __H_XUTIL_XSERVICE_HPP__
#define __H_XUTIL_XSERVICE_HPP__

#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/date_time.hpp>

namespace XUtil {

class XService
{
  public:
	XService() : service_(), work_(service_)
	{
	}

	~XService()
	{
	}

	bool start()
	{
		//service_ = std::make_shared<boost::asio::io_service>();
		//work_ = std::make_shared<boost::asio::io_service::work>(*service_);
		//strand_ = std::make_shared<boost::asio::io_service::strand>(*service_);
		thread_ = std::make_shared<std::thread>(boost::bind(&boost::asio::io_service::run, &service_));

		return true;
	}

	void stop()
	{
		service_.stop();
		if (thread_) {
			thread_->join();
			thread_.reset();
		}
		service_.reset();
	}

	inline boost::asio::io_service &service() { return service_; }

  private:
  	boost::asio::io_service service_;
	//使用work，即使没有异步io的情况下，也能保证io_service继续工作
	boost::asio::io_service::work work_;
	//使用strands最显著的好处就是简化我们的代码，因为通过strand来维护handler不需要显式地同步线程。
	//strands保证同属于一个strand的两个handler不会同时执行(在两个线程同时执行)。
	//如果你只使用一个IO线程（在Boost里面只有一个线程调用io_service::run），那么你不需要做任何的同步，此时已经是隐式的strand。
	//但是如果你想提高性能，因此使用多个IO线程，那么你有两种选择，一种是在不同的handler进行显式的同步，另一种就是使用strand。
	//使用strand将任务排序,即使在多线程情况下，我们也希望任务能按照post的次序执行
	//std::shared_ptr<boost::asio::io_service::strand> strand_;
	std::shared_ptr<std::thread> thread_;
};

}

#endif //__H_XUTIL_XSERVICE_HPP__
