#ifndef _H_XUTIL_HPP_
#define _H_XUTIL_HPP_

#pragma once

#include "XUtil.h"
#ifdef WIN32
#define NOMINMAX
#else
#endif//

#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <bitset>
#include <string>
#include <memory>
#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <algorithm>
#include <functional>

#include <bind>
#include <function>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time.hpp>
#include <boost/any.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/process.hpp>
#include <boost/dll.hpp>
#include <boost/program_options.hpp>

#ifndef _NOEXCEPT
#define _NOEXCEPT	noexcept
#define _NOEXCEPT_OP(x)	noexcept(x)
#else
//#define _NOEXCEPT	throw ()
//#define _NOEXCEPT_OP(x)
#endif

#ifndef _TRY_BEGIN
#define _TRY_BEGIN	try {
#define _CATCH(x)	} catch (x) {
#define _CATCH_ALL	} catch (...) {
#define _CATCH_END	}
#define _RAISE(x)	throw x
#define _RERAISE	throw
#define _THROW0()	noexcept
#else 
//#define _TRY_BEGIN	{ if (1) {
//#define _CATCH(x)	} else if (0) {
//#define _CATCH_ALL	} else if (0) {
//#define _CATCH_END	} }
//#define _RERAISE
//#define _THROW0()
#endif

#ifndef ASSERT
#define ASSERT BOOST_ASSERT
#endif//ASSERT

namespace XUtil
{
	//boost::filesystem::path常用函数
	//append(InputIterator first, InputIterator last);	将字符串 s 或字符序列[first, last) 中的路径元素追加到保存的路径中。
	//remove_filename();	去除路径中的文件名
	//replace_extension(const string_type & new_extension = "");	替换扩展名
	//string_type string()	得到可移植路径名
	//string_type file_string()	得到系统原生文件名
	//string_type directory_string()	得到系统原生路径名
	//string_type root_name() const;	得到根名
	//string_type root_directory() const;	得到根目录
	//basic_path root_path() const;	得到根路径：根名 + 根目录
	//basic_path relative_path() const;	得到相对路径
	//string_type filename() const;	得到文件名
	//basic_path parent_path() const;	得到父路径：根路径 + 相对路径
	//string_type stem(const Path & p) const;	得到不带扩展名的文件名
	//string_type extension(const Path & p) const;	得到扩展名
	//bool empty() const;	path未赋值
	//bool is_complete() const;	是否是完整路径
	//bool has_root_path() const;
	//bool has_root_name() const;
	//bool has_root_directory() const;
	//bool has_relative_path() const;
	//bool has_filename() const;
	//bool has_branch_path() const;	路经中是否包含指定的项
	//boost::filesystem常用函数
	//system_complete(path);	返回完整路径(相对路径 + 当前路径)
	//exists(path);	文件是否存在
	//is_directory(path);
	//is_directory(file_status);	是否是路径
	//is_regular_file(path);
	//is_regular_file(file_status);	是否是普通文件
	//is_symlink(path);
	//is_symlink(file_status);	是否是一个链接文件
	//file_status status(path);	返回路径名对应的状态
	//template <class Path> const Path& initial_path();	得到程序运行时的系统当前路径
	//template <class Path> Path current_path();	得到系统当前路径
	//template <class Path> void current_path(const Path& p);	改变当前路径
	//template <class Path> space_info space(const Path& p);	得到指定路径下的空间信息，space_info 有capacity, free 和 available三个成员变量，分别表示容量，剩余空间和可用空间。
	//template <class Path> std::time_t last_write_time(const Path& p);	最后修改时间
	//template <class Path> void last_write_time(const Path& p, const std::time_t new_time);	修改最后修改时间
	//template <class Path> bool create_directory(const Path& dp);	建立路径
	//template <class Path1, class Path2> void create_hard_link(const Path1& to_p, const Path2& from_p);
	//template <class Path1, class Path2> error_code create_hard_link(const Path1& to_p,
	//	const Path2& from_p, error_code& ec);	建立硬链接
	//template <class Path1, class Path2> void create_symlink(const Path1& to_p, const Path2& from_p);
	//template <class Path1, class Path2> error_code create_symlink(const Path1& to_p, const Path2& from_p, error_code& ec); 	建立软链接
	//template <class Path> void remove(const Path& p, system::error_code & ec = singular);	删除文件
	//template <class Path> unsigned long remove_all(const Path& p);	递归删除p中所有内容，返回删除文件的数量
	//template <class Path1, class Path2> void rename(const Path1& from_p, const Path2& to_p);	重命名
	//template <class Path1, class Path2> void copy_file(const Path1& from_fp, const Path2& to_fp);	拷贝文件
	//template <class Path> Path complete(const Path& p, const Path& base = initial_path<Path>());	以base以基，p作为相对路径，返回其完整路径
	//template <class Path> bool create_directories(const Path & p);	建立路径
	inline boost::filesystem::path get_initial_path() {
		return boost::filesystem::initial_path<boost::filesystem::path>();
	}

