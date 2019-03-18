#pragma once
#ifndef _H_XIPC_HPP_
#define _H_XIPC_HPP_

#include "XUtil.hpp"

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/upgradable_lock.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/smart_ptr/shared_ptr.hpp>
#include <boost/interprocess/smart_ptr/weak_ptr.hpp>

//如果我们允许对仅读取数据的线程的并发访问，但是我们避免对读 / 写线程的并发访问，我们就能够提升性能。
//这在如下应用中尤其明显：当读数据比修改数据更频繁并且同步数据读取的代码需要一定的时间来执行。采用升级互斥量，我们能够获得3种锁类型：
//独占锁：与普通互斥量类似。如果一个线程获取了一个独占锁，其他任何线程都不能获取此锁（互斥或其他方式）直到解锁。
//如果某一线程有一个共享或升级锁，则另一个企图获取独占锁的线程会阻塞。这种锁将被会修改数据的线程所拥有。
//共享锁：如果一个线程获取了一个共享锁，其他线程也可以获取一个共享锁或升级锁。
//如果任一线程获取了独占锁，则另一企图获取共享锁的线程会被阻塞。这种锁被仅需要读取数据的线程执行。
//升级锁：获取一个升级锁与获取一个特权共享锁类似。如果一个进程获取了一个升级锁，另外的线程可以获取一个共享锁。
//如果任一线程已经获取了独占锁或升级锁，则其他企图获取升级锁的线程会被阻塞。
//当其他已经获取了共享锁的线程释放了此锁，一个已经获取一个升级锁的线程被保证能够原子获取一个独占锁。
//它被用于如下线程中：此线程可能需要修改数据，但通常情况下它仅需要读取数据。这种线程获取升级锁，其他线程能获取共享锁。
//如果升级的线程读数据并且它需要修改数据时，此线程能够被提升至获取独占锁：当所有共享线程已经释放了共享锁时，升级锁被原子提升至独占锁。
//这个新被提升的线程能够修改数据，并且能够保证在过渡时，没有其他线程对数据修改。仅仅一个线程能获取升级（特权读）锁。

//有许多锁转移操作，我们可以根据出现在升级互斥量操作中的操作来分类：
//保证成功和非阻塞：任何从更严格的锁至次严格的锁。局部的->升级的、局部的->共享的、升级的->共享的。
//不保证成功：如果没有其他线程已经获取此升级或独占锁，则从共享的->独占的操作可能成功。此操作是一个尝试操作。
//如果无限等待，保证能成功：从升级的->局部的转换将成功，但需要等待所有共享锁被释放。
//由于这是一个阻塞操作，我们也可以选择不无限等待而仅仅尝试或仅等待超时即可。

