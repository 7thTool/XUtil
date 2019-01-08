#ifndef _H_XQUEUE_HPP_
#define _H_XQUEUE_HPP_

#pragma once

#include <XUtil.hpp>

namespace XUtil {
	template <class T>
	class BlockingQueue
	{
	public:
		BlockingQueue(size_t timeout = 0) : timeout_(timeout) { }
		BlockingQueue(const T&) = delete;
		BlockingQueue& operator=(const T&) = delete;
		~BlockingQueue()
		{
			std::lock_guard<std::mutex> lock(mutex_);
			while (!queue_.empty())
			{
				ASSERT(0);
				queue_.pop();
			}
		}
		void push(T& o)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(o);
			if (timeout_) {
				cv_.notify_one();
			}
		}
		bool pop(T& o)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (timeout_) {
				if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_),
					[this]() { return !queue_.empty(); })) {
					return false;
				}
			}
			else {
				if (queue_.empty()) {
					return false;
				}
			}
			o = queue_.front();
			queue_.pop();
			return true;
		}
		bool empty()
		{
			return queue_.empty();
		}
	private:
		std::queue<T> queue_;
		std::mutex mutex_;
		std::condition_variable cv_;
		size_t timeout_;
	};
}

#endif//_H_XQUEUE_HPP_