#ifndef __H_XUTIL_XREDIS_HPP__
#define __H_XUTIL_XREDIS_HPP__

#pragma once

#include "XUtil.hpp"
#include <hiredis.h>

namespace XUtil
{

#define DEFAULT_expire_TIME 22 * 3600

enum ErrorCMD
{
	REDIS_NO_OBJ,
	REDIS_INTERRUPT,
	REDIS_PROCESS_ERROR,
	REDIS_get_NULL,
	REDIS_db_ERROR,
	REDIS_CONN_ERROR
};

class XRedis
{
  protected:
	class redisReplyGuard
	{
	  public:
		redisReplyGuard(redisReply *reply) : m_reply(reply) {}
		~redisReplyGuard()
		{
			if (m_reply)
				freeReplyObject(m_reply);
		}

	  private:
		redisReply *m_reply;
	};

  public:
	XRedis() : context_(NULL)
	{
	}
	~XRedis()
	{
		if (context_)
			redisFree(context_);
	}

	bool Connect(const char *ip, int port, int timeout, std::string *errstr)
	{
		if (context_ != NULL)
			return true;

		memset(ip_, 0, sizeof(ip_));
		strncpy(ip_, ip, strlen(ip));
		port_ = port;
		timeout_ = timeout;

		struct timeval t = {timeout, 0};
		context_ = recloseWithTimeout(ip, port, t);
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}
		if (context_->err)
		{
			if (errstr)
				get_error_msg(REDIS_CONN_ERROR, errstr, context_->errstr);
			return false;
		}

		return true;
	}

	bool reconnect(std::string *errstr)
	{
		close();

		struct timeval t = {timeout_, 0};
		context_ = recloseWithTimeout(ip_, port_, t);
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}
		if (context_->err)
		{
			if (errstr)
				get_error_msg(REDIS_CONN_ERROR, errstr, context_->errstr);
			return false;
		}
		select(db_.c_str());

