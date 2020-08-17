#ifndef _H_XXML_HPP_
#define _H_XXML_HPP_

#pragma once

#include "XUtil.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/typeof/typeof.hpp>  
#include <boost/foreach.hpp>  

namespace XUtil {
	
	inline bool json_from_file(const boost::filesystem::path& path, boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ifstream fs(path.native(), std::ifstream::in);//only read
		if (fs.is_open()) {
			std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
			std::istringstream ss(str);
			//std::istringstream ss;
			//ss >> fs.rdbuf();
			boost::property_tree::read_json(ss, obj);
			fs.close();
			return true;
		}
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool json_from_str(const std::string & str, boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::istringstream ss(str);
		boost::property_tree::json_parser::read_json(ss, obj);
		return true;
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool json_to_file(boost::filesystem::path& path, const boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ofstream fs(path.native(), std::ifstream::out);//only read
		if (fs.is_open()) {
			std::ostringstream ss;
			boost::property_tree::write_json(ss, obj);
			fs << ss.str();
			fs.close();
			return true;
		}
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool json_to_str(std::string & str, const boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ostringstream ss;
		boost::property_tree::json_parser::write_json(ss, obj);
		str = ss.str();
		return true;
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool xml_from_file(const boost::filesystem::path& path, boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ifstream fs(path.native(), std::ifstream::in);//only read
		if (fs.is_open()) {
			std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
			std::istringstream ss(str);
			//std::istringstream ss;
			//ss >> fs.rdbuf();
			boost::property_tree::read_xml(ss, obj);
			fs.close();
			return true;
		}
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool xml_from_str(const std::string & str, boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::istringstream ss(str);
		boost::property_tree::xml_parser::read_xml(ss, obj);
		return true;
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool xml_to_file(boost::filesystem::path& path, const boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ofstream fs(path.native(), std::ifstream::out);//only read
		if (fs.is_open()) {
			std::ostringstream ss;
			boost::property_tree::write_xml(ss, obj);
			fs << ss.str();
			fs.close();
			return true;
		}
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	inline bool xml_to_str(std::string & str, const boost::property_tree::ptree & obj) _NOEXCEPT {
		_TRY_BEGIN
		std::ostringstream ss;
		boost::property_tree::xml_parser::write_xml(ss, obj);
		str = ss.str();
		return true;
		_CATCH_ALL
		_CATCH_END
		return false;
	}

	//从当前路径开始的全路径，父路径和子路径之间通过“.”连接，如“root.sub.child”
	//find获取的是第一个子节点，如果我们要获取子节点列表，则要用路径的上一级，如“root.sub”
	inline bool cfg_from_child(const std::string & child, const boost::property_tree::ptree & in, boost::property_tree::ptree & out) {
		auto opt_child = in.get_child_optional(child);
		if (opt_child) {
			out = opt_child.get();
			return true;
		}
		return false;
	}

	enum
	{
		XML_FLAG_JSON_STRING = 0, //默认字符串
		XML_FLAG_JSON_FILE,
		XML_FLAG_XML_STRING,
		XML_FLAG_XML_FILE,
		XML_FLAG_PTREE,
	};

	inline bool cfg_from_xml(const char* xml, size_t xmlflag, boost::property_tree::ptree & obj) {
		if (xml) {
			bool xmlok = false;
			boost::property_tree::ptree* xmlobj;
			if (xmlflag == XML_FLAG_JSON_FILE) {
				xmlok = json_from_file(xml, obj);
			}
			else if (xmlflag == XML_FLAG_JSON_STRING) {
				xmlok = json_from_str(xml, obj);
			}
			else if (xmlflag == XML_FLAG_XML_FILE) {
				xmlok = xml_from_file(xml, obj);
			}
			else if (xmlflag == XML_FLAG_XML_STRING) {
				xmlok = xml_from_str(xml, obj);
			}
			else if (xmlflag == XML_FLAG_PTREE) {
				obj = *(boost::property_tree::ptree*)xml;
				xmlok = true;
			}
			return xmlok;
		}
		return false;
	}
}

#define CFG_FROM_XML(cfg,xml,xmlflag) \
	boost::property_tree::ptree* _##cfg##_ptr; \
	boost::property_tree::ptree _##cfg##_ptree_; \
	if (xmlflag == XUtil::XML_FLAG_PTREE) { _##cfg##_ptr = (boost::property_tree::ptree*)xml; } \
	else { XUtil::cfg_from_xml(xml,xmlflag,_##cfg##_ptree_); _##cfg##_ptr = &_##cfg##_ptree_; } \
	boost::property_tree::ptree& cfg = *_##cfg##_ptr;

#endif//_H_XXML_HPP_