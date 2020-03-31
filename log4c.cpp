///////////////////////////////////////////////////////////
//  log4c.cpp
//  Implementation of the log4c
//  Created on:      31-3月-2020 11:46:18
//  Original author: zhangzhiqiang
///////////////////////////////////////////////////////////

#include "log4c.h"
#include <time.h>
#include <sstream>
#include <string>
#include <assert.h>
#include <mutex>

class CLOG4CMgr
{
protected:
    std::string path_;
    std::string name_;
    FILE* fp_[5] = { 0 };
    int date_ = 0;
    int level_ = LOG4C_INFO;
    std::mutex mutex_;
    std::string GetFileName(int level, int date) 
    {
		std::ostringstream ss;
		ss << path_ << "/" << name_;
        ss << "_";
        switch (level)
        {
        case LOG4C_ERROR:
            ss << "error_";
            break;
        case LOG4C_WARN:
            ss << "warn_";
            break;
        case LOG4C_INFO:
            ss << "info_";
            break;
        case LOG4C_DEBUG:
            ss << "debug_";
            break;
        default:
            assert(0);
            break;
        }
        ss << date << ".log";
		return ss.str();
    }
    void OpenFile(int level, int date)
    {
        assert(!fp_[level]);
        std::string filename = GetFileName(level, date);
        fp_[level] = fopen(filename.c_str(), "wt");
        if(fp_[level]) {
            LOG4P("LOG4C open %s success!\n", filename.c_str());
        } else {
            LOG4P("LOG4C open %s failed!\n", filename.c_str());
        }
    }
    void CloseFile(int level) 
    {
        if(fp_[level]) {
            fclose(fp_[level]);
            fp_[level] = 0;
        }
    }
public:
    static CLOG4CMgr& Inst() {
        static CLOG4CMgr _mgr;
        return _mgr;
    }

    int Init(const char *path, const char *name)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        path_ = path;
        name_ = name;
        return 0;
    }

    void Term()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = 0;
        for(int i = 0; i < 5; i++)
        {
            CloseFile(i);
        }
    }

    void Lock()
    {
        mutex_.lock();
    }

    void UnLock()
    {
        mutex_.unlock();
    }

    void SetLevel(LOG4C_LEVEL level)
    {
        level_ = level;
    }

    int GetLevel()
    {
        return level_;
    }

    FILE* GetFile(int level)
    {
        time_t tt;
        time(&tt);
        tm *tp = localtime(&tt);
        int date = (tp->tm_year + 1900) * 10000 + (tp->tm_mon + 1) * 100 + tp->tm_mday;
        if(!fp_[level] || date_ != date) {
            CloseFile(level);
            date_ = date;
            OpenFile(level, date);
        }
        return fp_[level];
    }
};

int LOG4C_Init(const char *path, const char *name)
{
    return CLOG4CMgr::Inst().Init(path, name);
}

void LOG4C_Term()
{
    CLOG4CMgr::Inst().Term();
}

void LOG4C_Lock()
{
    CLOG4CMgr::Inst().Lock();
}

void LOG4C_UnLock()
{
    CLOG4CMgr::Inst().UnLock();
}

void LOG4C_SetLevel(LOG4C_LEVEL level)
{
    CLOG4CMgr::Inst().SetLevel(level);
}

int LOG4C_GetLevel()
{
    return CLOG4CMgr::Inst().GetLevel();
}

const char * LOG4C_FormatCurTime(char* buf, int len, const char* format)
{
    time_t tt;
    time(&tt);
    strftime(buf, len, format, localtime(&tt));
    return buf;
}

FILE* LOG4C_GetFile(int level)
{
    return CLOG4CMgr::Inst().GetFile(level);
}

