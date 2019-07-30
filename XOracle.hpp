#ifndef _H_XORACLE_HPP_
#define _H_XORACLE_HPP_

#pragma once

#include "XUtil.hpp"
#include <boost/circular_buffer.hpp>
#include "occi.h" //occi头文件

//声明命名空间
using namespace oracle::occi;

namespace XUtil
{
// tinyint 一个字节8位 （0，255） 的整型数据
// smallint 两个字节16位 （-2^15 ，2^15 - 1） 的整型数据
// int 4个字节32位 （-2^31，2^31 - 1） 的整型数据
// bigint 8个字节64位 （-2^63，2^63 - 1） 的整型数据

class OracleConnect
{
  public:
    OracleConnect(const std::string &ip, size_t port,
               const std::string &user, const std::string &pwd, const std::string &db, oracle::occi::Environment *env)
               :ip_(ip),port_(port),user_(user),pwd_(pwd),db_(db),env_(env)
    {
        conn_ = nullptr;
        stmt_ = nullptr;
    }

    virtual ~OracleConnect()
    {
        close();
    }

    bool connect(std::string connstring)
    {
        close();

        try
        {
            conn_ = env_->createConnection(user_, pwd_,connstring);
            if (conn_->isClosed())
            {
                LOG4E("connect Oracle error, url = [%s], user = [%s], password = [%s]", connstring.c_str(), user_.c_str(), pwd_.c_str());
                return false;
            }
            else
            {
                stmt_ = conn_->createStatement();
            }
        }
        catch (oracle::occi::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, Oracle Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return false;
        }
        return true;
    }

    void close()
    {
        if (stmt_)
        {
            conn_->terminateStatement(stmt);
            stmt_ = nullptr;
        }
        if (conn_)
        {
            env_->terminateConnection(conn);//断开数据库连接
            conn_ = nullptr;
        }
    }

    oracle::occi::PreparedStatement* create_pstmt(const std::string& str_sql) {
        oracle::occi::PreparedStatement* pstmt = nullptr;
        try {
            pstmt = conn_->prepareStatement(str_sql);
        } catch (oracle::occi::SQLException &e) {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, Oracle Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return nullptr;
        }
        return pstmt;
    }

    oracle::occi::ResultSet *execute_query(const std::string& str_sql)
    {
        oracle::occi::ResultSet *res = nullptr;
        try
        {
            stmt_->setSQL(str_sql);
            res = stmt_->executeQuery();
        }
        catch (oracle::occi::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, Oracle Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return nullptr;
        }
        return res;
    }

    int execute_update(const std::string& str_sql)
    {
        int ret = 0;
        try
        {
            stmt_->setSQL(str_sql);
            ret = stmt_->executeUpdate();
        }
        catch (oracle::occi::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, Oracle Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return -1;
        }
        return ret;
    }

    int execute(const std::string& str_sql)
    {
        int ret = 0;
        try
        {
            stmt_->setSQL(str_sql);
            ret = stmt_->execute();
        }
        catch (oracle::occi::SQLException &e)
        {
            int err_code = e.getErrorCode();
            LOG4E("SQLException, Oracle Error Code = %d, SQLState = [%s], [%s]", err_code, e.getSQLState().c_str(), e.what());
            return -1;
        }
        return ret;
    }

    inline const std::string &get_server_ip() { return ip_; }
    inline size_t get_server_port() { return port_; }
    inline const std::string &get_username() { return user_; }
    inline const std::string &get_passwrod() { return pwd_; }
    inline const std::string &get_db_name() { return db_; }
    inline oracle::occi::Environment *get_env() { return env_; }

  private:
    const std::string ip_;
    size_t port_;
    const std::string user_;
    const std::string pwd_;
    const std::string db_;

    oracle::occi::Environment *env_;
    oracle::occi::Connection *conn_;
    oracle::occi::Statement *stmt_;
};

class OracleManager
{
  public:
    OracleManager(const std::string &ip, size_t port,
               const std::string &user, const std::string &pwd, const std::string &db
               , size_t max_conn_count = 15):ip_(ip),port_(port),user_(user),pwd_(pwd),db_(db),env_(nullptr)
    //OracleManager()::ip_(),port_(0),user_(),pwd_(),db_(),env_(nullptr)
    {
       //
    }

    ~OracleManager()
    {
        stop();
    }

    // bool start(const std::string &ip, size_t port,
    //            const std::string &user, const std::string &pwd, const std::string &db
    bool start(
               size_t conn_count = 3)
    {
        env_ = oracle::occi::Environment::createEnvironment();//创建环境变量
        for (int i = 0; i < conn_count; i++)
        {
            std::shared_ptr<OracleConnect> Oracle_conn = std::make_shared<OracleConnect>(ip_,port_,user_,pwd_,db_,env_);
            if (!Oracle_conn->connect()) {
                Oracle_conn.reset();
                return false;
            }
            push(Oracle_conn);
        }
        return true;
    }

    void stop()
    {
        if(!env_) {
            return;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        for (std::list<std::shared_ptr<OracleConnect> >::iterator it = free_list_.begin(); it != free_list_.end(); it++)
        {
            std::shared_ptr<OracleConnect> Oracle_conn = *it;
            Oracle_conn.reset();
        }
        free_list_.clear();
        // ip_.clear();
        // port_ = 0;
        // user_.clear();
        // pwd_.clear();
        // db_.clear();
        oracle::occi::Environment::terminateEnvironment(env);// 终止环境变量
        cv_.notify_all();
    }

    inline bool is_run() {
        return env_ != nullptr && port_ != 0;
    }

    std::shared_ptr<OracleConnect> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        do {
            if(!is_run()) {
                return nullptr;
            }
            cv_.wait(lock);
        }while (free_list_.empty());
        std::shared_ptr<OracleConnect> Oracle_conn = free_list_.front();
        free_list_.pop_front();
        return Oracle_conn;
    }

    void push(std::shared_ptr<OracleConnect> conn)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!is_run()) {
            return;
        }
        std::list<std::shared_ptr<OracleConnect> >::iterator it = free_list_.begin();
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
    inline oracle::occi::Environment *driver() { return env_; }

  private:
    const std::string ip_;
    size_t port_;
    const std::string user_;
    const std::string pwd_;
    const std::string db_;

    oracle::occi::Environment *env_;
    std::list<std::shared_ptr<OracleConnect>> free_list_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace XUtil

#endif //_H_XORACLE_HPP_