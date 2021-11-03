# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类
import numpy as np
from time_func import convert_time_stamp 
import json

#从已经按照时间排好序的用户行为序列中找到time_stamp之前的累计曝光/点击次数
def action_stat_from_seqs(user_action_dict, time_stamp):
  act_num = len(user_action_dict)
  shows = 0
  clks = 0
  for i in range(act_num):
    user_action = user_action_dict[i]
    fields = user_action.split(":")
    action_time_stamp = fields[0]
    object_id = fields[1]
    clk = int(fields[2])
    if (action_time_stamp >= time_stamp):
      break
    else:
      shows += 1
      clks += clk
  return shows, clks

#从已经按照时间排好序的用户行为序列中找到time_stamp之前的no点击、点击list
def gen_action_list_from_seqs(user_action_dict, time_stamp, non_click_dict, click_dict):
  act_num = len(user_action_dict)
  last_click = ""
  for i in range(act_num):
    user_action = user_action_dict[i]
    fields = user_action.split(":")
    action_time_stamp = fields[0]
    object_id = fields[1]
    clk = int(fields[2])
    if (action_time_stamp >= time_stamp):
      break
    else:
      #if (clk == 1) and object_id not in click_dict:
      #  click_dict.append(object_id)
      #if (clk == 0) and object_id not in non_click_dict:
      #  non_click_dict.append(object_id)

      last_click = object_id

      #  不进行去重
      if (clk == 1):
        if object_id not in click_dict:
          click_dict[object_id]=1
        else:
          click_dict[object_id]+=1
      else:
        if object_id not in non_click_dict:
          non_click_dict[object_id]=1
        else:
          non_click_dict[object_id]+=1

  return last_click

#将用户/广告的行为按照userid*日期*小时*时间戳*点击标记，存储起来
def add_behavior_local_dict_realtime(behavior_local_dict_realtime,id,object_id,date,tm_hour,time_stamp,clk):

  if id not in behavior_local_dict_realtime:
    behavior_local_dict_realtime[id] = {}

  if date not in behavior_local_dict_realtime[id]:
    behavior_local_dict_realtime[id][date] = {}

  if tm_hour not in behavior_local_dict_realtime[id][date]:
    behavior_local_dict_realtime[id][date][tm_hour] = {}
    behavior_local_dict_realtime[id][date][tm_hour]["show"] = 0
    behavior_local_dict_realtime[id][date][tm_hour]["click"] = 0
    behavior_local_dict_realtime[id][date][tm_hour]["behavior_list"] = []

  behavior_local_dict_realtime[id][date][tm_hour]["show"] += 1
  behavior_local_dict_realtime[id][date][tm_hour]["click"] += int(clk)
  behavior = time_stamp + ":" + object_id + ":" + clk
  behavior_local_dict_realtime[id][date][tm_hour]["behavior_list"].append(behavior)

def gen_local_behavior_dict_realtime(user_behavior_local_dict_realtime, ad_behavior_local_dict_realtime):
  count = 0
  with open("uncompress/raw_sample.csv", 'r') as input_data:
    #第一行为格式说明，可以跳过
    #格式:user,time_stamp,adgroup_id,pid,nonclk,clk
    line = input_data.readline()
    line = input_data.readline()
    while line:
      line = line.strip()
      fields = line.split(",")
      userid = fields[0]
      time_stamp = fields[1]
      adgroup_id = fields[2]
      pid = fields[3]
      nonclk = fields[4]
      clk = fields[5]

      tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
      date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')
      tm_hour = str(tm_hour)

      add_behavior_local_dict_realtime(user_behavior_local_dict_realtime,userid,adgroup_id,date,tm_hour,time_stamp,clk)
      add_behavior_local_dict_realtime(ad_behavior_local_dict_realtime,adgroup_id,userid,date,tm_hour,time_stamp,clk)

      line = input_data.readline()
      
      count += 1
      if count%100000 == 0:
        print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), " processed ", count, " user history behaviors")
        #break

  for userid in user_behavior_local_dict_realtime:
    for date in user_behavior_local_dict_realtime[userid]:
      for tm_hour in user_behavior_local_dict_realtime[userid][date]:
        (user_behavior_local_dict_realtime[userid][date][tm_hour]["behavior_list"]).sort()

  for adgroup_id in ad_behavior_local_dict_realtime:
    for date in ad_behavior_local_dict_realtime[adgroup_id]:
      for tm_hour in ad_behavior_local_dict_realtime[adgroup_id][date]:
        (ad_behavior_local_dict_realtime[adgroup_id][date][tm_hour]["behavior_list"]).sort()

