#ifndef _H_XTABLE_HPP_
#define _H_XTABLE_HPP_

#pragma once

#include <XUtil.hpp>
#include <XXml.hpp>
#include <boost/unordered_set.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace XUtil
{
struct Field_Info {
	std::string field_label;
	std::string field_type;
	std::string field_name;
	std::string key_type;
	std::string key_name;
};

inline bool is_struct(const std::string &field_type) {
	if(field_type == "int8" || field_type == "int16" || field_type == "int32" || field_type == "int64" ||
			field_type == "double" || field_type == "bool" || field_type == "string") return false;
	return true;
}

class Table_Info {
public:
	Table_Info(onst boost::property_tree::ptree& xml)
	: table_name_()
	, index_name_()
	{
		field_vec_.clear();
		BOOST_FOREACH (const boost::property_tree::ptree::value_type &it, xml)
		{
			if(it.first == "head") {
				table_name_ = it.second.get<std::string>("table_name", "");
				index_name_ = it.second.get<std::string>("index_name", "");
			} else {
				Field_Info field_info;
				field_info.field_label = it.first;
				field_info.field_type = it.second.get<std::string>("field_type", "");
				field_info.field_name = it.second.get<std::string>("field_name", "");
				field_info.key_type = it.second.get<std::string>("key_type", "");
				field_info.key_name = it.second.get<std::string>("key_name", "");
				field_vec_.push_back(field_info);
			}
		}
	}
	virtual ~Table_Info() 
	{
		table_name_.clear();
		index_name_.clear();
		field_vec_.clear();
	}

	virtual void create_data(int64_t key_index, Block_Buffer &buffer) {}
	virtual void load_data(int64_t key_index, Block_Buffer &buffer) {}
	virtual void save_data(Block_Buffer &buffer) {}
	virtual void save_data_vector(Block_Buffer &buffer) {}
	virtual void delete_data(Block_Buffer &buffer) {}

	inline const std::string &table_name() { return table_name_; }
	inline void set_table_name(const std::string &table_name) { table_name_ = table_name; }
	inline const std::string &index_name() { return index_name_; }
	inline const std::vector<Field_Info>& field_vec() { return field_vec_; }

private:
	std::string table_name_;
	std::string index_name_;
	std::vector<Field_Info> field_vec_;
};
} // namespace XUtil

#endif //_H_XTABLE_HPP_