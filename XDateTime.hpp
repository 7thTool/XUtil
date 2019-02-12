#ifndef __H_XLOGGER_HPP_
#define __H_XLOGGER_HPP_

#pragma once

#include "XUtil.hpp"

namespace XUtil {

const int MAX_MINUTE_PER_HOUR      = 60;   //一小时分钟数
const int MAX_SECOND_PER_HOUR      = 3600; //一小时秒数
const int MAX_SECOND_PER_MINUTE    = 60;   //一分钟秒数
const int MAX_HOUR_PER_DAY         = 24;   //一天的小时数
const int MAX_MINUTE_PER_DAY       = 1440; //一天的分钟数
const int MAX_SECOND_PER_DAY       = 86400;//一天的秒数

//判断是否是闰年  
inline static bool IsLeapYear(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }
//构造日期
inline static int MakeDate(int y, int m, int d) { return y * 10000 + m * 100 + d; }
//构造时间
inline static int MakeTime(int h, int m, int s) { return h * 10000 + m * 100 + s; }
//获取时间HHMMSS格式中的小时数
inline static int GetHour(int time) { return time / 10000; }
//获取时间HHMMSS格式中的分钟数
inline static int GetMinute(int time) { return time / 100 % 100; }
//获取时间HHMMSS格式中的秒数
inline static int GetSecond(int time) { return time % 100; }
//获取时间YYYYMMDD格式中的年
inline static int GetYear(int date) { return date / 10000; }
//获取时间YYYYMMDD格式中的季
inline static int GetQuarter(int date) { return (date / 100 % 100 - 1) % 3; }
//获取时间YYYYMMDD格式中的月
inline static int GetMonth(int date) { return date / 100 % 100; }
//获取时间YYYYMMDD格式中的日
inline static int GetDay(int date) { return date % 100; }

//时间HHMMSS格式转换成分钟数
inline static int MinutesPerDay(int time)
{
    return GetHour(time) * MAX_MINUTE_PER_HOUR + GetMinute(time);
}

//分钟数转换成时间HHMMSS格式
inline static int MinutesToTimePerDay(int minutes)
{
    return minutes / MAX_MINUTE_PER_HOUR * 10000 + minutes % MAX_MINUTE_PER_HOUR * 100;
}

//日间分钟差，时间HHMMSS格式
inline static int DiffTimePerDay(int start, int end)
{
    return MinutesPerDay(end) - MinutesPerDay(start);
}

//将HHMMSS格式转换成秒数
inline static int SecondsPerDay(int time)
{
    return MinutesPerDay(time) * MAX_SECOND_PER_MINUTE + GetSecond(time);
}

const std::vector<int> MAX_DAYS_PER_MONTH = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const std::vector<int> MAX_DAYS_PER_MONTH_LEAP = { 31,29,31,30,31,30,31,31,30,31,30,31 };

//累计天数到某月
inline static std::vector<int> SumSUM_DAYS_OF_MONTH(bool leap_year)
{
	std::vector<int> ret;
	int sum = 0;
	ret.push_back(sum); //第0月设0,无公元零年，填充0
	ret.push_back(sum); //第1月设0,公元一年索引是1，比如查公元一年，传1
	for (int m = 0; m < 12; ++m) {
		if (leap_year) {
			sum += MAX_DAYS_PER_MONTH_LEAP[m];
		}
		else {
			sum += MAX_DAYS_PER_MONTH[m];
		}
		ret.push_back(sum);
	}
	return ret;
}

const std::vector<int> SUM_DAYS_OF_MONTH = SumSUM_DAYS_OF_MONTH(false);
const std::vector<int> SUM_DAYS_OF_MONTH_LEAP = SumSUM_DAYS_OF_MONTH(true);
//累计天数到某年，技巧：公元一年一月一日为星期一
const std::vector<int> SUM_DAYS_OF_YEAR = [] -> bool { 
	std::vector<int> ret;
	int sum = 0;
	ret.push_back(sum); //第0年设0,无公元零年，填充0
	ret.push_back(sum); //第1年设0,公元一年索引是1，比如查公元一年，传1
	for (int y = 1; y < 2099; ++y) {
		sum += (IsLeapYear(y) ? 366 : 365);
		ret.push_back(sum);
	}
	return ret;
}();

//累计天数到某天
inline static int DaysOfDate(int y, int m, int d)
{
	if (IsLeapYear(y)) {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH_LEAP[m] + d;
	}
	else {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH[m] + d;
	}
}

//累计天数到某月
inline static int DaysOfMonth(int y, int m)
{
	if (IsLeapYear(y)) {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH_LEAP[m];
	}
	else {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH[m];
	}
}

//周几
inline static int WeekdayOfDate(int y, int m, int d)
{
	int days = DaysOfDate(y, m, d);
	return days % 7;
}

//累计周数到某天
inline static int WeeksOfDate(int y, int m, int d)
{
	int days = DaysOfDate(y, m, d);
	return (days + (7 - 1)) / 7;
}

//一年的第几周
inline static int WeeksPerYear(int y, int m, int d)
{
	int days = DaysOfDate(y, m, d);
	int first_days = DaysOfDate(y, 1, 1);
	return (days + (7 - 1)) / 7 - (first_days + (7 - 1)) / 7;
}

//累计周数到某月
inline static int WeeksOfMonth(int y, int m)
{
	int days = DaysOfMonth(y, m);
	return (days + (7 - 1)) / 7;
}

//一月的第几周
inline static int WeeksPerMonth(int y, int m, int d)
{
	int days = DaysOfDate(y, m, d);
	int first_days = DaysOfMonth(y, m);
	return (days + (7 - 1)) / 7 - (first_days + (7 - 1)) / 7;
}

//同一周
inline static bool SameWeek(int olddate, int newdate)
{
	int olddays = DaysOfDate(GetYear(olddate), GetMonth(olddate), GetDay(olddate));
	int newdays = DaysOfDate(GetYear(newdate), GetMonth(newdate), GetDay(newdate));
	return (olddays / 7) == (newdays / 7);
}

//同一月
inline static bool SameMonth(int olddate, int newdate)
{
	int oldmonths = DaysOfDateMonth(GetYear(olddate), GetMonth(olddate));
	int newmonths = DaysOfDateMonth(GetYear(newdate), GetMonth(newdate));
	return (oldmonths / 7) == (newmonths / 7);
}

//同一季
inline static bool SameQuarter(int olddate, int newdate)
{
	int oldmonths = DaysOfDateMonth(GetYear(olddate), GetMonth(olddate));
	int newmonths = DaysOfDateMonth(GetYear(newdate), GetMonth(newdate));
	return (oldmonths / 3) == (newmonths / 3);
}

inline static time_t make_time_t(int year, int mon, int day, int hour = 0, int min = 0, int sec = 1)
{
	/*struct*/ tm _tm;
	_tm.tm_year = year;
	_tm.tm_mon = mon;
	_tm.tm_mday = day;
	_tm.tm_hour = hour;
	_tm.tm_min = min;
	_tm.tm_sec = sec;
	_tm.tm_isdst = 0;
	return mktime(&_tm);
}

inline static int get_current_time(int* time = nullptr)
{
	time_t _t = time(NULL);
	struct tm * timeinfo = localtime(&_t);
	int year = timeinfo->tm_year + 1900;
	int month = timeinfo->tm_mon + 1;
	int day = timeinfo->tm_mday;
	if (time) {
		*time = MakeTime(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	}
	return year * 10000 + month * 100 + day;
}

}

#endif//__H_XLOGGER_HPP_


