# -*- coding: utf-8 -*-
import ctypes
import chardet
import sys
from gen_fea_dict import gen_fea_dict 

#将python类型转换成c类型，支持int, float,string的变量和数组的转换
def convert_type(input_data):
    ctypes_map = {int:ctypes.c_int,
                 float:ctypes.c_double,
                 str:ctypes.c_char_p
    }
    input_data_type = type(input_data)
    #print(input_data, input_data_type)
    if input_data_type is list:
        length = len(input_data)
        if length==0:
            print("convert type failed...input_data is "+input_data)
            return null
        else:
            arr = (ctypes_map[type(input_data[0])] * length)()
            for i in range(length):
                arr[i] = input_data[i]
            return arr
    else:
        if input_data_type in ctypes_map:
            if input_data_type is str:
                input_data_bytes = (input_data)
                return ctypes.c_char_p(input_data_bytes)
            else:
                return ctypes_map[input_data_type](input_data)
        else:
            print("convert type failed...input_data is "+input_data)
            return null

def feature_extract_stage1(raw_sample_file, intermediate_sample_file):
    libfe = ctypes.cdll.LoadLibrary('./libfe.so')
    length = 81920
    buf = ctypes.create_string_buffer(length)
    print(ctypes.sizeof(buf))
    buf.raw = b'ps-dnn'
    c_buflen = ctypes.c_int(length)

    print("feature_extract_stage1")
    print("raw_sample_file: ", raw_sample_file)
    print("intermediate_sample_file: ", intermediate_sample_file)

    count = 0;
    with open(raw_sample_file, "r") as f, open(intermediate_sample_file, "w") as g:
      raw_sample = f.readline()
      while raw_sample:
        raw_sample = raw_sample.strip("\n")
        raw_sample = raw_sample.encode("utf-8")
        
        libfe.fea_extract_stage1(ctypes.c_char_p(fea_conf_file),\
            ctypes.c_char_p(raw_sample), ctypes.byref(buf), ctypes.byref(c_buflen))
        if c_buflen.value > 0:
            g.write(ctypes.string_at(buf,c_buflen.value).decode())
            g.write("\n")

        raw_sample = f.readline()
        count += 1 
        if count%100000 == 0:
          print("feature_extract_stage1: ", count, " samples processed")

def feature_extract_stage2(intermediate_sample_file, sparse_dict_index_file, final_sample_file):
    libfe = ctypes.cdll.LoadLibrary('./libfe.so')
    length = 81920
    buf = ctypes.create_string_buffer(length)
    print(ctypes.sizeof(buf))
    buf.raw = b'ps-dnn'
    c_buflen = ctypes.c_int(length)

    print("feature_extract_stage2")
    print("intermediate_sample_file: ", intermediate_sample_file)
    print("sparse_dict_index_file: ", sparse_dict_index_file)
    print("final_sample_file: ", final_sample_file)

    count = 0;
    with open(intermediate_sample_file, "r") as f, open(final_sample_file, "w") as g:
      intermediate_sample = f.readline()
      while intermediate_sample:
        intermediate_sample = intermediate_sample.strip("\n")
        intermediate_sample = intermediate_sample.encode("utf-8")
        
        libfe.fea_extract_stage2(ctypes.c_char_p(fea_conf_file),ctypes.c_char_p(sparse_dict_index_file),\
            ctypes.c_char_p(intermediate_sample), ctypes.byref(buf), ctypes.byref(c_buflen))
        if c_buflen.value > 0:
            g.write(ctypes.string_at(buf,c_buflen.value).decode())
            g.write("\n")

        intermediate_sample = f.readline()
        count += 1 
        if count%100000 == 0:
          print("feature_extract_stage2: ", count, " samples processed")

if __name__ == '__main__':
    version = sys.argv[1]
    min_fea_freq = int(sys.argv[2])
    raw_sample_train_file = "../sample/final_data/train_data"
    raw_sample_test_file = "../sample/final_data/test_data"
    intermediate_sample_train_file = "./data/intermediate_sample_train_" + version
    intermediate_sample_test_file = "./data/intermediate_sample_test_"  + version
    final_sample_train_file = "./data/final_sample_train_" + version
    final_sample_test_file = "./data/final_sample_test_" + version

    raw_sparse_dict_file = ("./data/raw_sparse_fea_dict_" + version).encode('utf-8')
    sparse_dict_index_file = ("./data/sparse_fea_index_dict_" + version).encode('utf-8')
    fea_conf_file = ("../conf/features_" + version + ".ini").encode('utf-8')

    # 1. Gen intermediate_sample for train&test
    feature_extract_stage1(raw_sample_train_file, intermediate_sample_train_file)
    feature_extract_stage1(raw_sample_test_file, intermediate_sample_test_file)

    # 2. Gen sparse fea dict with train intermediate_sample
    gen_fea_dict(intermediate_sample_train_file, raw_sparse_dict_file, sparse_dict_index_file, min_fea_freq)

    # 3. Gen final_sample for train&test
    feature_extract_stage2(intermediate_sample_train_file, sparse_dict_index_file, final_sample_train_file)
    feature_extract_stage2(intermediate_sample_test_file, sparse_dict_index_file, final_sample_test_file)

