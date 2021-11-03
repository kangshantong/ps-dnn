# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类
import numpy as np
from time_func import convert_time_stamp 
import json
import copy

#将dict按照value降序排列，返回key的list
def sort_by_value(d): 
  items=d.items() 
  backitems=[[v[1],v[0]] for v in items] 
  backitems.sort(reverse=True) 
  return [ backitems[i][1] for i in range(0,len(backitems))] 

#copy_dict将dict_b copy到dict a上
#dict_a、dict_b为三级索引，末级节点为整数，for ex:dict_a["buy"]["cate"]["phone"]=2
def copy_dict(dict_a, dict_b):
  dict_a = {}
  for btag in dict_b:
    dict_a[btag] = {}
    for dim in dict_b[btag]:
      dict_a[btag][dim] = {}
      for ele in dict_b[btag][dim]:
        dict_a[btag][dim][ele] = dict_b[btag][dim][ele]

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
          if dict_a[btag][dim][ele] < 0:
            print("Error:dict_b is not a subset of dict_a")
          if dict_a[btag][dim][ele] == 0:
            del dict_a[btag][dim][ele]

#从用户的行为序列中找到分位点，也就是离time_stamp最近的行为序号
def find_latest_index(user_action_dict, time_stamp):
  act_num = len(user_action_dict)
  index = -1
  for i in range(act_num):
    user_action = user_action_dict[i]
    fields = user_action.split(":")
    action_time_stamp = fields[0]
    if (action_time_stamp >= time_stamp):
      break
    else:
      index = i
  return index

#将用户的行为按照userid*日期*行为类型*维度【类目、品牌】*次数，存储起来
def add_user_behavior_dict_1day_global(user_behavior_dict_1day_global,userid,date,btag,cate,brand):

  if userid not in user_behavior_dict_1day_global:
    user_behavior_dict_1day_global[userid] = {}

  if date not in user_behavior_dict_1day_global[userid]:
    user_behavior_dict_1day_global[userid][date] = {}

  if btag not in user_behavior_dict_1day_global[userid][date]:
    user_behavior_dict_1day_global[userid][date][btag] = {}
    user_behavior_dict_1day_global[userid][date][btag]["cate"] = {}
    user_behavior_dict_1day_global[userid][date][btag]["brand"] = {}

  if cate not in user_behavior_dict_1day_global[userid][date][btag]["cate"]:
    user_behavior_dict_1day_global[userid][date][btag]["cate"][cate] = 1
  else:
    user_behavior_dict_1day_global[userid][date][btag]["cate"][cate] += 1

  if brand not in user_behavior_dict_1day_global[userid][date][btag]["brand"]:
    user_behavior_dict_1day_global[userid][date][btag]["brand"][brand] = 1
  else:
    user_behavior_dict_1day_global[userid][date][btag]["brand"][brand] += 1

#将用户的行为按照userid*日期*行为类型*维度【类目、品牌】*时间戳，存储起来
def add_user_behavior_dict_1day_global_realtime(user_behavior_dict_1day_global_realtime,userid,date,time_stamp,btag,cate,brand):

  if date not in ["20170506", "20170507", "20170508", "20170509", "20170510", "20170511", "20170512", "20170513"]:
    return

  if userid not in user_behavior_dict_1day_global_realtime:
    user_behavior_dict_1day_global_realtime[userid] = {}

  if date not in user_behavior_dict_1day_global_realtime[userid]:
    user_behavior_dict_1day_global_realtime[userid][date] = {}

  if btag not in user_behavior_dict_1day_global_realtime[userid][date]:
    user_behavior_dict_1day_global_realtime[userid][date][btag] = {}
    user_behavior_dict_1day_global_realtime[userid][date][btag]["cate"] = []
    user_behavior_dict_1day_global_realtime[userid][date][btag]["brand"] = []
    
  cate_behavior = time_stamp + ":" + cate
  brand_behavior = time_stamp + ":" + brand

  user_behavior_dict_1day_global_realtime[userid][date][btag]["cate"].append(cate_behavior)
  user_behavior_dict_1day_global_realtime[userid][date][btag]["brand"].append(brand_behavior)

