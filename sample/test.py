import time
import numpy as np
from time_func import convert_time_stamp 
import datetime

def sort_by_value(d): 
  items=d.items() 
  backitems=[[v[1],v[0]] for v in items] 
  backitems.sort(reverse=True) 
  return [ backitems[i][1] for i in range(0,len(backitems))] 

#merge_dict将dict_b merge到dict a上
#dict_a、dict_b为三级索引，末级节点为整数，for ex:dict_a["buy"]["cate"]["phone"]=2
def merge_dict(dict_a, dict_b):
  for btag in dict_b:
    if btag not in dict_a:
      dict_a[btag] = {}
    for dim in dict_b[btag]:
      if dim not in dict_a[btag]:
        dict_a[btag][dim] = {}
      for ele in dict_b[btag][dim]:
        if ele not in dict_a[btag][dim]:
          dict_a[btag][dim][ele] = dict_b[btag][dim][ele]
        else:
          dict_a[btag][dim][ele] += dict_b[btag][dim][ele]

#cut_dict将dict b 从dict a中移走
#dict_a、dict_b为三级索引，末级节点为整数，for ex:dict_a["buy"]["cate"]["phone"]=2
def cut_dict(dict_a, dict_b):
  for btag in dict_b:
    for dim in dict_b[btag]:
      for ele in dict_b[btag][dim]:
          dict_a[btag][dim][ele] -= dict_b[btag][dim][ele]
          if dict_a[btag][dim][ele] == 0:
            del dict_a[btag][dim][ele]


if __name__ == '__main__':
  x={"a":{"aa":{"aaa":9},"ab":{"aba":2, "abb":1}},"b":{"aa":{"aaa":9},"ab":{"aba":2, "abb":1}}}
  y={"a":{"ab":{"aba":2, "abb":1}},"b":{"aa":{"aaa":9},"ab":{"aba":2, "abb":1}}}

  print("x:\n", x)
  print("y:\n", y)
  merge_dict(x, y)

  print("merged x:\n", x)

  cut_dict(x, y)
  
  print("cutted x:\n", x)

  z = {"1":20,"9":10,"3":15}
  print("z:\n",z)
  keys = sort_by_value(z)
  print("sorted keys:\n", keys)

  w = ["1:20","9:20","3:15"]
  print("w:\n", w)
  w.sort()
  print("sorted w:\n", w)

  time_stamp="1493741627"
  tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
  date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')
  time = (datetime.datetime(tm_year, tm_mon, tm_mday, tm_hour)).strftime('%Y%m%d%H') 
  print ("time", time)
  print ("now", datetime.datetime.now())

  new_time_stamp=int(time_stamp) - 3600 
  tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(str(new_time_stamp))
  date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')
  time = (datetime.datetime(tm_year, tm_mon, tm_mday, tm_hour)).strftime('%Y%m%d%H') 
  print ("time", time)
  print ("now", datetime.datetime.now())

  start_date = "20170506"
  print ("start_date:", start_date)
  for i in range(14):
    past_date = (datetime.datetime.strptime(start_date, '%Y%m%d') + datetime.timedelta(days=-(i+1))).strftime('%Y%m%d')
    print ("past_date:", past_date)
  for date in ["20170507", "20170508", "20170509", "20170510", "20170511", "20170512", "20170513"]:
    past_15_date = (datetime.datetime.strptime(date, '%Y%m%d') + datetime.timedelta(days=-15)).strftime('%Y%m%d')
    past_1_date = (datetime.datetime.strptime(date, '%Y%m%d') + datetime.timedelta(days=-1)).strftime('%Y%m%d')
    print("date:",date)
    print("past_15_date:",past_15_date)
    print("past_1_date:",past_1_date)
   
  hours = 24
  for i in range(hours-1,0,-1):
    print("hour:",i)

  datas = ["1:0:1", "0:1:0", "1:1:1"]
  print("orig:", datas)
  datas.sort()
  print("sorted:", datas)
