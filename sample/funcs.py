# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类

#缓存一个时间戳对应的日期和小时
dates={}
hours={}

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

#返回的date为绝对日期，譬如"20170513"
#返回的hour为24小时制，范围【0，23】
def convert_time_stamp_v3(time_stamp):
  time_stamp  = int(time_stamp)
  struct_time = time.localtime(time_stamp)

  tm_year = str(struct_time.tm_year)

  tm_mon = str(struct_time.tm_mon)
  if struct_time.tm_mon < 10:
    tm_mon = "0" + str(struct_time.tm_mon)

  tm_mday = str(struct_time.tm_mday)
  if struct_time.tm_mday < 10:
    tm_mday = "0" + str(struct_time.tm_mday)

  tm_hour = str(struct_time.tm_hour)
  if struct_time.tm_hour < 10:
    tm_hour = "0" + str(struct_time.tm_hour)

  date = tm_year + tm_mon + tm_mday

  return date, tm_hour

#返回的date为绝对日期，譬如"2017-05-13"
#返回的hour为24小时制，范围【00，23】
def convert_time_stamp(time_stamp):
  if time_stamp in dates:
    return dates[time_stamp], hours[time_stamp]

  time_str = datetime.datetime.fromtimestamp(int(time_stamp))
  date = time_str.date().isoformat()
  tm_hour = str(time_str.hour)

  dates[time_stamp] = date
  hours[time_stamp] = tm_hour
  
  return date, tm_hour

#返回的date为相对于1970101的天数，譬如"17299"实际代表"20170513"
#返回的hour为24小时制，范围【0，23】
def convert_time_stamp_v2(time_stamp):
  date = int(time_stamp/86400)
  hour=(int(time_stamp/3600))%24

  return str(date), str(hour)

def minus_1hour(old_date, old_hour, new_time_stamp)
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
  print(convert_time_stamp(1494159710))
  print(convert_time_stamp(1635164898))
  print(convert_time_stamp(1635166740))

  print(convert_time_stamp_v2(1494159710))
  print(convert_time_stamp_v2(1635164898))
  print(convert_time_stamp_v2(1635166740))

  print(convert_time_stamp_v3(1494159710))
  print(convert_time_stamp_v3(1635164898))
  print(convert_time_stamp_v3(1635166740))

  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v1 start")
  for i in range(10000000):
    convert_time_stamp(1494159710)
  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v1 end")

  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v2 start")
  for i in range(10000000):
    convert_time_stamp_v2(1494159710)
  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v2 end")

  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v3 start")
  for i in range(10000000):
    convert_time_stamp_v3(1494159710)
  print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), ": v3 end")
