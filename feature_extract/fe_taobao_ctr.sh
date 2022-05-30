version=$1
min_fea_freq=$2

g++ -std=c++11 -lstdc++ -lm feature_extract.cpp -o feature_extract 2>&1|tee log
g++ -std=c++11 -lstdc++ -lm -shared -o libfe.so -fPIC feature_extract.cpp

mkdir data/
echo "test demo"
head -100 ../sample/final_data/train_data > data/demo
echo "demo fea stage1"
./feature_extract 1 $version
if [ $? -eq 0 ];then
    echo "demo fea stage1 successfully"
else
    "demo fea stage1 failed"
    exit
fi

echo "demo gen sparse dict"
python3 gen_fea_dict.py
if [ $? -eq 0 ];then
    echo "demo gen sparse dict successfully"
else
    "demo gen sparse dict failed"
    exit
fi

echo "demo fea stage2"
./feature_extract 2 $version
if [ $? -eq 0 ];then
    echo "demo fea stage2 successfully"
else
    "demo fea stage2 failed"
    exit
fi


echo "Feature extract for "$version

python3 feature_extract.py $version $min_fea_freq
if [ $? -eq 0 ];then
    echo "feature_extract successfully"
else
    "feature_extract failed"
    exit
fi

