#ifndef _H_XCOUNTER_HPP_
#define _H_XCOUNTER_HPP_

#pragma once

#include "XUtil.hpp"
#include <atomic>

namespace XUtil {

	inline size_t get_unique_id()
	{
		static std::atomic_size_t counter(0);
		return ++counter;
	}
}

#endif//_H_XCOUNTER_HPP_