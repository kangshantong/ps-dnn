#This is a demo for how to train a dnn model.
#The dataset is taobao ad ctr dataset.

#prepare
#.0. Download taqbao ad ctr dataset from https://tianchi.aliyun.com/dataset/dataDetail?dataId=56 to ./sample/orig/
echo "!!!Please get dataset from  https://tianchi.aliyun.com/dataset/dataDetail?dataId=56 to ./sample/orig first"

# 1. gen sample
cd sample
#sh gen_sample_taobao_ctr.sh
cd ../

# 2. feature extract
version=v16
model=fnn
fea_min_freq=1

cd feature_extract
#sh fe_tao_ctr.sh $version $fea_min_freq
cd ../

# 3. Compile the model training framework
cd model/third_party
eigen_dir="./eigen"
pslite_dir="./ps-lite"

if [ ! -d "$eigen_dir" ]; then
	#wget https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
	cp ../deps/eigen-3.4.0.tar.gz .
	tar xvf eigen-3.4.0.tar.gz
	mv eigen-3.4.0 $eigen_dir
fi

if [ ! -d "$pslite_dir" ]; then
	#get ps lite code from https://github.com/dmlc/ps-lite
	#NOTE::The file ps-lite/make/deps.mk is modified, we get cityhash-1.1.1.tar.gz、lz4-r129.tar.gz、protobuf-cpp-3.5.1.tar.gz、zeromq-4.1.4.tar.gz from model/third_party/deps
	cd $pslite_dir
	make 
fi

cd ../../
cd model/build
rm -fr CMake*
cmake ../
make -j 10

# 4. Train the taobao ctr model
sh local_taobao_ctr.sh $version $model
sh dist_multi_process_taobao_ctr.sh $version $model 
