#ifndef _H_XTHREAD_HPP_
#define _H_XTHREAD_HPP_

#pragma once

#include "XUtil.hpp"

namespace XUtil {
  /// null_mutex
  class null_mutex
  {
  public:
    null_mutex() noexcept = default;
    ~null_mutex() = default;

    null_mutex(const null_mutex&) = delete;
    null_mutex& operator=(const null_mutex&) = delete;

    void lock() {}
    bool try_lock() noexcept { return true; }
    void unlock() {}
  };

//typedef boost::mutex						x_mutex_t;
//typedef boost::recursive_mutex			x_recursive_mutex_t;
//typedef boost::shared_mutex				x_rw_mutex_t;
//typedef boost::condition_variable_any		x_condition_t;

//typedef boost::mutex::scoped_lock			x_auto_lock;
//typedef boost::recursive_mutex::scoped_lock	x_recursive_auto_lock;
//typedef boost::shared_lock<my_rw_mutex_t>	x_auto_rlock;
//typedef boost::unique_lock<my_rw_mutex_t>	x_auto_wlock;

	struct SingleThreadTag {};
	struct MultiThreadTag {};

	template <typename LockType>
	class ThreadingModel
	{
	public:
		typedef MultiThreadTag ThreadTag;

		class Lock
		{
			Lock(const Lock&);
			void operator=(const Lock&);

		public:
			explicit Lock(const ThreadingModel& host)
				: host_(host)
			{
				host_.mutex_.lock();
			}
			~Lock()
			{
				host_.mutex_.unlock();
			}

		private:
			const ThreadingModel& host_;
		};

		friend class Lock;

		ThreadingModel() {}

	private:
		ThreadingModel(const ThreadingModel&);
		ThreadingModel& operator=(const ThreadingModel&);

	private:
		mutable LockType    mutex_;
	};

	template <>
	class ThreadingModel<void>
	{
	public:
		typedef SingleThreadTag ThreadTag;

		struct Lock
		{
			explicit Lock(const ThreadingModel&) {}
		};

		ThreadingModel() {}

	private:
		ThreadingModel(const ThreadingModel&);
		ThreadingModel& operator=(const ThreadingModel&);
	};

#ifdef WIN32
#define gettid GetCurrentThreadId
#else
#ifndef gettid
#define gettid() syscall(__NR_gettid)
#endif//
inline size_t GetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#endif//
}

#endif//_H_XTHREAD_HPP_

