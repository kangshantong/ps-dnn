#.1. Download taobao ad ctr dataset from https://tianchi.aliyun.com/dataset/dataDetail?dataId=56 to ./orig/
# 2. uncompress datasets
cd uncompress/
tar xvf ../orig/behavior_log.csv.tar.gz 
tar xvf ../orig/raw_sample.csv.tar.gz 
tar xvf ../orig/ad_feature.csv.tar.gz 
tar xvf ../orig/user_profile.csv.tar.gz
cd -
# 3. shuffle raw_sample
shuf uncompress/raw_sample.csv -o final_data/raw_sample_shuffle.csv 
# 4.gen global user behavior feature from behavior_log.csv
# Note: This step needs main memorys more than 64GB, you can skip it. 
# python3 user_behavior_global.py 2>&1|tee ub.log
# 5.gen local user behavior feature from raw_sample.csv
python3 user_behavior_local.py 2>&1|tee rl.log
# 6.join ad features & user profile & global user feature & local user feature with raw_sample_shuffle
python3 gen_sample.py 2>&1|tee log
