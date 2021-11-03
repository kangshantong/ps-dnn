# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类

def convert_time_stamp(time_stamp):
  time_stamp  = int(time_stamp)
  tm_year = time.localtime(time_stamp).tm_year
  tm_mon = time.localtime(time_stamp).tm_mon
  tm_mday = time.localtime(time_stamp).tm_mday
  tm_wday = time.localtime(time_stamp).tm_wday
  tm_hour = time.localtime(time_stamp).tm_hour
  tm_min = time.localtime(time_stamp).tm_min
  tm_sec = time.localtime(time_stamp).tm_sec
  
  workdayflag =1 if tm_wday<=4 else 0

  return tm_year, tm_mon, tm_mday, workdayflag, tm_hour



if __name__ == '__main__':
  n=-1
  date = datetime.datetime(2017,5,6) + datetime.timedelta(days=n)
  time_format = date.strftime('%Y%m%d')
  print (date)
  print (time_format)
  date = datetime.datetime.strptime("20170506", '%Y%m%d')
  time_format = date.strftime('%Y%m%d')
  print (date)
  print (time_format)