def gen_user_behavior_1day_global(user_behavior_dict_1day_global, user_behavior_dict_1day_global_realtime):
  count = 0
  with open("uncompress/behavior_log.csv", 'r') as input_data:
  #with open("temp/debug_ub_global", 'r') as input_data:
    #第一行为格式说明，可以跳过
    #格式:user,time_stamp,btag,cate,brand
    line = input_data.readline()
    line = input_data.readline()
    while line:
      line = line.strip()
      fields = line.split(",")
      userid = fields[0]
      time_stamp = fields[1]
      btag = fields[2]
      cate = fields[3]
      brand = fields[4]
      tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
      date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')

      add_user_behavior_dict_1day_global(user_behavior_dict_1day_global,userid,date,btag,cate,brand)
      add_user_behavior_dict_1day_global_realtime(user_behavior_dict_1day_global_realtime,userid,date,time_stamp,btag,cate,brand)

      line = input_data.readline()
      
      count += 1
      if count%100000 == 0:
        print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), " processed ", count, " user history behaviors")
        #break

  for userid in user_behavior_dict_1day_global_realtime:
    for date in user_behavior_dict_1day_global_realtime[userid]:
      for btag in user_behavior_dict_1day_global_realtime[userid][date]:
        (user_behavior_dict_1day_global_realtime[userid][date][btag]["cate"]).sort()
        (user_behavior_dict_1day_global_realtime[userid][date][btag]["brand"]).sort()


#合并用户过去14天的行为,譬如当前日期为20170513，则将20170429-20170512共14天的行为合并记录在一起
def gen_user_behavior_last_14day_global(user_behavior_dict_1day_global, user_behavior_dict_last_14day_global):  
  count = 0
  for userid in user_behavior_dict_1day_global:
    user_behavior_dict_last_14day_global[userid] = {}
    start_date = "20170506"
    temp_user_behavior_dict_last_14day_global = {}
    for i in range(14):
      past_date = (datetime.datetime.strptime(start_date, '%Y%m%d') + datetime.timedelta(days=-(i+1))).strftime('%Y%m%d')
      if past_date in user_behavior_dict_1day_global[userid]:
        merge_dict(temp_user_behavior_dict_last_14day_global, user_behavior_dict_1day_global[userid][past_date])
    user_behavior_dict_last_14day_global[userid][start_date] = copy.deepcopy(temp_user_behavior_dict_last_14day_global)  

    for date in ["20170507", "20170508", "20170509", "20170510", "20170511", "20170512", "20170513"]:
      past_15_date = (datetime.datetime.strptime(date, '%Y%m%d') + datetime.timedelta(days=-15)).strftime('%Y%m%d')
      if past_15_date in user_behavior_dict_1day_global[userid]:
        cut_dict(temp_user_behavior_dict_last_14day_global, user_behavior_dict_1day_global[userid][past_15_date])
      past_1_date = (datetime.datetime.strptime(date, '%Y%m%d') + datetime.timedelta(days=-1)).strftime('%Y%m%d')
      if past_1_date in user_behavior_dict_1day_global[userid]:
        merge_dict(temp_user_behavior_dict_last_14day_global, user_behavior_dict_1day_global[userid][past_1_date])
      user_behavior_dict_last_14day_global[userid][date] = copy.deepcopy(temp_user_behavior_dict_last_14day_global)

    #for date in ["20170506", "20170507", "20170508", "20170509", "20170510", "20170511", "20170512", "20170513"]:
    #  for i in range(14):
    #    past_date = (datetime.datetime.strptime(date, '%Y%m%d') + datetime.timedelta(days=-(i+1))).strftime('%Y%m%d')

    #    if userid not in user_behavior_dict_last_14day_global:
    #      user_behavior_dict_last_14day_global[userid] = {}
    #    if date not in user_behavior_dict_last_14day_global[userid]:
    #      user_behavior_dict_last_14day_global[userid][date] = {}

    #    if past_date in user_behavior_dict_1day_global[userid]:
    #      merge_dict(user_behavior_dict_last_14day_global[userid][date], user_behavior_dict_1day_global[userid][past_date])

    count += 1
    if count%10000 == 0:
      print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), " gen last 14 days user behaviors for  ", count, " users")

#提取出指定用户在指定日期、指定行为类型（pv/fav/cart/buy）上过去14天的top cate或者brand
def gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, userid, date, action_type, dim, num=50):
  res = []
  if userid not in user_behavior_dict_last_14day_global:
    return res
  if date not in user_behavior_dict_last_14day_global[userid]:
    return res
  if action_type not in user_behavior_dict_last_14day_global[userid][date]:
    return res
  if dim not in user_behavior_dict_last_14day_global[userid][date][action_type]:
    return res

  user_action_dict = user_behavior_dict_last_14day_global[userid][date][action_type][dim]
  keys = sort_by_value(user_action_dict)
  if len(keys) > num:
    res = keys[:num]
  else:
    res = keys
  return res

