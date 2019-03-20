#pragma once
#ifndef __H_XDATETIME_HPP_
#define __H_XDATETIME_HPP_

#include "XUtil.hpp"

namespace XUtil {

const uint32_t MAX_MINUTE_PER_HOUR      = 60;   //一小时分钟数
const uint32_t MAX_SECOND_PER_HOUR      = 3600; //一小时秒数
const uint32_t MAX_SECOND_PER_MINUTE    = 60;   //一分钟秒数
const uint32_t MAX_HOUR_PER_DAY         = 24;   //一天的小时数
const uint32_t MAX_MINUTE_PER_DAY       = 1440; //一天的分钟数
const uint32_t MAX_SECOND_PER_DAY       = 86400;//一天的秒数

//判断是否是闰年  
inline static bool IsLeapYear(uint32_t y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }
//构造日期
inline static uint32_t MakeDate(uint32_t y, uint32_t m, uint32_t d) { return y * 10000 + m * 100 + d; }
//构造时间
inline static uint32_t MakeTime(uint32_t h, uint32_t m, uint32_t s) { return h * 10000 + m * 100 + s; }
//解构日期
inline static uint32_t SplitDate(uint32_t date, uint32_t& y, uint32_t& m, uint32_t& d) {
	y = date / 10000;
	m = date / 100 % 100;
	d = date % 100;
}
//解构时间
inline static uint32_t SplitTime(uint32_t time, uint32_t& h, uint32_t& m, uint32_t& s) {
	h = time / 10000;
	m = time / 100 % 100;
	s = time % 100;
}
//获取时间HHMMSS格式中的小时数
inline static uint32_t GetHour(uint32_t time) { return time / 10000; }
//获取时间HHMMSS格式中的分钟数
inline static uint32_t GetMinute(uint32_t time) { return time / 100 % 100; }
//获取时间HHMMSS格式中的秒数
inline static uint32_t GetSecond(uint32_t time) { return time % 100; }
//获取时间YYYYMMDD格式中的年
inline static uint32_t GetYear(uint32_t date) { return date / 10000; }
//获取时间YYYYMMDD格式中的季
inline static uint32_t GetQuarter(uint32_t date) { return (date / 100 % 100 - 1) % 3; }
//获取时间YYYYMMDD格式中的月
inline static uint32_t GetMonth(uint32_t date) { return date / 100 % 100; }
//获取时间YYYYMMDD格式中的日
inline static uint32_t GetDay(uint32_t date) { return date % 100; }

//时间HHMMSS格式转换成分钟数
inline static uint32_t TimeToMinutes(uint32_t time)
{
    return GetHour(time) * MAX_MINUTE_PER_HOUR + GetMinute(time);
}

//分钟数转换成时间HHMMSS格式
inline static uint32_t MinutesToTime(uint32_t minutes)
{
    return minutes / MAX_MINUTE_PER_HOUR * 10000 + minutes % MAX_MINUTE_PER_HOUR * 100;
}

//日间分钟差，时间HHMMSS格式
inline static uint32_t DiffTime(uint32_t start, uint32_t end)
{
    return TimeToMinutes(end) - TimeToMinutes(start);
}

//将HHMMSS格式转换成秒数
inline static uint32_t TimeToSeconds(uint32_t time)
{
    return TimeToMinutes(time) * MAX_SECOND_PER_MINUTE + GetSecond(time);
}

//分钟数转换成时间HHMMSS格式
inline static uint32_t SecondsToTime(uint32_t seconds)
{
	uint32_t minutes = seconds / MAX_SECOND_PER_MINUTE;
    return minutes / MAX_MINUTE_PER_HOUR * 10000 + minutes % MAX_MINUTE_PER_HOUR * 100 + seconds % MAX_SECOND_PER_MINUTE;
}

const std::vector<uint32_t> MAX_DAYS_PER_MONTH = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const std::vector<uint32_t> MAX_DAYS_PER_MONTH_LEAP = { 31,29,31,30,31,30,31,31,30,31,30,31 };

//累计天数到某月
inline static std::vector<uint32_t> SumSUM_DAYS_OF_MONTH(bool leap_year)
{
	std::vector<uint32_t> ret;
	uint32_t sum = 0;
	ret.push_back(sum); //第0月设0,无公元零年，填充0
	ret.push_back(sum); //第1月设0,公元一年索引是1，比如查公元一年，传1
	for (uint32_t m = 0; m < 12; ++m) {
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

const std::vector<uint32_t> SUM_DAYS_OF_MONTH = SumSUM_DAYS_OF_MONTH(false);
const std::vector<uint32_t> SUM_DAYS_OF_MONTH_LEAP = SumSUM_DAYS_OF_MONTH(true);
//累计天数到某年，技巧：公元一年一月一日为星期一
const std::vector<uint32_t> SUM_DAYS_OF_YEAR = [] { 
	std::vector<uint32_t> ret;
	uint32_t sum = 0;
	ret.push_back(sum); //第0年设0,无公元零年，填充0
	ret.push_back(sum); //第1年设0,公元一年索引是1，比如查公元一年，传1
	for (uint32_t y = 1; y < 2099; ++y) {
		sum += (IsLeapYear(y) ? 366 : 365);
		ret.push_back(sum);
	}
	return ret;
}();

//累计天数到某天
inline static uint32_t DaysOfDate(uint32_t y, uint32_t m, uint32_t d)
{
	if (IsLeapYear(y)) {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH_LEAP[m] + d;
	}
	else {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH[m] + d;
	}
}

//累计天数到某月
inline static uint32_t DaysOfMonth(uint32_t y, uint32_t m)
{
	if (IsLeapYear(y)) {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH_LEAP[m];
	}
	else {
		return SUM_DAYS_OF_YEAR[y] + SUM_DAYS_OF_MONTH[m];
	}
}

//周几
inline static uint32_t WeekdayOfDate(uint32_t y, uint32_t m, uint32_t d)
{
	uint32_t days = DaysOfDate(y, m, d);
	return days % 7;
}

//累计周数到某天
inline static uint32_t WeeksOfDate(uint32_t y, uint32_t m, uint32_t d)
{
	uint32_t days = DaysOfDate(y, m, d);
	return (days + (7 - 1)) / 7;
}

//一年的第几周
inline static uint32_t WeeksPerYear(uint32_t y, uint32_t m, uint32_t d)
{
	uint32_t days = DaysOfDate(y, m, d);
	uint32_t first_days = DaysOfDate(y, 1, 1);
	return (days + (7 - 1)) / 7 - (first_days + (7 - 1)) / 7;
}

//累计周数到某月
inline static uint32_t WeeksOfMonth(uint32_t y, uint32_t m)
{
	uint32_t days = DaysOfMonth(y, m);
	return (days + (7 - 1)) / 7;
}

//一月的第几周
inline static uint32_t WeeksPerMonth(uint32_t y, uint32_t m, uint32_t d)
{
	uint32_t days = DaysOfDate(y, m, d);
	uint32_t first_days = DaysOfMonth(y, m);
	return (days + (7 - 1)) / 7 - (first_days + (7 - 1)) / 7;
}

//同一周
inline static bool SameWeek(uint32_t olddate, uint32_t newdate)
{
	uint32_t olddays = DaysOfDate(GetYear(olddate), GetMonth(olddate), GetDay(olddate));
	uint32_t newdays = DaysOfDate(GetYear(newdate), GetMonth(newdate), GetDay(newdate));
	return (olddays / 7) == (newdays / 7);
}

//同一月
inline static bool SameMonth(uint32_t olddate, uint32_t newdate)
{
	uint32_t oldmonths = DaysOfMonth(GetYear(olddate), GetMonth(olddate));
	uint32_t newmonths = DaysOfMonth(GetYear(newdate), GetMonth(newdate));
	return (oldmonths / 7) == (newmonths / 7);
}

//同一季
inline static bool SameQuarter(uint32_t olddate, uint32_t newdate)
{
	uint32_t oldmonths = DaysOfMonth(GetYear(olddate), GetMonth(olddate));
	uint32_t newmonths = DaysOfMonth(GetYear(newdate), GetMonth(newdate));
	return (oldmonths / 3) == (newmonths / 3);
}

// inline static time_t make_time_t(uint32_t year, uint32_t mon, uint32_t day, uint32_t hour = 0, uint32_t min = 0, uint32_t sec = 1)
// {
// 	/*struct*/ tm _tm;
// 	_tm.tm_year = year;
// 	_tm.tm_mon = mon;
// 	_tm.tm_mday = day;
// 	_tm.tm_hour = hour;
// 	_tm.tm_min = min;
// 	_tm.tm_sec = sec;
// 	_tm.tm_isdst = 0;
// 	return mktime(&_tm);
// }

inline static uint32_t NowDateTime(uint32_t* t = nullptr)
{
	time_t _t = ::time(nullptr);
	struct tm * timeinfo = localtime(&_t);
	uint32_t year = timeinfo->tm_year + 1900;
	uint32_t month = timeinfo->tm_mon + 1;
	uint32_t day = timeinfo->tm_mday;
	if (t) {
		*t = MakeTime(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	}
	return year * 10000 + month * 100 + day;
}

inline static uint32_t PrevSecond(uint32_t hour, uint32_t minute, uint32_t second, uint32_t secs)
{
	boost::posix_time::time_duration t(hour, minute, second, 0);
	t -= boost::posix_time::seconds(secs);
	return MakeDate(t.hours(), t.minutes(), t.seconds());
}

inline static uint32_t NextSecond(uint32_t hour, uint32_t minute, uint32_t second, uint32_t secs)
{
	boost::posix_time::time_duration t(hour, minute, second, 0);
	t += boost::posix_time::seconds(secs);
	return MakeDate(t.hours(), t.minutes(), t.seconds());
}

inline static uint32_t PrevMinute(uint32_t hour, uint32_t minute, uint32_t mins)
{
	boost::posix_time::time_duration t(hour, minute, 0, 0);
	t += boost::posix_time::minutes(mins);
	return MakeDate(t.hours(), t.minutes(), t.seconds());
}

inline static uint32_t NextMinute(uint32_t hour, uint32_t minute, uint32_t mins)
{
	boost::posix_time::time_duration t(hour, minute, 0, 0);
	t += boost::posix_time::minutes(mins);
	return MakeDate(t.hours(), t.minutes(), t.seconds());
}

inline static uint32_t PrevDay(uint32_t year, uint32_t month, uint32_t day, uint32_t days)
{
	boost::gregorian::date d(year, month, day);
	d -= boost::gregorian::days(days);
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t NextDay(uint32_t year, uint32_t month, uint32_t day, uint32_t days)
{
	boost::gregorian::date d(year, month, day);
	d += boost::gregorian::days(days);
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t PrevWorkDay(uint32_t year, uint32_t month, uint32_t day, uint32_t days)
{
	boost::gregorian::date d(year, month, day);
	while (days>0)
	{
		bool workday = true;
		do 
		{
			d -= boost::gregorian::days(1);
			switch (d.day_of_week())
			{
			case 0:
			case 6:
				workday = false;
				break;
			default:
				workday = true;
				break;
			}
		} while (!workday);
		days--;
	} 
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t NextWorkDay(uint32_t year, uint32_t month, uint32_t day, uint32_t days)
{
	boost::gregorian::date d(year, month, day);
	while (days > 0)
	{
		bool workday = true;
		do
		{
			d += boost::gregorian::days(1);
			switch (d.day_of_week())
			{
			case 0:
			case 6:
				workday = false;
				break;
			default:
				workday = true;
				break;
			}
		} while (!workday);
		days--;
	}
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t PrevMonth(uint32_t year, uint32_t month, uint32_t days)
{
	boost::gregorian::date d(year, month, 0);
	d -= boost::gregorian::months(month);
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t NextMonth(uint32_t year, uint32_t month, uint32_t days)
{
	boost::gregorian::date d(year, month, 0);
	d += boost::gregorian::months(month);
	return MakeDate(d.year(), d.month(), d.day());
}

inline static uint32_t PrevDay(uint32_t date, uint32_t days)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return PrevDay(year, month, day, days);
}

inline static uint32_t NextDay(uint32_t date, uint32_t days)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return NextDay(year, month, day, days);
}

inline static uint32_t PrevWorkDay(uint32_t date, uint32_t days)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return PrevWorkDay(year, month, day, days);
}

inline static uint32_t NextWorkDay(uint32_t date, uint32_t days)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return NextWorkDay(year, month, day, days);
}

inline static uint32_t PrevMonth(uint32_t date, uint32_t months)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return PrevMonth(year, month, months);
}

inline static uint32_t NextMonth(uint32_t date, uint32_t months)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	return NextMonth(year, month, months);
}

inline static void PrevDateTime(uint32_t& date, uint32_t& time, uint32_t secs)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	uint32_t hour, minute, second;
	SplitTime(time, hour, minute, second);
	boost::posix_time::ptime pt(boost::gregorian::date(year, month, day), boost::posix_time::time_duration(hour, minute, second, 0));
	pt -= boost::posix_time::seconds(secs);
	boost::gregorian::date pt_date = pt.date();
	date = MakeDate(pt_date.year(), pt_date.month(), pt_date.day());
	boost::posix_time::time_duration pt_time = pt.time_of_day();
	time = MakeTime(pt_time.hours(), pt_time.minutes(), pt_time.seconds());
}

inline static void NextDateTime(uint32_t& date, uint32_t& time, uint32_t secs)
{
	uint32_t year, month, day;
	SplitDate(date, year, month, day);
	uint32_t hour, minute, second;
	SplitTime(time, hour, minute, second);
	boost::posix_time::ptime pt(boost::gregorian::date(year, month, day), boost::posix_time::time_duration(hour, minute, second, 0));
	pt += boost::posix_time::seconds(secs);
	boost::gregorian::date pt_date = pt.date();
	date = MakeDate(pt_date.year(), pt_date.month(), pt_date.day());
	boost::posix_time::time_duration pt_time = pt.time_of_day();
	time = MakeTime(pt_time.hours(), pt_time.minutes(), pt_time.seconds());
}

}

#endif//__H_XDATETIME_HPP_


