#ifndef __H_XLOGGER_HPP_
#define __H_XLOGGER_HPP_

#pragma once

#include "XUtil.hpp"

namespace XUtil {

const int MinuteOfHour      = 60;   //一小时分钟数
const int SecondOfHour      = 3600; //一小时秒数
const int SecondOfMinute    = 60;   //一分钟秒数
const int HourOfDay         = 24;   //一天的小时数
const int MinuteOfDay       = 1440; //一天的分钟数
const int SecondOfDay       = 86400;//一天的秒数

//判断是否是闰年  
inline static bool IsLeapYear(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }
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
inline static int MinutesOfDay(int time)
{
    return GetHour(time) * MinuteOfHour + GetMinute(time);
}

//分钟数转换成时间HHMMSS格式
inline static int MinutesOfDayToTime(int time)
{
    return time / MinuteOfHour * 10000 + time % MinuteOfHour * 100;
}

//日间分钟差，时间HHMMSS格式
inline static int DiffTimeOfDay(int start, int end)
{
    return MinutesOfDay(end) - MinutesOfDay(start);
}

//将HHMMSS格式转换成秒数
inline static int SecondsOfDay(int time)
{
    return MinutesOfDay(time) * SecondOfMinute + GetSecond(time);
}

const std::vector<int> DaysPerMonth = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const std::vector<int> DaysPerMonth_Leap = { 31,29,31,30,31,30,31,31,30,31,30,31 };

//累计天数到某月
inline static std::vector<int> SumDaysOfMonth(bool leap_year)
{
	std::vector<int> ret;
	int sum = 0;
	ret.push_back(sum); //第0月设0,无公元零年，填充0
	ret.push_back(sum); //第1月设0,公元一年索引是1，比如查公元一年，传1
	for (int m = 0; m < 12; ++m) {
		if (leap_year) {
			sum += DaysPerMonth_Leap[m];
		}
		else {
			sum += DaysPerMonth[m];
		}
		ret.push_back(sum);
	}
	return ret;
}

const std::vector<int> DaysOfMonth = SumDaysOfMonth(false);
const std::vector<int> DaysOfMonth_Leap = SumDaysOfMonth(true);
//累计天数到某年，技巧：公元一年一月一日为星期一
const std::vector<int> DaysOfYear = [] -> bool { 
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
		return DaysOfYear[y] + DaysOfMonth_Leap[m] + d;
	}
	else {
		return DaysOfYear[y] + DaysOfMonth[m] + d;
	}
}

//累计天数到某月
inline static int DaysOfDateMonth(int y, int m)
{
	if (IsLeapYear(y)) {
		return DaysOfYear[y] + DaysOfMonth_Leap[m];
	}
	else {
		return DaysOfYear[y] + DaysOfMonth[m];
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
	return days / 7;
}

//累计周数到某月
inline static int WeeksOfDate(int y, int m)
{
	int days = DaysOfDateMonth(y, m);
	return days / 7;
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

inline static time_t MakeTime(int year, int mon, int day, int hour = 0, int min = 0, int sec = 1)
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

inline static NowTime(int* time = nullptr)
{
	time_t _t = time(NULL);
	struct tm * timeinfo = localtime(&_t);
	int year = timeinfo->tm_year + 1900;
	int month = timeinfo->tm_mon + 1;
	int day = timeinfo->tm_mday;
	if (time) {
		//time = 
	}
	return year * 10000 + month * 100 + day;
}

}

#endif//__H_XLOGGER_HPP_