namespace XUtil
{
inline int mq_create(const char *name, int max_que_num, int max_msg_size)
{
    try
    {
        boost::interprocess::message_queue::remove(name);
        //Create a message_queue.
        boost::interprocess::message_queue mq(boost::interprocess::create_only //only create
                                              ,
                                              name //name
                                              ,
                                              max_que_num //max message number
                                              ,
                                              max_msg_size //max message size
        );
        //printf("Boost Msg Created Name:%s QueLen:%d MsgMaxLen:%d \n", name, max_que_num, max_msg_size);
        return true;
    }
    catch (boost::interprocess::interprocess_exception &ex)
    {
        //std::cout << ex.what() << std::endl;
    }
    return 0;
}

inline int mq_send(const char *name, const char *msg, int msg_len)
{
    try
    {
        //Erase previous message queue
        //message_queue::remove("message_queue");
        //Create a message_queue.
        boost::interprocess::message_queue mq(boost::interprocess::open_only, name);

        mq.send(msg, msg_len, 0);
        return msg_len;
    }
    catch (boost::interprocess::interprocess_exception &ex)
    {
        //std::cout << ex.what() << std::endl;
    }
    return 0;
}

inline int mq_recv(const char *name, char *msg, int max_msg_size)
{
    try
    {
        //Open a message queue.
        boost::interprocess::message_queue mq(boost::interprocess::open_only, name);

        unsigned int priority;
        boost::interprocess::message_queue::size_type recvd_size;

        memset(msg, 0, max_msg_size);
        mq.receive(msg, max_msg_size, recvd_size, priority);
        return recvd_size;
    }
    catch (boost::interprocess::interprocess_exception &ex)
    {
        //std::cout << ex.what() << std::endl;
    }
    boost::interprocess::message_queue::remove(name);
    return 0;
}

//Remove shared memory on construction and destruction
struct XSharedMemoryRemove
{
    XSharedMemoryRemove(const char *name)
    {
        boost::interprocess::shared_memory_object::remove(name);
    }
    ~XSharedMemoryRemove()
    {
        //boost::interprocess::shared_memory_object::remove(name);
    }
};

template <class managed_t = boost::interprocess::managed_shared_memory>
class XSharedMemory
{
  public:
    //typedef boost::interprocess::interprocess_upgradable_mutex shm_mutex_t;
    //This will call lock_sharable()
    //boost::interprocess::sharable_lock<shm_mutex_t> share_lock(mutex);
    //This will call lock_upgradable()
    //boost::interprocess::upgradable_lock<shm_mutex_t> upgrade_lock(mutex);
    //This calls unlock_upgradable_and_lock()
    //boost::interprocess::scoped_lock<shm_mutex_t> scoped_lock(std::move(upgrade_lock));
    //This calls try_unlock_sharable_and_lock()
    //boost::interprocess::scoped_lock<Mutex>      scoped_lock(move(share_lock, boost::interprocess::try_to_lock));
    //This calls unlock_and_lock_upgradable()
    //boost::interprocess::upgradable_lock<Mutex>  upgrade_lock(move(scoped_lock));
    //This calls unlock_and_lock_sharable()
    //boost::interprocess::sharable_lock<Mutex>    share_lock(move(scoped_lock));
// #if mapped_file
//     typedef boost::interprocess::managed_mapped_file managed_t;
//     typedef boost::interprocess::managed_mapped_file::segment_manager segment_manager_t;
// #else
//     typedef boost::interprocess::managed_shared_memory managed_t;
//     typedef boost::interprocess::managed_shared_memory::segment_manager segment_manager_t;
// #endif
    typedef typename managed_t::segment_manager segment_manager_t;
    typedef boost::interprocess::allocator<void, segment_manager_t> void_allocator;
    typedef boost::interprocess::allocator<char, segment_manager_t> char_allocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator> char_string;
    typedef boost::interprocess::allocator<char_string, segment_manager_t> char_string_allocator;

  public:
    XSharedMemory()
    {
    }
    XSharedMemory(const char *segment_name, size_t segment_size)
    {
        if (!open(segment_name, segment_size))
        {
            close();
        }
    }
    ~XSharedMemory()
    {
        close();
    }

    bool open(const char *segment_name, size_t segment_size)
    {
        segment_name_ = segment_name;
        return open(segment_size);
    }

    inline bool is_open()
    {
        return segment_ptr_ != nullptr;
    }

    void close()
    {
        if (segment_ptr_)
        {
            segment_ptr_.reset();
        }
    }

    size_t total_size()
    {
        return segment_ptr_->get_size();
    }

    size_t used_size()
    {
        return segment_ptr_->get_size() - segment_ptr_->get_free_memory();
    }

    size_t free_size()
    {
        return segment_ptr_->get_free_memory();
    }

    bool grow(size_t grow_size)
    {
        return managed_t::grow(segment_name_.c_str(), grow_size);
    }

    bool shrink()
    {
        return managed_t::shrink_to_fit(segment_name_.c_str());
    }

  protected:
    bool open(size_t segment_size, bool open_only = false)
    {
        try
        {
            //if(open_only) {
            //	segment_ptr_ = std::make_shared<managed_t>(boost::interprocess::open_only, segment_name_.c_str(), segment_size);
            //} else {
            //Open or  create a segment with given name and size
            segment_ptr_ = std::make_shared<managed_t>(boost::interprocess::open_or_create, segment_name_.c_str(), segment_size);
            //}
        }
        catch (const std::exception &)
        {
            //
        }
        return is_open();
    }

    std::string segment_name_;
    std::shared_ptr<managed_t> segment_ptr_;
};
} // namespace XUtil

#endif //_H_XIPC_H_