	inline boost::filesystem::path get_current_path() {
		return boost::filesystem::current_path();
	}

	inline boost::filesystem::path get_program_path() {
		return boost::dll::program_location();
	}

	inline boost::filesystem::path get_module_path() {
		return boost::dll::this_line_location();
	}

	template <class T>
	inline boost::filesystem::path get_symbol_path(const T &symbol) {
		return boost::dll::symbol_location(symbol);
	}

	//得到当前进程的环境变量
	//boost::process::self::get_environment()
	//我们可以对环境变量进行修改，如
	//boost::process::environment_context env;
	//env.insert(boost::process::environment::valuetype(“A”, “a”));

	//启动进程
	//std::error_code err;
	//std::string arduinoDir = R"(avrtool)";
	//std::string arduinoPath = R"(avrtool\avrdude.exe)";
	//boost::filesystem::path absArduinoDir = boost::filesystem::absolute(arduinoDir);
	//BOOST_VERIFY(boost::filesystem::exists(arduinoDir) && boost::filesystem::exists(arduinoPath));
	//int exit_code = boost::process::system(
	//	R"(avrtool\avrdude.exe)",
	//	boost::process::start_dir = absArduinoDir,
	//	boost::process::args = { "-C", "avrdude.conf",
	//	//"-v",
	//	"-p", "atmega328p",
	//	"-c", "arduino",
	//	"-P", com,
	//	"-b", "115200",
	//	"-D",
	//	"-Uflash:w:" + file + ":i"
	//},
	//	boost::process::on_exit([&](int exit_code, const std::error_code& ec) {
	//	err = ec;
	//})
	//);
	//if (err) {
	//	SetLastErrorMessage(err.message());
	//}
	//return exit_code == 0;

	//arc argv参数解析
	//std::string one; // 外部变量 存储 参数one的值
	//std::vector<string> mult;
	//boost::program_options::options_description opts("test options");
	//opts.add_options()
	//	("help,h", "help info")
	//	("test1,t", boost::program_options::value<std::string>(), "test aaa ")
	//	("one,o", boost::program_options::value<std::string>(&one)->default_value("one"), "test one default") // 默认值
	//	("mult,m", boost::program_options::value<std::vector<std::string> >(&mult)->multitoken(), "mult test"); //多个参数

	//boost::program_options::variables_map vm;
	//try
	//{
	//	boost::program_options::store(parse_command_line(argc, argv, opts), vm); // 分析参数
	//}
	//catch (boost::program_options::error_with_no_option_name &ex)
	//{
	//	std::cout << ex.what() << std::endl;
	//}

	//boost::program_options::notify(vm); // 将解析的结果存储到外部变量
	//if (vm.count("help"))
	//{
	//	std::cout << opts << std::endl;
	//	return -1;
	//}
	//if (vm.count("test1"))
	//{
	//	std::cout << vm["test1"].as<std::string>() << std::endl;
	//}

	//std::cout << one << std::endl;
	//std::cout << mult.size() << std::endl;

	template<class Ty>
	char Sign(Ty x, Ty y)
	{
		if (x > y) {
			return 1;
		}
		else if (x < y) {
			return -1;
		}
		return 0;
	}
	template<class Ty>
	char SignEx(Ty x, Ty y)
	{
		if (x >((Ty)(y + _zero_ctrl_))) {
			return 1;
		}
		else if (x < ((Ty)-(y + _zero_ctrl_))) {
			return -1;
		}
		return 0;
	}
	template<class Ty>
	char SignEx(Ty x)
	{
		if (x >((Ty)_zero_ctrl_)) {
			return 1;
		}
		else if (x < ((Ty)-_zero_ctrl_)) {
			return -1;
		}
		return 0;
	}
} // namespace XUtil

#endif //_H_XUTIL_HPP_