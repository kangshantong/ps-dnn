version=$1
model=$2
#make -j 10
#time CPUPROFILE=tmp/prof_local.out ./local ../../conf/${model}.ini ../../conf/features_${version}.ini ../../feature_extract/data/final_sample_train_${version} ../../feature_extract/data/final_sample_test_${version} ../checkpoints/taobao_ctr_model_${version} 2>&1 |tee logs/local_${version}_${model}.log
time ./local ../../conf/${model}.ini ../../conf/features_${version}.ini ../../feature_extract/data/final_sample_train_${version} ../../feature_extract/data/final_sample_test_${version} ../checkpoints/taobao_ctr_model_${version} 2>&1 |tee logs/local_${version}_${model}.log
