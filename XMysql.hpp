#ifndef _H_XMYSQL_HPP_
#define _H_XMYSQL_HPP_

#pragma once

#include "XUtil.hpp"
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
// tinyint 一个字节8位 （0，255） 的整型数据
// smallint 两个字节16位 （-2^15 ，2^15 - 1） 的整型数据
// int 4个字节32位 （-2^31，2^31 - 1） 的整型数据
// bigint 8个字节64位 （-2^63，2^63 - 1） 的整型数据

class DataBuf : public std::streambuf
{
  public:
    DataBuf(char *buf, size_t len)
    {
        setg(buf, buf, buf + len);
    }
};

class MysqlConnect
{
  public:
    MysqlConnect(const std::string &ip, size_t port,
               const std::string &user, const std::string &pwd, const std::string &db, sql::Driver *driver)
               :ip_(ip),port_(port),user_(user),pwd_(pwd),db_(db),driver_(driver)
    {
        conn_ = nullptr;
        stmt_ = nullptr;
    }

    virtual ~MysqlConnect()
    {
        close();
    }

    bool connect()
    {
        close();

        std::stringstream ss;
        ss << "tcp://" << ip_ << ":" << port_;
        std::string url = ss.str();
        try
        {
            conn_ = driver_->connect(url.c_str(), user_, pwd_);
            conn_->setSchema(db_);
            if (conn_->isClosed())
            {
                LOG4E("connect mysql error, url = [%s], user = [%s], password = [%s]", url.c_str(), user_.c_str(), pwd_.c_str());
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

    sql::PreparedStatement* create_pstmt(const std::string& str_sql) {
        sql::PreparedStatement* pstmt = nullptr;
        try {
            pstmt = conn_->prepareStatement(str_sql);
        } catch (sql::SQLException &e) {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, MySQL Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return nullptr;
        }
        return pstmt;
    }

    sql::ResultSet *execute_query(const std::string& str_sql)
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

    int execute_update(const std::string& str_sql)
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

    int execute(const std::string& str_sql)
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

    inline const std::string &get_server_ip() { return ip_; }
    inline size_t get_server_port() { return port_; }
    inline const std::string &get_username() { return user_; }
    inline const std::string &get_passwrod() { return pwd_; }
    inline const std::string &get_db_name() { return db_; }
    inline sql::Driver *get_driver() { return driver_; }

  private:
    const std::string ip_;
    size_t port_;
    const std::string user_;
    const std::string pwd_;
    const std::string db_;

    sql::Driver *driver_;
    sql::Connection *conn_;
    sql::Statement *stmt_;
};

class MysqlManager
{
  public:
    MysqlManager(const std::string &ip, size_t port,
               const std::string &user, const std::string &pwd, const std::string &db
               , size_t max_conn_count = 15):ip_(ip),port_(port),user_(user),pwd_(pwd),db_(db),driver_(nullptr)
    //MysqlManager()::ip_(),port_(0),user_(),pwd_(),db_(),driver_(nullptr)
    {
       //
    }

    ~MysqlManager()
    {
        stop();
    }

    // bool start(const std::string &ip, size_t port,
    //            const std::string &user, const std::string &pwd, const std::string &db
    bool start(
               size_t conn_count = 3)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            try {
                driver_ = get_driver_instance();
            }
            catch (sql::SQLException &e) {
                LOG4E("mysql驱动连接出错;\n");
                return false;
            } catch (std::runtime_error &e) {
                LOG4E("mysql运行时错误\n");
                return false;
            }
            // ip_ = ip;
            // port_ = port;
            // user_ = user;
            // pwd_ = pwd;
            // db_ = db;
        }
        for (int i = 0; i < conn_count; i++)
        {
            std::shared_ptr<MysqlConnect> mysql_conn = std::make_shared<MysqlConnect>(ip_,port_,user_,pwd_,db_,driver_);
            if (!mysql_conn->connect()) {
                mysql_conn.reset();
                return false;
            }
            push(mysql_conn);
        }
        return true;
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        for (std::list<std::shared_ptr<MysqlConnect> >::iterator it = free_list_.begin(); it != free_list_.end(); it++)
        {
            std::shared_ptr<MysqlConnect> mysql_conn = *it;
            mysql_conn.reset();
        }
        free_list_.clear();
        // ip_.clear();
        // port_ = 0;
        // user_.clear();
        // pwd_.clear();
        // db_.clear();
        driver_ = nullptr;
        cv_.notify_all();
    }

    inline bool is_run() {
        return driver_ != nullptr && port_ != 0;
    }

    std::shared_ptr<MysqlConnect> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        do {
            if(!is_run()) {
                return nullptr;
            }
            cv_.wait(lock);
        }while (free_list_.empty());
        std::shared_ptr<MysqlConnect> mysql_conn = free_list_.front();
        free_list_.pop_front();
        return mysql_conn;
    }

    void push(std::shared_ptr<MysqlConnect> conn)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!is_run()) {
            return;
        }
        std::list<std::shared_ptr<MysqlConnect> >::iterator it = free_list_.begin();
        for (; it != free_list_.end(); it++)
        {
            if (*it == conn) {
                break;
            }
        }
        if (it == free_list_.end()) {
            free_list_.push_back(conn);
        }
        cv_.notify_one();
    }
    
    inline const std::string &ip() { return ip_; }
    inline size_t port() { return port_; }
    inline const std::string &username() { return user_; }
    inline const std::string &passwrod() { return pwd_; }
    inline const std::string &db() { return db_; }
    inline sql::Driver *driver() { return driver_; }

  private:
    const std::string ip_;
    size_t port_;
    const std::string user_;
    const std::string pwd_;
    const std::string db_;

    sql::Driver *driver_;
    std::list<std::shared_ptr<MysqlConnect>> free_list_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace XUtil

#endif //_H_XMYSQL_HPP_