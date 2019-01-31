#ifndef _H_XIPC_HPP_
#define _H_XIPC_HPP_

#pragma once

#include <XUtil.hpp>
#include <XThread.hpp>
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
template <typename Obj, typename Mutex = std::mutex>
class Object_Pool {
	typedef std::list<Obj *> Obj_List;
	typedef boost::unordered_set<Obj *> Obj_Set;

	const static int used_obj_list_bucket = 2000;

public:
	Object_Pool(void);

	Object_Pool(int shurink_level,
			int shrink_delay_level,
			int dump_log_interval,
			size_t used_list_bucket);

	virtual ~Object_Pool(void);

	Obj *pop(void);

	int push(Obj *obj);

	void clear(void);

	void push_shrink(void);

	void push_shrink_i(void);

	void shrink_rate(double rate = 0.8);

	void shrink_rate_i(double rate = 0.8);

	void shrink_all(void);

	void shrink_all_i(void);

	void dump_info(void);

	void dump_info_i(void);

	void debug_info(void);

	size_t sum_size(void);

	size_t free_obj_list_size(void);

	size_t used_obj_list_size(void);

	void xxx_info(void);

private:
	void shrink_info_dump(void);


private:
	Obj_List free_obj_list_;
	size_t free_obj_list_size_;
	Obj_Set used_obj_list_; /// 使用hash_set防止push时std::list::remove操作的低效
	Mutex mutex_;

	int shrink_level_;			/// 收缩水位(默认10万个)
	int shrink_delay_level_;		/// 收缩延迟次数水平(默认1万次)
	int shrink_delay_;			/// 收缩延迟次数

	int dump_log_interval_;		/// 日志记录间隔
	int dump_log_;				/// 操作次数
};

template <typename Obj, typename Mutex>
Object_Pool<Obj, Mutex>::Object_Pool(void)
: free_obj_list_size_(0),
  used_obj_list_(used_obj_list_bucket),
  shrink_level_(100000),
  shrink_delay_level_(10000),
  shrink_delay_(0),
  dump_log_interval_(500),
  dump_log_(0)
{ }

template <typename Obj, typename Mutex>
Object_Pool<Obj, Mutex>::Object_Pool(int shrink_level,
		int shrink_delay_level,
		int dump_log_interval,
		size_t used_list_bucket)
: free_obj_list_size_(0),
  used_obj_list_(used_list_bucket),
  shrink_level_(shrink_level),
  shrink_delay_(0),
  shrink_delay_level_(shrink_delay_level),
  dump_log_interval_(dump_log_interval),
  dump_log_(0)
{ }

