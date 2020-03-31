///////////////////////////////////////////////////////////
//  log4c.h
//  Declaration of the log4c
//  Created on:      31-3月-2020 11:46:18
//  Original author: zhangzhiqiang
//  Update  history: Please add modify record
//  31-3月-2020 11:46:18    :create by zhangzhiqiang
///////////////////////////////////////////////////////////
#pragma once
#include <cstdio>

#if defined(__cplusplus)
extern "C" {
#endif//

enum LOG4C_LEVEL {
    LOG4C_ERROR = 1,
    LOG4C_WARN,
    LOG4C_INFO,
    LOG4C_DEBUG,
};

int LOG4C_Init(const char *path, const char *name);
void LOG4C_Term();

void LOG4C_Lock();
void LOG4C_UnLock();

void LOG4C_SetLevel(LOG4C_LEVEL level);
int LOG4C_GetLevel();

const char * LOG4C_FormatCurTime(char* buf, int len, const char* format);

FILE* LOG4C_GetFile(int level);

#define LOG4P(format,...) printf("%s "format"\n", __FUNCTION__, ##__VA_ARGS__)

#define LOG4C(level,format, ...) \
    do { \
        LOG4C_Lock(); \
        if(LOG4C_GetLevel() >= level) { \
            FILE* fp = LOG4C_GetFile(level); \
            if (fp) { \
                char time[64] = {0}; \
                fprintf(fp, "%s "format"\n", LOG4C_FormatCurTime(time,sizeof(time)-1,"%Y-%m-%d %H:%M:%S"), ##__VA_ARGS__); \
                fflush(fp); \
            } \
        } \
        LOG4C_UnLock(); \
    } while(0)

#define LOG4E(format,...) LOG4C(LOG4C_ERROR, "[ERROR] [%s:%d] "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG4W(format,...) LOG4C(LOG4C_WARN, "[WARN] "format, ##__VA_ARGS__)
#define LOG4I(format,...) LOG4C(LOG4C_INFO, "[INFO] "format, ##__VA_ARGS__)
#define LOG4D(format,...) LOG4C(LOG4C_DEBUG, "[DEBUG] [%s] "format, __FUNCTION__, ##__VA_ARGS__)

#if defined(__cplusplus)
}

// namespace LOG4C {

// class LockGuard
// {
// public:
//     LockGuard()
//     {
//         LOG4C_Lock();
//     }

//     ~LockGuard()
//     {
//         LOG4C_UnLock();
//     }
// }

// }

#endif//
