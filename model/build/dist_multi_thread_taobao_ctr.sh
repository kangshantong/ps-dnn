#!/bin/bash
# set -x
make -j10
version=$1
model=$2
export PS_VERBOSE=1
export DMLC_NUM_SERVER=8
export DMLC_NUM_WORKER=16
bin=./dist_multi_thread
net_conf=../../conf/${model}.ini
fea_conf=../../conf/features_${version}.ini
train_file=../../feature_extract/data/final_sample_train_${version}
test_file=../../feature_extract/data/final_sample_test_${version}
param2key=../checkpoints/taobao_ctr_param2key_${version}
key2value=../checkpoints/taobao_ctr_key2value_${version}
now=$(date "+%Y-%m-%d-%H-%M-%S")

ps -ef | grep ${bin} | grep -v grep | awk '{print $2}' | xargs kill

# start the scheduler
export DMLC_PS_ROOT_URI='127.0.0.1'
export DMLC_PS_ROOT_PORT='8902'
export DMLC_ROLE='scheduler'
echo "start scheduler"
${bin} scheduler ${net_conf} ${fea_conf} 2>&1|tee logs/${model}_${version}_scheduler.log &
sleep 0.5s

echo "num_servers: ${DMLC_NUM_SERVER}"
echo "num_workers ${DMLC_NUM_WORKER}"


# start servers
export DMLC_ROLE='server'
for ((i=0; i<${DMLC_NUM_SERVER}; ++i)); do
    export HEAPPROFILE=./S${i}
    echo "start server $i"
    ${bin} server ${net_conf} ${fea_conf} $i ${key2value} 2>&1|tee logs/${model}_${version}_server_$i.log &
    sleep 0.5s
done

#split train datas
train_dir=./data/${version}_${DMLC_NUM_WORKER}_train
if [ ! -d $train_dir ]; then
mkdir $train_dir
cd $train_dir
split -n $DMLC_NUM_WORKER ../../$train_file -d "" 
cd -
fi

#split test datas
test_dir=./data/${version}_${DMLC_NUM_WORKER}_test
if [ ! -d $test_dir ]; then
mkdir $test_dir
cd $test_dir
split -n $DMLC_NUM_WORKER ../../$test_file -d "" 
cd -
fi
export DMLC_ROLE='worker'
export HEAPPROFILE=./W${DMLC_NUM_WORKER}
#time CPUPROFILE=tmp/prof_dist.out ${bin} worker ${net_conf} ${fea_conf} ${train_dir} ${test_dir} $DMLC_NUM_WORKER ${param2key} 2>&1|tee logs/${model}_${version}_${DMLC_NUM_SERVER}_${DMLC_NUM_WORKER}.log &
time ${bin} worker ${net_conf} ${fea_conf} ${train_dir} ${test_dir} $DMLC_NUM_WORKER ${param2key} 2>&1|tee logs/${model}_${version}_${DMLC_NUM_SERVER}_${DMLC_NUM_WORKER}.log &

wait

grep -r "test auc" logs/${model}_${version}_${DMLC_NUM_SERVER}_${DMLC_NUM_WORKER}.log|awk '{print $3;total_auc+=$3;count+=1}END{print "avg test auc:", total_auc/count}'
