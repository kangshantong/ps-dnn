# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类

mins_range=15

#二分查找，找到比time_stamp小并且距离time_stamp最近的数组下标
def binary_search(user_action_dict, time_stamp):
  left = 0
  right = len(user_action_dict) - 1
  while (left <= right):
    mid = int((left+right)/2)
    user_action = user_action_dict[mid]
    fields = user_action.split(":")
    action_time_stamp = fields[0]
    if (action_time_stamp >= time_stamp):
      right = mid - 1
    else:
      left = mid + 1

  return left

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

  tm_min_range = int(tm_min/mins_range)

  return tm_year, tm_mon, tm_mday, workdayflag, str(tm_hour), str(tm_min_range)



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
  print(convert_time_stamp(1635163315))
  print(convert_time_stamp(1635164898))
  print(convert_time_stamp(1635166740))
