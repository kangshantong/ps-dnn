# -*- coding: UTF-8 -*-
import numpy as np

#将dict按照value降序排列，返回key的list
def sort_by_value(d): 
  items=d.items() 
  backitems=[[v[1],v[0]] for v in items] 
  backitems.sort(reverse=True) 
  return [ backitems[i][1] for i in range(0,len(backitems))] 

#merge_dict将dict_b merge到dict a上
def gen_raw_fea_dict(intermediate_sample_file, fea_dict):
  with open(intermediate_sample_file, 'r') as f:
    line = f.readline()
    while line:
      line = line.strip()
      fields = line.split("\t")
      sampleid=fields[0]
      label=fields[1]
      dense_features_user=fields[2]
      dense_features_ad=fields[3]
      dense_features_user_ad=fields[4]
      sparse_features_user=fields[5].split(",") 
      sparse_features_ad=fields[6].split(",") 
      sparse_features_user_ad=fields[7].split(",") 

      for sparse_features in [sparse_features_user, sparse_features_ad, sparse_features_user_ad]:
        for sparse_feature in sparse_features:
          fea_values = sparse_feature.split("|")
          for fea_value in fea_values:
            if fea_value not in fea_dict:
              fea_dict[fea_value] = 1
            else:
              fea_dict[fea_value] += 1
        
      line = f.readline()
            
def save_dict(out_file, data_dict):
  with open(out_file, 'w+') as f:
    for key in data_dict:
      value = str(data_dict[key])
      f.write(key + "\t" + value)
      f.write("\n")

def gen_fea_dict(intermediate_sample_file, raw_sparse_dict_file, sparse_dict_index_file, min_fea_freq=1):
  raw_fea_dict = {}
  fea_dict_index = {}

  print("Gen raw fea dict")
  gen_raw_fea_dict(intermediate_sample_file, raw_fea_dict)
  print("Raw dict counts: ", len(raw_fea_dict))
  
  print("Sort fea dict by counts")
  fea_list_sorted = sort_by_value(raw_fea_dict) 

  print("Gen fea dict index")
  ##index 0有特殊用途，稀疏特征的特征值编号从1开始
  index = 1
  for fea in fea_list_sorted:
    counts = raw_fea_dict[fea]
    #print(fea, counts, index)
    if counts <= min_fea_freq:
      continue
    fea_dict_index[fea] = index
    index += 1
  print("fea dict index counts: ", len(fea_dict_index))

  print("Save raw fea dict")
  save_dict(raw_sparse_dict_file, raw_fea_dict)
  print("Save fea dict index")
  save_dict(sparse_dict_index_file, fea_dict_index)

if __name__ == '__main__':
  intermediate_sample_file = "./data/demo_out_stage1"
  raw_sparse_dict_file = "./data/raw_fea_dict"
  sparse_dict_index_file = "./data/fea_dict_index"
  min_fea_freq = 1

  gen_fea_dict(intermediate_sample_file, raw_sparse_dict_file, sparse_dict_index_file, min_fea_freq)
