#ifndef _H_XIPC_HPP_
#define _H_XIPC_HPP_

#pragma once

#include <XUtil.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace XUtil
{
    inline int mq_create(const char *name, int max_que_num, int max_msg_size)
    {
        try
        {
            boost::interprocess::message_queue::remove(name);
            //Create a message_queue.
            boost::interprocess::message_queue mq(boost::interprocess::create_only //only create
                                                , name //name
                                                , max_que_num //max message number
                                                , max_msg_size //max message size
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
            boost::interprocess::message_queue mq(boost::interprocess::open_only
                                                , name
            );

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
            boost::interprocess::message_queue mq(boost::interprocess::open_only
                                                , name
            );

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
} // namespace XUtil

#endif //_H_XIPC_H_