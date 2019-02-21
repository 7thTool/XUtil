#ifndef _H_XNODE_HPP_
#define _H_XNODE_HPP_

#pragma once

#include "XUtil.hpp"

namespace XUtil {
	
	enum field_id
	{
		idf_none = 0,
		idf_symobl,
		idf_name,
		idf_next = 1000,
	};

	enum field_type
	{
		field_none = 0,
		field_dataset,
		field_datetime,
		field_int64,
		field_double,
		field_string,
	};
	
	struct FieldInfo
	{
		size_t id; //列ID
		field_type type; //列类型
	};

	class XDataSet
	{
	public:
		virtual const char* Symbol() = 0;

		//快照字段
		virtual size_t GetFieldIndexById(size_t id) = 0;
		virtual size_t GetFieldCount() = 0;
		virtual bool GetFieldInfo(size_t index, FieldInfo& info) = 0;
		virtual void* GetFieldValue(size_t index) = 0;
		inline int64_t GetFieldValueAsInt(int index) { return (int64_t)GetFieldValue(index); };
		inline double GetFieldValueAsDouble(int index) { return (double)GetFieldValue(index); };
		inline char* GetFieldValueAsStr(int index) { return (char*)GetFieldValue(index); };

		//数据集
		virtual size_t GetBufferIndexById(size_t id) = 0;
		virtual size_t GetBufferFieldCount() = 0; //Buffer的数目
		virtual bool GetBufferFieldInfo(size_t index, FieldInfo& info) = 0;
		virtual size_t GetBufferSize(int index) = 0; //指定index的Buffer的长度
		virtual void* GetBufferPtr(size_t index) = 0; //获取指定index的Buffer指针
		virtual void* GetBufferValue(size_t index, size_t offset) = 0; //获取指定index的Buffer的数值
		inline int64_t GetBufferValueAsInt(int index, size_t offset) { return (int64_t)GetBufferValue(index, offset); };
		inline double GetBufferValueAsDouble(int index, size_t offset) { return (double)GetBufferValue(index, offset); };
		inline char* GetBufferValueAsStr(int index, size_t offset) { return (char*)GetBufferValue(index, offset); };
	};
}

#endif//_H_XNODE_HPP_
