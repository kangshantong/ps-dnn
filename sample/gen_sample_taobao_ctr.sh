#.1. Download taobao ad ctr dataset from https://tianchi.aliyun.com/dataset/dataDetail?dataId=56 to ./orig/

# 2. uncompress datasets

if [ ! -f "./uncompress/behavior_log.csv" ]; then
    echo "解压缩原始样本"
    cd uncompress/
    tar xvf ../orig/behavior_log.csv.tar.gz 
    tar xvf ../orig/raw_sample.csv.tar.gz 
    tar xvf ../orig/ad_feature.csv.tar.gz 
    tar xvf ../orig/user_profile.csv.tar.gz
    cd -
else
    echo ""
fi

# 3.gen global user behavior feature from behavior_log.csv
# Note: This step needs main memorys more than 64GB, you can skip it.
echo "gen global user behavior feature from behavior_log.csv"
# python3 user_behavior_global.py

# 4.gen local user behavior feature from raw_sample.csv
echo "gen local user behavior feature from raw_sample.csv"
#python3 user_behavior_local.py

# 5.join ad features & user profile & global user feature & local user feature with raw_sample
echo "join ad features & user profile & global user feature & local user feature with raw_sample"
python3 gen_sample.py
if [$? -eq 0];then
    echo "gen sample successfully"
else
    "gen sample failed"
    exit
fi

# 6.shuffle train/test data
echo "shuffle train/test data"
shuf final_data/train_data -o final_data/train_data_shuf
shuf final_data/test_data -o final_data/test_data_shuf

