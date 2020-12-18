#pragma once

#include "XApp.hpp"
#include "XService.hpp"
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
class XModuleServer : public XApp, public XService
{
	typedef XApp Base;
	typedef XService Service;
  public:
	XModuleServer() : Base(), Service()
	{
	}

	~XModuleServer()
	{
	}

	bool start(const char* xml, int xmlflag)
	{
		Base::init(xml, xmlflag);
		return Service::start();
	}

	void stop()
	{
		Service::stop();
		Base::term();
	}
};

template<class T>
class XServer : public XApp
{
	typedef XApp Base;
#ifdef __APPLE__
	class Input : public boost::enable_shared_from_this<Input>
	{
	public:
		typedef boost::shared_ptr<Input> Ptr;

	public:
		static void create(T* pT, boost::asio::io_service& io_service)
		{
			Ptr input(
					new Input( pT, io_service )
					);
			input->read();
		}

	private:
		explicit Input(T* pT, boost::asio::io_service& io_service ) : pT_(pT), _input( io_service )
		{
			_input.assign( STDIN_FILENO );
		}

		void read()
		{
			async_read(
					_input,
					boost::asio::buffer( &_command, sizeof(_command) ),
					boost::bind(
						&Input::read_handler,
						this->shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
						)
					);
		}

		void read_handler(
				const boost::system::error_code& error,
				size_t bytes_transferred
				)
		{
			if ( error ) {
				std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
				return;
			}

			if(_command == 'q') {
				pT_->stop();
				return;
			}
			//if ( _command != '\n' ) {
				std::cout << "command: " << _command << std::endl;
			//}

			this->read();
		}

	private:
		T* pT_;
		boost::asio::posix::stream_descriptor _input;
		char _command;
	};
#else
#define INPUT_BUFFER_LENGTH 512
class Input : public boost::enable_shared_from_this<Input> 
{

	public:
		typedef boost::shared_ptr<Input> Ptr;

	public:
		static void create(T* pT, boost::asio::io_service& io_service)
		{
			Ptr input(
					new Input( pT, io_service )
					);
			input->read();
		}

    public:
        Input(T* pT, boost::asio::io_service& io_service)
            : pT_(pT), input_buffer( INPUT_BUFFER_LENGTH), input_handle( io_service )
        {
            
        }

		void read()
		{
			// Read a line of input.
            /*boost::asio::async_read_until( input_handle, input_buffer, "\r\n",
                boost::bind( &Input::handle_read
				, this->shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));*/
		}
        
		void handle_read( const boost::system::error_code& error, std::size_t length)
		{
			if (error) {
				std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
				return;
			}

			// Remove newline from input.
			input_buffer.consume(1);
			input_buffer.commit( length - 1);

			std::istream is(&input_buffer);
			std::string s;
			is >> s;

			std::cout << s << std::endl;
			if(s == "q") {
				pT_->stop();
				return;
			}

			this->read();
		}

		void handle_write( const boost::system::error_code& error)
		{
			if (!error)
			{
				this->read();
			}
		}
    private:
		T* pT_;
        boost::asio::streambuf input_buffer;
        boost::asio::windows::stream_handle input_handle;
};
#endif
public:
	XServer():io_service_(),signals_(io_service_)
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
		boost::system::error_code ec;
		signals_.cancel(ec);

		io_service_.stop();
		io_service_.reset();
	}
	
	bool run()
	{
// 有两个信号可以停止进程:SIGTERM和SIGKILL。 SIGTERM比较友好，进程能捕捉这个信号，根据您的需要来关闭程序。在关闭程序之前，您可以结束打开的记录文件和完成正在做的任务。在某些情况下，假如进程正在进行作业而且不能中断，那么进程可以忽略这个SIGTERM信号。
// 对于SIGKILL信号，进程是不能忽略的。这是一个 “我不管您在做什么,立刻停止”的信号。假如您发送SIGKILL信号给进程，Linux就将进程停止在那里。
		// The signal_set is used to register for process termination notifications.
    	//boost::asio::signal_set signals(io_service_);
		signals_.add(SIGINT);
   		signals_.add(SIGTERM);
#if defined(SIGQUIT)
    	signals_.add(SIGQUIT);
#endif	
   		//signals.add(SIGKILL); /* kill (cannot be caught or ignored) */
		T* pT = static_cast<T*>(this);
    	signals_.async_wait(boost::bind(&T::on_signals, pT, boost::asio::placeholders::error, boost::asio::placeholders::signal_number));
#ifdef __APPLE__
		Input::create(pT, io_service_);
#endif
		io_service_.run();

		return true;
	}

	void on_signals(const boost::system::error_code& ec, int signal_number)
	{
		if(ec) {
			std::cerr << "signals error: " << boost::system::system_error(ec).what() << std::endl;
			return;
		}
		T* pT = static_cast<T*>(this);
		pT->stop();
	}

private:
	boost::asio::io_service io_service_;
	boost::asio::signal_set signals_;
};

}