template <typename Obj, typename Mutex>
Object_Pool<Obj, Mutex>::~Object_Pool(void) {
	this->clear();
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::push_shrink(void) {
	std::lock_guard<Mutex> lock(mutex_);
	push_shrink_i();
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::push_shrink_i(void) {
	if (free_obj_list_size_ > (size_t)shrink_level_) {
		if (shrink_delay_ > shrink_delay_level_) {
			shrink_rate_i();
		} else {
			++shrink_delay_;
		}
	} else {
		shrink_delay_ = 0;
	}
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::shrink_rate(double rate) {
	std::lock_guard<Mutex> lock(mutex_);
	shrink_rate_i(rate);
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::shrink_rate_i(double rate) {
	if (free_obj_list_size_ < (size_t)shrink_level_)
		return ;

	int shrink_num = free_obj_list_size_ * rate;

	Obj *obj = 0;
	for (int i = 0; i < shrink_num; ++i) {
		obj = free_obj_list_.front();
		free_obj_list_.pop_front();
		--free_obj_list_size_;
		delete obj;
	}
	if (shrink_num > 0)
		shrink_delay_ = 0;
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::shrink_all(void) {
	std::lock_guard<Mutex> lock(mutex_);
	shrink_all_i();
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::shrink_all_i(void) {
	for (typename Obj_List::iterator it = free_obj_list_.begin(); it != free_obj_list_.end(); ++it) {
		delete *it;
	}
	free_obj_list_.clear();
	free_obj_list_size_ = 0;
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::dump_info(void) {
	std::lock_guard<Mutex> lock(mutex_);
	dump_info_i();
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::dump_info_i(void) {
	if (++dump_log_ > dump_log_interval_) {
		LOG4D("free_obj_list_siz = %u, used_obj_list_size = %u", free_obj_list_size_, used_obj_list_.size());
		dump_log_ = 0;
	}
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::debug_info(void) {
	LOG4D("free_obj_list_size_ = %u, free_obj_list_size_ = %u, used_size = %u, shrink_level_ = %u, shrink_delay_level_ = %u, shrink_delay_ = %u",
			free_obj_list_size_, free_obj_list_.size(), used_obj_list_.size(), shrink_level_, shrink_delay_level_, shrink_delay_);
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::xxx_info(void) {
	if (++shrink_delay_ > 500) {
		LOG4D("free_obj_list_size_ = %u, used_size = %u, SUM = %u", free_obj_list_size_, used_obj_list_.size(), free_obj_list_size_ + used_obj_list_.size());
		shrink_delay_ = 0;
	}
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::shrink_info_dump(void) {
/*	std::cerr << "free_obj_list_size_ = " << free_obj_list_size_
			<< " shrink_level_ = " << shrink_level_
			<< " shrink_delay_level_ = " << shrink_delay_level_
			<< " shrink_delay_ = " << shrink_delay_ << std::endl;*/
}

template <typename Obj, typename Mutex>
Obj * Object_Pool<Obj, Mutex>::pop(void) {
#ifdef NO_CACHE
	Obj *obj = new Obj;
#else
	std::lock_guard<Mutex> lock(mutex_);

	Obj *obj = 0;
	if (this->free_obj_list_size_ != 0) {
		obj = this->free_obj_list_.front();
		this->free_obj_list_.pop_front();
		if (! this->used_obj_list_.insert(obj).second) {
			LOG4T("insert failure !");
		}
		--(this->free_obj_list_size_);
	} else {
		while (1) {
			if ((obj = new Obj) == 0) {
				LOG4T("new return 0");
				continue;
			} else {
				if (! this->used_obj_list_.insert(obj).second) {
					LOG4T("insert failure !");
				}
				break;
			}
		}
	}
#endif
	return obj;
}

template <typename Obj, typename Mutex>
int Object_Pool<Obj, Mutex>::push(Obj *obj) {
#ifdef NO_CACHE
	delete obj;
#else
	std::lock_guard<Mutex> lock(mutex_);

	if (obj == 0)
		return -1;

	typename Obj_Set::iterator used_obj_it = this->used_obj_list_.find(obj);
	if (used_obj_it == this->used_obj_list_.end()) {
		LOG4T("***** Object_Pool<Obj, Mutex>::push(Obj *obj) can't find this Object Block !!!");
		return -1;
	}

	this->used_obj_list_.erase(used_obj_it);
	this->free_obj_list_.push_back(obj);
	++free_obj_list_size_;
#endif
	return 0;
}

template <typename Obj, typename Mutex>
void Object_Pool<Obj, Mutex>::clear(void) {
	std::lock_guard<Mutex> lock(mutex_);

	for (typename Obj_List::iterator it = this->free_obj_list_.begin();
			it != this->free_obj_list_.end(); ++it) {
		delete *it;
	}
	for (typename Obj_Set::iterator it = this->used_obj_list_.begin();
			it != this->used_obj_list_.end(); ++it) {
		delete *it;
	}

	this->free_obj_list_size_ = 0;
	this->free_obj_list_.clear();
	this->used_obj_list_.clear();
}

template <typename Obj, typename Mutex>
size_t Object_Pool<Obj, Mutex>::sum_size(void) {
	std::lock_guard<Mutex> lock(mutex_);

	size_t sum_size = 0;
	for (typename Obj_List::iterator it = free_obj_list_.begin(); it != free_obj_list_.end(); ++it) {
		sum_size += (*it)->capacity();
	}
	for (typename Obj_Set::iterator it = used_obj_list_.begin(); it != used_obj_list_.end(); ++it) {
		sum_size += (*it)->capacity();
	}
	return sum_size;
}

template <typename Obj, typename Mutex>
size_t Object_Pool<Obj, Mutex>::free_obj_list_size(void) {
	return this->free_obj_list_size_;
}

template <typename Obj, typename Mutex>
size_t Object_Pool<Obj, Mutex>::used_obj_list_size(void) {
	return this->used_obj_list_.size();
}
} // namespace XUtil

#endif //_H_XIPC_H_