# -*- coding: UTF-8 -*-
import time
import datetime # 提供操作日期和时间的类
import numpy as np
import json
import os
from time_func import convert_time_stamp 
from user_behavior_global import gen_user_feature_last_14day_global,gen_user_feature_global_realtime 
from user_behavior_local import gen_stat_feature_last_xhours_local,gen_action_list_feature_last_xhours_local 

def load_basic_fea_dict(fea_dict_file, fea_dict, length):
  if not os.path.exists(fea_dict_file):
    print("Error:", fea_dict_file, " dose not exists!!!")
    return

  with open(fea_dict_file, 'r') as f:
    #第一行为格式说明，可以跳过
    line = f.readline()
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split(",")
      if len(fields) != length:
        print("Error:",line, "only have ", len(fields), "fields, but need ", length)
        line = f.readline()
        continue
      key = fields[0]
      value = fields[1:]
      fea_dict[key] = value
      line = f.readline()

def load_behavior_fea_dict(fea_dict_file, fea_dict):
  if not os.path.exists(fea_dict_file):
    print("Error:", fea_dict_file, " dose not exists!!!")
    return

  with open(fea_dict_file, 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      id = fields[0]
      behavior_fea = json.loads(fields[1])
      fea_dict[id] = behavior_fea

      #break

      line = f.readline()

def gen_local_stat_feas(sample, user_behavior_local_dict_realtime, ad_behavior_local_dict_realtime, userid, adgroup_id, time_stamp, hours):
  user_show, user_click, user_ctr = gen_stat_feature_last_xhours_local(user_behavior_local_dict_realtime,userid,time_stamp, hours)
  ad_show, ad_click, ad_ctr = gen_stat_feature_last_xhours_local(ad_behavior_local_dict_realtime,adgroup_id,time_stamp, hours)
  sample.append(str(user_show))
  sample.append(str(user_click))
  sample.append(str(user_ctr))
  sample.append(str(ad_show))
  sample.append(str(ad_click))
  sample.append(str(ad_ctr))


def gen_local_behavior_feas(sample, user_behavior_local_dict_realtime, ad_fea_dict, userid, time_stamp, adgroup_id, hours):
  
  non_click_aid_dict, click_aid_dict, last_click = gen_action_list_feature_last_xhours_local(user_behavior_local_dict_realtime,userid,time_stamp, hours)

  #用户喜欢点击
  click_aid_list_uniq = list(click_aid_dict.keys())
  click_cate_id_dict_uniq = {}
  click_brand_dict_uniq = {}
  click_customer_dict_uniq = {}
  last_click_cate_id = ""
  last_click_brand = ""
  last_click_customer = ""
  click_this_ad_count = 0
  #用户不喜欢点击
  #non_click_aid_list_uniq = []
  #non_click_cate_id_list_uniq = []
  #non_click_brand_list_uniq = []
  #non_click_customer_list_uniq = []
  non_click_this_ad_count = 0

  for i in range(0, len(click_aid_list_uniq)):
    aid = click_aid_list_uniq[i]

    if aid in ad_fea_dict: 
      aid_info = ad_fea_dict[aid]
      cate_id = aid_info[0]
      campaign_id = aid_info[1]
      customer = aid_info[2]
      brand = aid_info[3]
      price = aid_info[4]
    
      if cate_id not in click_cate_id_dict_uniq:
        click_cate_id_dict_uniq[cate_id] = 1
      else:
        click_cate_id_dict_uniq[cate_id] += 1
      if brand not in click_brand_dict_uniq:
        click_brand_dict_uniq[brand] = 1
      else:
        click_brand_dict_uniq[brand] += 1
      if customer not in click_customer_dict_uniq:
        click_customer_dict_uniq[customer] = 1
      else:
        click_customer_dict_uniq[customer] += 1
  
  if last_click in ad_fea_dict:
    aid_info = ad_fea_dict[last_click]
    last_click_cate_id = aid_info[0]
    last_click_customer = aid_info[2]
    last_click_brand = aid_info[3]

  if adgroup_id in click_aid_dict:
    click_this_ad_count = click_aid_dict[adgroup_id]
  if adgroup_id in non_click_aid_dict:
    non_click_this_ad_count = non_click_aid_dict[adgroup_id]

  #for i in range(0, len(non_click_aid_list)):
  #  aid = non_click_aid_list[i]
  #  if aid not in click_aid_list_uniq and aid not in non_click_aid_list_uniq:
  #    non_click_aid_list_uniq.append(aid)

  #  if aid == adgroup_id:
  #    non_click_this_ad_count += 1

  #  if aid in ad_fea_dict: 
  #    aid_info = ad_fea_dict[aid]
  #    cate_id = aid_info[0]
  #    campaign_id = aid_info[1]
  #    customer = aid_info[2]
  #    brand = aid_info[3]
  #    price = aid_info[4]
  # 
  #    if cate_id not in click_cate_id_list_uniq and cate_id not in non_click_cate_id_list_uniq:
  #      non_click_cate_id_list_uniq.append(cate_id)

  #    if brand not in click_brand_list_uniq and brand not in non_click_brand_list_uniq:
  #      non_click_brand_list_uniq.append(brand)

  #    if customer not in click_customer_list_uniq and customer not in non_click_customer_list_uniq:
  #      non_click_customer_list_uniq.append(customer)

  sample.append(",".join(click_aid_list_uniq))
  sample.append(",".join(list(click_cate_id_dict_uniq.keys())))
  sample.append(",".join(list(click_brand_dict_uniq.keys())))
  sample.append(",".join(list(click_customer_dict_uniq.keys())))
  sample.append(last_click)
  sample.append(last_click_cate_id)
  sample.append(last_click_brand)
  sample.append(last_click_customer)
  #sample.append(",".join(non_click_aid_list_uniq))
  #sample.append(",".join(non_click_cate_id_list_uniq))
  #sample.append(",".join(non_click_brand_list_uniq))
  #sample.append(",".join(non_click_customer_list_uniq))
  sample.append(str(click_this_ad_count))
  sample.append(str(non_click_this_ad_count))

if __name__ == '__main__':
  ad_fea_dict = {}
  user_fea_dict = {}
  user_behavior_dict_last_14day_global={}
  user_behavior_1day_global_realtime={}
  user_behavior_local_dict_realtime = {}
  ad_behavior_local_dict_realtime = {}

  #ad feature 格式:adgroup_id,cate_id,campaign_id,customer,brand,price
  print("Load ad feature")
  load_basic_fea_dict("uncompress/ad_feature.csv", ad_fea_dict, 6);

  #user feature 格式:userid,cms_segid,cms_group_id,final_gender_code,age_level,pvalue_level,shopping_level,occupation,new_user_class_level
  print("Load user feature")
  load_basic_fea_dict("uncompress/user_profile.csv", user_fea_dict, 9);

  print("Load user global behavior feature")
  load_behavior_fea_dict("final_data/user_behavior_1day_global_realtime", user_behavior_1day_global_realtime)
  print(gen_user_feature_global_realtime(user_behavior_1day_global_realtime, "305254", "1494160210", "pv", "cate"))
  print(gen_user_feature_global_realtime(user_behavior_1day_global_realtime, "305254", "1494155252", "pv", "brand"))
  print(gen_user_feature_global_realtime(user_behavior_1day_global_realtime, "305254", "1494312046", "buy", "cate"))
  print(gen_user_feature_global_realtime(user_behavior_1day_global_realtime, "305254", "1494312047", "buy", "brand"))

  load_behavior_fea_dict("final_data/user_behavior_last_14day_global", user_behavior_dict_last_14day_global)
  #check if user_behavior_dict_last_14day_global is ok
  print(gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, "558157", "20170513", "pv", "cate"))
  print(gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, "558157", "20170513", "pv", "brand"))

  print("Load user/ad local behavior feature")
  load_behavior_fea_dict("final_data/user_behavior_local_dict_realtime", user_behavior_local_dict_realtime)
  print(gen_stat_feature_last_xhours_local(user_behavior_local_dict_realtime, "581738", "1494137644", 24))
  print(gen_action_list_feature_last_xhours_local(user_behavior_local_dict_realtime, "581738", "1494137644", 24))
  load_behavior_fea_dict("final_data/ad_behavior_local_dict_realtime", ad_behavior_local_dict_realtime)
  print(gen_stat_feature_last_xhours_local(ad_behavior_local_dict_realtime, "1", "1494137645", 24))

  #raw_sample 格式:user,time_stamp,adgroup_id,pid,nonclk,clk
  with open("final_data/raw_sample_shuffle.csv", 'r') as input_data, \
       open("final_data/train_data", 'w+') as train_data, \
       open("final_data/test_data", 'w+') as test_data:
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
      tm_hour = str(tm_hour)
      
      if date not in ["20170506", "20170507", "20170508", "20170509", "20170510", "20170511", "20170512", "20170513"]:
        continue

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

      #4.global 历史行为特征
      for action_type in ["pv", "fav", "cart", "buy"]:
        for dim in ["cate", "brand"]:
          user_feature_last_14day = gen_user_feature_last_14day_global(user_behavior_dict_last_14day_global, userid, date, action_type, dim)
          sample.append(",".join(user_feature_last_14day))

      #5.global 实时特征
      for action_type in ["pv", "fav", "cart", "buy"]:
        for dim in ["cate", "brand"]:
          latest_num_feas, latest_1_fea = gen_user_feature_global_realtime(user_behavior_1day_global_realtime, userid, time_stamp, action_type, dim)
          sample.append(",".join(latest_num_feas))
          sample.append(latest_1_fea)

      #6.local 统计特征
      gen_local_stat_feas(sample, user_behavior_local_dict_realtime, ad_behavior_local_dict_realtime, userid, adgroup_id, time_stamp, hours=24)
      gen_local_stat_feas(sample, user_behavior_local_dict_realtime, ad_behavior_local_dict_realtime, userid, adgroup_id, time_stamp, hours=168)

      #7.local实时行为特征 
      gen_local_behavior_feas(sample, user_behavior_local_dict_realtime, ad_fea_dict, userid, time_stamp, adgroup_id, hours=24)
      gen_local_behavior_feas(sample, user_behavior_local_dict_realtime, ad_fea_dict, userid, time_stamp, adgroup_id, hours=168)

      #样本取自5月6号-13号，将13号的数据作为测试集，其他数据作为训练集
      if tm_mday != 13:
        train_data.write("\t".join(sample))
        train_data.write("\n")
      else:
        test_data.write("\t".join(sample))
        test_data.write("\n")

      line = input_data.readline()

      count += 1
      if count % 10000 == 0:
        print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), " processed ", count, " samples")
