#echo "Gen sample"
#python3 gen_sample.py

echo "大小判断"
../build/local ./conf/bigger_than_500_network.ini ./conf/bigger_than_500_features.ini ./data/test_x_is_bigger_than_500 ./data/test_x_is_bigger_than_500 1001 ./checkpoints/x_is_bigger_than_500 2>&1|tee logs/x_is_bigger_than_500
echo "y is x?"
../build/local ./conf/y_is_x_network.ini ./conf/y_is_x_features.ini ./data/train_y_is_x ./data/test_y_is_x 101 ./checkpoints/y_is_x 2>&1|tee logs/y_is_x
echo "y is xx?"
../build/local ./conf/y_is_xx_network.ini ./conf/y_is_xx_features.ini ./data/train_y_is_xx ./data/test_y_is_xx 101 ./checkpoints/y_is_xx 2>&1|tee logs/y_is_xx