		return true;
	}

	bool select(const std::string &db, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}
		if (db.length() <= 0)
		{
			if (errstr)
				get_error_msg(REDIS_db_ERROR, errstr);
			return false;
		}

		if (db_.compare(db) == 0)
		{
			return true;
		}

		m_reply = (redisReply *)redisCommand(context_, "SELECT %s", db.c_str());

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		db_ = db;

		return true;
	}

	bool ping(std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "ping");

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_STATUS && strcasecmp(m_reply->str, "PONG") == 0)
			return true;

		return false;
	}

	void flush()
	{
		if (context_ == NULL)
		{
			return;
		}

		m_reply = (redisReply *)redisCommand(context_, "FLUSHDB");
		redisReplyGuard rrg(m_reply);
	}

	void close()
	{
		if (context_ == NULL)
		{
			return;
		}

		redisFree(context_);
		context_ = NULL;
	}

	void expire(const char *key, uint32_t second, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return;
		}

		m_reply = (redisReply *)redisCommand(context_, "expire %s %u", key, second);
		redisReplyGuard rrg(m_reply);
	}

	bool set(const char *key, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "set %s %b", key, value, len);
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);
		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
	}

	bool set(const char *key, int expire, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "set %s %b EX %d", key, value, len, expire);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
		{
			//m_reply = (redisReply *)redisCommand(context_,"expire %s %d",key,expire);
			return true;
		}
		else
		{
			if (errstr)
				get_error_msg(REDIS_PROCESS_ERROR, errstr, m_reply->str);
			return false;
		}
	}

	bool get(const char *key, std::string &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "get %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_std::string)
		{
			result.append(m_reply->str, m_reply->len);
		}
		return true;
	}

	bool exists(const char *key, int &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "exists %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_std::string)
		{
			std::string str(m_reply->str, m_reply->len);
			result = atoi(str.c_str());
		}
		else
		{
			result = m_reply->integer;
		}
		return true;
	}

	bool mget(const char *key, std::list<std::string> &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);
		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t j = 0; j < m_reply->elements; j++)
			{
				redisReply *childReply = m_reply->element[j];
				//if(childReply->type == REDIS_REPLY_std::string)
				{
					result.push_back(std::string(childReply->str, childReply->len));
				}
			}
		}
		return true;
	}

	bool mmget(const char *key, std::list<std::list<std::string>> &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);
		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t i = 0; i < m_reply->elements; i++)
			{
				redisReply *childReply = m_reply->element[i];
				if (m_reply->type == REDIS_REPLY_ARRAY)
				{
					std::list<std::string> std::lists;
					for (size_t j = 0; j < childReply->elements; j++)
					{
						redisReply *tempReply = childReply->element[j];
						//if(tempReply->type == REDIS_REPLY_std::string)
						{
							result.push_back(std::string(tempReply->str, tempReply->len));
						}
					}
					std::listss.push_back(std::lists);
				}
			}
		}
		return true;
	}

	bool push(const char *key, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		if (!key || !value)
			return false;

		m_reply = (redisReply *)redisCommand(context_, "Rpush %s %b", key, value, len);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_PROCESS_ERROR, errstr, m_reply->str);
			return false;
		}
	}

	bool push(const char *key, std::list<std::string> &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		if (!key || result.size() == 0)
			return false;

		std::list<std::string>::iterator it;
		for (it = result.begin(); it != result.end(); it++)
		{
			m_reply = (redisReply *)redisCommand(context_, "Lpush %s %b", key, it->c_str(), it->length());

			if (m_reply == NULL)
			{
				if (errstr)
					get_error_msg(REDIS_INTERRUPT, errstr);
				return false;
			}
			redisReplyGuard rrg(m_reply);

			if (m_reply->type == REDIS_REPLY_ERROR)
			{
				if (errstr)
					get_error_msg(REDIS_PROCESS_ERROR, errstr, m_reply->str);
				return false;
			}
		}

		return true;
	}

	bool pop(const char *key, std::string &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LPOP %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		result.append(m_reply->str, m_reply->len);
		return true;
	}

	bool pop(const char *key, int start, int end, std::list<std::string> &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LRANGE %s %d %d", key, start, end);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t j = 0; j < m_reply->elements; j++)
			{
				result.push_back(std::string(m_reply->element[j]->str, m_reply->element[j]->len));
			}
		}
		return true;
	}

	bool pop(const char *key, int start, std::string &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LRANGE %s %d %d", key, start, start);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			result = std::string(m_reply->element[0]->str, m_reply->element[0]->len);
		}
		return true;
	}

	bool del(const char *key, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "DEL %s", key);
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
		return true;
	}

	bool del(const char *key, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LREM %s 0 %b", key, value, len);
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
		return true;
	}

	bool Ltrim(const char *key, int start, int stop, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LTRIM %s %d %d", key, start, stop);
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
		return true;
	}
	uint64_t len(const char *key, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "LLEN %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return 0;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_INTEGER)
			return m_reply->integer;

		return 0;
	}

	bool set_add(const char *setname, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		if (!setname || !value)
			return false;

		m_reply = (redisReply *)redisCommand(context_, "SADD %s %b", setname, value, len);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_PROCESS_ERROR, errstr, m_reply->str);
			return false;
		}
	}

	bool set_add(const char *setname, std::list<std::string> &sets, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		if (!setname || sets.size() == 0)
			return false;

		std::string tmp = "";
		std::list<std::string>::iterator it;
		for (it = sets.begin(); it != sets.end(); it++)
		{
			if (it != sets.begin())
				tmp.append(" ");
			tmp.append(*it);
		}

		m_reply = (redisReply *)redisCommand(context_, "SADD %s %b", setname, tmp.c_str(), tmp.length());

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_PROCESS_ERROR, errstr, m_reply->str);
			return false;
		}

		return true;
	}

	bool set_get(const char *setname, std::list<std::string> &set, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "SMEMBERS %s", setname);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t j = 0; j < m_reply->elements; j++)
			{
				set.push_back(std::string(m_reply->element[j]->str, m_reply->element[j]->len));
			}
		}
		return true;
	}

	bool set_remove(const char *key, std::list<std::string> &sets, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		std::string tmp = "";
		std::list<std::string>::iterator it;
		for (it = sets.begin(); it != sets.end(); it++)
		{
			if (it != sets.begin())
				tmp.append(" ");
			tmp.append(*it);
		}

		m_reply = (redisReply *)redisCommand(context_, "SREM %s %b", key, tmp.c_str(), tmp.length());
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
		return true;
	}

	bool set_remove(const char *key, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "SREM %s %b", key, value, len);
		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type != REDIS_REPLY_ERROR)
			return true;
		else
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr, m_reply->str);
			return false;
		}
		return true;
	}

	int set_is_member(const char *key, const char *value, size_t len, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return -1;
		}

		m_reply = (redisReply *)redisCommand(context_, "SISMEMBER %s %b", key, value, len);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return -1;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_INTEGER)
			return m_reply->integer;

		return true;
	}

	bool sort(const char *key, std::string &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "SORT %s LIMIT 0 1 DESC", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_ARRAY)
		{
			if (m_reply->elements > 0)
				result = std::string(m_reply->element[0]->str, m_reply->element[0]->len);
		}
		return true;
	}

	bool pipe(vector<std::string> &vec_cmd, std::list<int> &result, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}
		for (size_t i = 0; i < vec_cmd.size(); i++)
		{
			redisAppendCommand(context_, vec_cmd[i].c_str());
		}
		//redisReplyGuard rrg(m_reply);
		for (size_t i = 0; i < vec_cmd.size(); i++)
		{
			if (REDIS_OK != redisgetReply(context_, (void **)&m_reply))
			{
				result.push_back(i);
			}

			freeReplyObject(m_reply);
		}
		return true;
	}

	uint64_t set_discard(const char *key, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "SCARD %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return 0;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_INTEGER)
			return m_reply->integer;

		return 0;
	}
	bool set_pop(const char *key, std::string *errstr)
	{
		if (context_ == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_NO_OBJ, errstr);
			return false;
		}

		m_reply = (redisReply *)redisCommand(context_, "SPOP %s", key);

		if (m_reply == NULL)
		{
			if (errstr)
				get_error_msg(REDIS_INTERRUPT, errstr);
			return false;
		}
		redisReplyGuard rrg(m_reply);

		if (m_reply->type == REDIS_REPLY_NIL)
		{
			return false;
		}
		else
			return true;
	}

	inline const std::string& ip() { return ip_; }
	inline size_t port() { return port_; }
	inline const std::string& db() { return db_; }

  private:
	void get_error_msg(enum ErrorCMD error, std::string *errstr, char *msg)
	{
		char str[1024] = {0};
		switch (error)
		{
		case REDIS_NO_OBJ:
		{
			errstr->append("CREATE_REDIS_ERROR");
		}
		break;
		case REDIS_INTERRUPT:
		{
			errstr->append("INTERRUPT:CHECK CONNECTION");
		}
		break;
		case REDIS_PROCESS_ERROR:
		{
			errstr->append("PROCESS_ERROR:");
			sprintf(str, "%s", msg);
			errstr->append(str);
		}
		break;
		case REDIS_get_NULL:
		{
			errstr->append("NOT_EXIST");
		}
		break;
		case REDIS_db_ERROR:
		{
			errstr->append("db_ERROR");
		}
		break;
		case REDIS_CONN_ERROR:
		{
			errstr->append("CONN_ERROR");
		}
		break;
		default:
			break;
		}
	}

	redisContext *context_;
	std::string ip_;
	size_t port_;
	int timeout_;
	std::string db_;

  private:
	redisReply *m_reply;
};

} // namespace XUtil

#endif//__H_XUTIL_XREDIS_HPP__