#提取出指定用户或者广告在过去x个小时的show、click、ctr
def gen_stat_feature_last_xhours_local(behavior_local_dict_realtime, id, time_stamp, hours):
  total_show = 0
  total_clk = 0

  tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
  date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')
  tm_hour = str(tm_hour)
  #print("behavior_local_dict_realtime:", behavior_local_dict_realtime)
  #print("id:",id)
  #print("date:",date)
  #print("tm_hour:",tm_hour)
  #print(behavior_local_dict_realtime[id])
  #print(behavior_local_dict_realtime[id][date])
  #print(behavior_local_dict_realtime[id][date][tm_hour])
  for i in range(hours-1,0,-1):
    new_time_stamp = int(time_stamp) - i*3600
    new_tm_year, new_tm_mon, new_tm_mday, new_workdayflag, new_tm_hour = convert_time_stamp(str(new_time_stamp))
    new_date = (datetime.datetime(new_tm_year, new_tm_mon, new_tm_mday)).strftime('%Y%m%d')
    new_tm_hour = str(new_tm_hour)

    if id in behavior_local_dict_realtime and new_date in behavior_local_dict_realtime[id] and new_tm_hour in behavior_local_dict_realtime[id][new_date]:
      total_show +=  behavior_local_dict_realtime[id][new_date][new_tm_hour]['show']
      total_clk +=  behavior_local_dict_realtime[id][new_date][new_tm_hour]['click']
      #print("other hour")
      #print(id, new_date, new_tm_hour, total_show,  total_clk)

  if id in behavior_local_dict_realtime and date in behavior_local_dict_realtime[id] and tm_hour in behavior_local_dict_realtime[id][date]:
    #print("this hour")
    show_this_hour, clk_this_hour = action_stat_from_seqs(behavior_local_dict_realtime[id][date][tm_hour]["behavior_list"], time_stamp)
    #print(show_this_hour, clk_this_hour)
    total_show += show_this_hour
    total_clk += total_clk


  return total_show, total_clk, total_clk*1.0/(total_show+10)

#提取出指定用户或者广告在过去x个小时访问list,按照访问时间顺序升序排列
def gen_action_list_feature_last_xhours_local(behavior_local_dict_realtime, id, time_stamp, hours):

  non_click_dict = {}
  click_dict = {}
  last_click = ""

  tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
  date = (datetime.datetime(tm_year, tm_mon, tm_mday)).strftime('%Y%m%d')
  tm_hour = str(tm_hour)
  #print("behavior_local_dict_realtime:", behavior_local_dict_realtime)
  #print("id:",id)
  #print("date:",date)
  #print("tm_hour:",tm_hour)
  #print(behavior_local_dict_realtime[id])
  #print(behavior_local_dict_realtime[id][date])
  #print(behavior_local_dict_realtime[id][date][tm_hour])

  for i in range(hours-1,0,-1):
    new_time_stamp = int(time_stamp) - i*3600
    new_tm_year, new_tm_mon, new_tm_mday, new_workdayflag, new_tm_hour = convert_time_stamp(str(new_time_stamp))
    new_date = (datetime.datetime(new_tm_year, new_tm_mon, new_tm_mday)).strftime('%Y%m%d')
    new_tm_hour = str(new_tm_hour)

    if id in behavior_local_dict_realtime and new_date in behavior_local_dict_realtime[id] and new_tm_hour in behavior_local_dict_realtime[id][new_date]:
      user_action_dict = behavior_local_dict_realtime[id][new_date][new_tm_hour]["behavior_list"]
      last_click = gen_action_list_from_seqs(user_action_dict, time_stamp, non_click_dict, click_dict)
      #print("other hour")
      #print(id, new_date, new_tm_hour, non_click_dict, click_dict)

  if id in behavior_local_dict_realtime and date in behavior_local_dict_realtime[id] and tm_hour in behavior_local_dict_realtime[id][date]:
    user_action_dict = behavior_local_dict_realtime[id][date][tm_hour]["behavior_list"]
    last_click = gen_action_list_from_seqs(user_action_dict, time_stamp, non_click_dict, click_dict)
    #print("this hour")
    #print(id, date, tm_hour, non_click_dict, click_dict)
  
  return non_click_dict, click_dict, last_click

if __name__ == '__main__':
  user_behavior_local_dict_realtime = {}
  ad_behavior_local_dict_realtime = {}

  gen_local_behavior_dict_realtime(user_behavior_local_dict_realtime, ad_behavior_local_dict_realtime)

  #print(user_behavior_local_dict_realtime)
  #print(user_behavior_dict_last_14day)

  #用户在预测广告位上的中间统计数据
  with open("final_data/user_behavior_local_dict_realtime", 'w+') as f:
    for userid in user_behavior_local_dict_realtime:
      user_behavior_1day = user_behavior_local_dict_realtime[userid]
      f.write(userid)  
      f.write("\t")  
      f.write(json.dumps(user_behavior_1day))  
      f.write("\n")  

  with open("final_data/ad_behavior_local_dict_realtime", 'w+') as f:
    for adgroup_id in ad_behavior_local_dict_realtime:
      ad_behavior_1day = ad_behavior_local_dict_realtime[adgroup_id]
      f.write(adgroup_id)  
      f.write("\t")  
      f.write(json.dumps(ad_behavior_1day))  
      f.write("\n")  

  with open("final_data/user_behavior_local_dict_realtime", 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      userid = fields[0]
      user_behavior_1day = json.loads(fields[1])
      print(userid, user_behavior_1day)
      line = f.readline()
      break

  with open("final_data/ad_behavior_local_dict_realtime", 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      adgroup_id = fields[0]
      ad_behavior_1day = json.loads(fields[1])
      print(adgroup_id, ad_behavior_1day)
      line = f.readline()
      break

  print(gen_stat_feature_last_xhours_local(user_behavior_local_dict_realtime, "581738", "1494137644", 24))
  print(gen_action_list_feature_last_xhours_local(user_behavior_local_dict_realtime, "581738", "1494137644", 24))
  print(gen_stat_feature_last_xhours_local(ad_behavior_local_dict_realtime, "1", "1494137645", 24))