#提取出指定用户在指定日期、指定行为类型（pv/fav/cart/buy）上最近访问过的cate或者brand
def gen_user_feature_global_realtime(user_behavior_dict_1day_global_realtime, userid, time_stamp, action_type, dim):
  latest_num_feas_dict = {}
  latest_num_feas = []
  latest_1_fea = ""

  tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
  date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')

  if userid not in user_behavior_dict_1day_global_realtime:
    return latest_num_feas, latest_1_fea
  if date not in user_behavior_dict_1day_global_realtime[userid]:
    return latest_num_feas, latest_1_fea
  if action_type not in user_behavior_dict_1day_global_realtime[userid][date]:
    return latest_num_feas, latest_1_fea
  if dim not in user_behavior_dict_1day_global_realtime[userid][date][action_type]:
    return latest_num_feas, latest_1_fea

  user_action_dict = user_behavior_dict_1day_global_realtime[userid][date][action_type][dim]
  act_num = len(user_action_dict)
  for i in range(act_num):
    user_action = user_action_dict[i]
    fields = user_action.split(":")
    action_time_stamp = fields[0]
    action_object = fields[1]
    if (action_time_stamp >= time_stamp):
      break

    latest_1_fea = action_object
    latest_num_feas_dict[action_object] = 1
    #if action_object not in latest_num_feas:
    #  latest_num_feas.append(action_object)

  latest_num_feas = list(latest_num_feas_dict.keys())

  return latest_num_feas, latest_1_fea

if __name__ == '__main__':
  user_behavior_dict_1day_global = {}
  user_behavior_dict_last_14day_global={}
  user_behavior_dict_1day_global_realtime = {}

  gen_user_behavior_1day_global(user_behavior_dict_1day_global, user_behavior_dict_1day_global_realtime)
  gen_user_behavior_last_14day_global(user_behavior_dict_1day_global, user_behavior_dict_last_14day_global)

  #print(user_behavior_dict_1day_global)
  #print(user_behavior_dict_last_14day_global)

  #1.长期行为特征
  with open("final_data/user_behavior_1day_global", 'w+') as f:
    for userid in user_behavior_dict_1day_global:
      user_behavior_1day = user_behavior_dict_1day_global[userid]
      f.write(userid)  
      f.write("\t")  
      f.write(json.dumps(user_behavior_1day))  
      f.write("\n")  

  with open("final_data/user_behavior_last_14day_global", 'w+') as f:
    for userid in user_behavior_dict_last_14day_global:
      user_behavior_last_14day_global = user_behavior_dict_last_14day_global[userid]
      f.write(userid)  
      f.write("\t")  
      f.write(json.dumps(user_behavior_last_14day_global))  
      f.write("\n")  

  with open("final_data/user_behavior_1day_global", 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      userid = fields[0]
      user_behavior_1day = json.loads(fields[1])
      print("user_behavior_1day_global")
      print(userid, user_behavior_1day)
      line = f.readline()
      break

  with open("final_data/user_behavior_last_14day_global", 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      userid = fields[0]
      user_behavior_last_14day_global = json.loads(fields[1])
      print("user_behavior_last_14day_global")
      print(userid, user_behavior_last_14day_global)
      line = f.readline()
      break

  print("test pv cate 14days\t", gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, "452547", "20170508", "pv", "cate"))
  print("test pv brand 14days\t", gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, "452547", "20170508", "pv", "brand"))

  #2.realtime行为特征
  with open("final_data/user_behavior_1day_global_realtime", 'w+') as f:
    for userid in user_behavior_dict_1day_global_realtime:
      user_behavior_1day = user_behavior_dict_1day_global_realtime[userid]
      f.write(userid)  
      f.write("\t")  
      f.write(json.dumps(user_behavior_1day))  
      f.write("\n")  

  with open("final_data/user_behavior_1day_global_realtime", 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      userid = fields[0]
      user_behavior_1day = json.loads(fields[1])
      print("user_behavior_1day_global_realtime")
      print(userid, user_behavior_1day)
      line = f.readline()
      break

  print("test pv cate realtime\t", gen_user_feature_global_realtime(user_behavior_dict_1day_global_realtime, "452547", "1494246535", "pv", "cate"))
  print("test pv brand realtime\t", gen_user_feature_global_realtime(user_behavior_dict_1day_global_realtime, "452547", "1494246535", "pv", "brand"))
  print("test cart cate realtime\t", gen_user_feature_global_realtime(user_behavior_dict_1day_global_realtime, "452547", "1494246535", "cart", "cate"))
  print("test cart brand realtime\t", gen_user_feature_global_realtime(user_behavior_dict_1day_global_realtime, "452547", "1494246535", "cart", "brand"))

