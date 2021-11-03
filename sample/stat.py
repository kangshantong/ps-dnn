# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类
import numpy as np
import json
from time_func import convert_time_stamp 

def load_basic_fea_dict(fea_dict_file, fea_dict, length):
  with open(fea_dict_file, 'r') as f:
    #第一行为格式说明，可以跳过
    line = f.readline()
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split(",")
      if len(fields) != length:
        print("Error:",line, "only have ", len(fields), "fields, but need ", length)
        continue
      key = fields[0]
      value = fields[1:]
      fea_dict[key] = value
      line = f.readline()

def load_behavior_fea_dict(fea_dict_file, fea_dict):
  with open(fea_dict_file, 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      userid = fields[0]
      behavior_fea = json.loads(fields[1])
      fea_dict[userid] = behavior_fea

      line = f.readline()

if __name__ == '__main__':
  ad_fea_dict = {}
  user_fea_dict = {}

  #ad feature 格式:adgroup_id,cate_id,campaign_id,customer,brand,price
  print("Load ad feature")
  load_basic_fea_dict("uncompress/ad_feature.csv", ad_fea_dict, 6);
  #user feature 格式:userid,cms_segid,cms_group_id,final_gender_code,age_level,pvalue_level,shopping_level,occupation,new_user_class_level
  print("Load user feature")
  load_basic_fea_dict("uncompress/user_profile.csv", user_fea_dict, 9);

  #raw_sample 格式:user,time_stamp,adgroup_id,pid,nonclk,clk
  with open("final_data/raw_sample_shuffle.csv", 'r') as input_data, \
       open("temp/raw_sample_with_basic_fea", 'w+') as output_data:
    #第一行为格式说明，可以跳过
    line = input_data.readline()
    line = input_data.readline()
    count = 0
    while line:
      line = line.strip()
      fields = line.split(",")
      if len(fields) != 6:
        print("Error:",line, "only have ", len(fields), "fields, but need ", 6)
        continue

      userid = fields[0]
      time_stamp = fields[1]
      adgroup_id = fields[2]
      tm_year, tm_mon, tm_mday, workdayflag, tm_hour = convert_time_stamp(time_stamp)
      date = datetime.datetime(tm_year, tm_mon, tm_mday).strftime('%Y%m%d')

      #准备样本
      #样本格式为:user,time_stamp,adgroup_id,pid,nonclk,clk,workdayflag,tm_hour,\
      #    cms_segid,cms_group_id,final_gender_code,age_level,pvalue_level,shopping_level,occupation,new_user_class_level,\
      #    cate_id,campaign_id,customer,brand,price, \
      #    pv_cate_last_14days,pv_brand_last_14days,fav_cate_last_14days,fav_brand_last_14days,\
      #    cart_cate_last_14days,cart_brand_last_14days,buy_cate_last_14days,buy_brand_last_14days
      # 分隔符为"\t"

      #0.基本字段 
      sample = fields

      #1.workdayflag和tm_hour
      sample.append(str(date))
      sample.append(str(workdayflag))
      sample.append(str(tm_hour))

      #2.用户特征
      if userid in user_fea_dict:
        sample.extend(user_fea_dict[userid])
      else:
        sample.extend(['' for i in range(8)])
      
      #3.广告特征
      if adgroup_id in ad_fea_dict:
        sample.extend(ad_fea_dict[adgroup_id])
      else:
        sample.extend(['' for i in range(5)])

      output_data.write("\t".join(sample))
      output_data.write("\n")

      line = input_data.readline()

      count += 1
      if count % 10000 == 0:
        print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), " processed ", count, " samples")
