#ifndef _H_XMYSQL_HPP_
#define _H_XMYSQL_HPP_

#pragma once

#include <XUtil.hpp>
#include <boost/circular_buffer.hpp>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

namespace XUtil
{

class DataBuf : public std::streambuf
{
  public:
    DataBuf(char *buf, size_t len)
    {
        setg(buf, buf, buf + len);
    }
};

class Mysql_Pool;
class Mysql_Conn
{
  public:
    Mysql_Conn(Mysql_Pool *mysql_pool)
    {
        mysql_pool_ = mysql_pool;
        conn_ = nullptr;
        stmt_ = nullptr;
    }

    virtual ~Mysql_Conn()
    {
        close();
    }

    bool connect()
    {
        close();

        std::stringstream ss;
        ss << "tcp://" << mysql_pool_->get_server_ip() << ":" << mysql_pool_->get_server_port();
        std::string url = ss.str();
        try
        {
            conn_ = mysql_pool_->get_driver()->connect(url.c_str(), mysql_pool_->get_username(), mysql_pool_->get_passwrod());
            conn_->setSchema(mysql_pool_->get_db_name());
            if (conn_->isClosed())
            {
                LOG4E("connect mysql error, url = [%s], user = [%s], password = [%s]", url.c_str(), mysql_pool_->get_username().c_str(), mysql_pool_->get_passwrod().c_str());
                return false;
            }
            else
            {
                stmt_ = conn_->createStatement();
            }
        }
        catch (sql::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return false;
        }
        return true;
    }

    void close()
    {
        if (stmt_)
        {
            delete stmt_;
            stmt_ = nullptr;
        }
        if (conn_)
        {
            delete conn_;
            conn_ = nullptr;
        }
    }

    inline bool is_connect()
    {
        return conn_ && !conn_.isClosed();
    }

    /*sql::PreparedStatement* create_pstmt(const char* str_sql) {
	sql::PreparedStatement* pstmt = nullptr;

	try {
		pstmt = conn_->prepareStatement(str_sql);
	} catch (sql::SQLException &e) {
		int err_code = e.getErrorCode();
		LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
		return nullptr;
	}
	return pstmt;
}*/

    std::string &get_pool()
    {
        return mysql_pool_;
    }

    sql::ResultSet *execute_query(const char *str_sql)
    {
        sql::ResultSet *res = nullptr;
        try
        {
            res = stmt_->executeQuery(str_sql);
        }
        catch (sql::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return nullptr;
        }
        return res;
    }

    int execute_update(const char *str_sql)
    {
        int ret = 0;
        try
        {
            ret = stmt_->executeUpdate(str_sql);
        }
        catch (sql::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return -1;
        }
        return ret;
    }

    int execute(const char *str_sql)
    {
        int ret = 0;
        try
        {
            ret = stmt_->execute(str_sql);
        }
        catch (sql::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return -1;
        }
        return ret;
    }

  private:
    Mysql_Pool *mysql_pool_;
    sql::Connection *conn_;
    sql::Statement *stmt_;
};

class Mysql_Pool
{
  public:
    Mysql_Pool(const std::string &pool_name, const std::string &server_ip, size_t server_port,
               const std::string &username, const std::string &password, const std::string &db_name, size_t max_conn_cnt)
    {
        pool_name_ = pool_name;
        server_ip_ = server_ip;
        server_port_ = server_port;
        username_ = username;
        password_ = password;
        db_name_ = db_name;
        max_conn_cnt_ = max_conn_cnt;
        cur_conn_cnt_ = 0;
        driver_ = nullptr;
    }

    virtual ~Mysql_Pool()
    {
        stop();
    }

    bool start(size_t init_conn_cnt = 2)
    {
        try
        {
            driver_ = get_driver_instance();
        }
        catch (sql::SQLException &e)
        {
            LOG4E("mysql驱动连接出错;\n");
            return false;
        }
        catch (std::runtime_error &e)
        {
            LOG4E("mysql运行时错误\n");
            return false;
        }
        cur_conn_cnt_ = init_conn_cnt;
        for (int i = 0; i < cur_conn_cnt_; i++)
        {
            Mysql_Conn *mysql_conn = new Mysql_Conn(this);
            if (!mysql_conn->connect())
            {
                delete mysql_conn;
                return false;
            }
            free_list_.push_back(mysql_conn);
        }

        LOG4I("db pool: %s, size: %d", pool_name_.c_str(), (int)free_list_.size());
        return 0;
    }

    void stop()
    {
        for (std::list<Mysql_Conn *>::iterator it = free_list_.begin(); it != free_list_.end(); it++)
        {
            Mysql_Conn *mysql_conn = *it;
            delete mysql_conn;
        }
        free_list_.clear();
    }

    Mysql_Conn *pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (free_list_.empty())
        {
            if (cur_conn_cnt_ >= max_conn_cnt_)
            {
                cv_.wait(lock);
            }
            else
            {
                Mysql_Conn *mysql_conn = new Mysql_Conn(this);
                bool ret = mysql_conn->connect();
                if (ret)
                {
                    delete mysql_conn;
                    return nullptr;
                }
                else
                {
                    free_list_.push_back(mysql_conn);
                    cur_conn_cnt_++;
                    LOG4I("new db connection: %s, conn_cnt: %d", pool_name_.c_str(), cur_conn_cnt_);
                }
            }
        }
        Mysql_Conn *mysql_conn = free_list_.front();
        free_list_.pop_front();
        return mysql_conn;
    }

    void push(Mysql_Conn *conn)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        std::list<Mysql_Conn *>::iterator it = free_list_.begin();
        for (; it != free_list_.end(); it++)
        {
            if (*it == conn)
            {
                break;
            }
        }
        if (it == free_list_.end())
        {
            free_list_.push_back(conn);
        }
        cv_.notify_one();
    }

    inline std::string &get_pool_name() { return pool_name_; }
    inline std::string &get_server_ip() { return server_ip_; }
    inline size_t get_server_port() { return server_port_; }
    inline std::string &get_username() { return username_; }
    inline std::string &get_passwrod() { return password_; }
    inline std::string &get_db_name() { return db_name_; }
    inline sql::Driver *get_driver() { return driver_; }
    inline size_t get_conn_count() { return cur_conn_cnt_; }

  private:
    std::string pool_name_;
    std::string server_ip_;
    size_t server_port_;
    std::string username_;
    std::string password_;
    std::string db_name_;
    size_t cur_conn_cnt_;
    size_t max_conn_cnt_;

    sql::Driver *driver_;
    std::list<Mysql_Conn *> free_list_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace XUtil

#endif //_H_XMYSQL_HPP_