# ps-dnn
This is a model training and prediction framework.
1) It includes a complete set of processes such as sample generation, feature extraction, model training and prediction 
2） It is lightweight, based on C++, and can be directly used for online business where tensorflow is too inconvenient 
3)it supports feature extraction operator such as bucket/combine/group/hit 
4)it supports distributed training with ps-lite 


这是一个基于参数服务器（Parameter Server）PS-Lite的分布式深度学习训练和预测框架，特点如下：
1) 框架包含了从样本生成、特征抽取、模型训练到预测等一套完整的互联网深度学习落地流程
2）非常轻量级，可以直接用于线上业务
3）支持大量的特征抽取算子，譬如bucket/combine/group/hit等
4）支持分布式训练，基于ps-lite

代码结构：
conf:特征抽取和模型训练参数
sample:样本处理和原始特征拼接
feature_extract:特征抽取
model:模型训练

为了验证本框架的效果，我们使用淘宝点击率预估数据集（https://tianchi.aliyun.com/dataset/dataDetail?dataId=56）进行了效果和性能验证。
数据集合基线AUC为0.622,采用本框架训练的点击率预估模型AUC为0.66


