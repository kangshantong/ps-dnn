version=$1
min_fea_freq=$2

g++ -std=c++11 -lstdc++ -lm feature_extract.cpp -o feature_extract 2>&1|tee log
g++ -std=c++11 -lstdc++ -lm -shared -o libfe.so -fPIC feature_extract.cpp

mkdir data/
echo "test demo"
head -100 ../sample/final_data/train_data > data/demo
echo "demo fea stage1"
./feature_extract 1 $version
echo "demo gen sparse dict"
python3 gen_fea_dict.py
echo "demo fea stage2"
./feature_extract 2 $version

echo "Feature extract for "$version

python3 feature_extract.py $version $min_fea_freq
