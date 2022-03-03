# ps-dnn-一个基于参数服务器（Parameter Server）PS-Lite的分布式深度学习训练和预测框架
This is a model training and prediction framework.

1) It includes a complete set of processes such as sample generation, feature extraction, model training and prediction 

2） It is lightweight, based on C++, and can be directly used for online business where tensorflow is too inconvenient 

3)it supports feature extraction operator such as bucket/combine/group/hit 

4)it supports distributed training with ps-lite 

We provide a demo in order to introduce how to use this framework.

The dataset is taobao Ad Display/Click Data on Taobao.com(https://tianchi.aliyun.com/dataset/dataDetail?dataId=56) .

sh run_taobao_ctr.sh

This command will start the whole prodedure of taobao ctr prediction model.


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


为了评估分布式训练的效果，本文使用使用淘宝点击率预估数据集（https://tianchi.aliyun.com/dataset/dataDetail?dataId=56）在不同的模式（单机/分布式）和不同的server/worker节点数量对训练效率进行了评估。

运行脚本：sh run_taobao_ctr.sh (为了缩短运行时间，示例中去掉了用户行为特征的抽取，如果需要的话，在sample/gen_sample_taobao_ctr.sh中将global user behavior feature和local user behavior feature的注释关掉）

 ![image](https://user-images.githubusercontent.com/9473273/141041693-99bba636-1d3c-4206-952a-aa73dabb6c7d.png)
  
	
	从上述数据可以看出：
	
1）	改造为分布式后，1 server 1 worker的配置下，训练时间为545.2min，比单机模式的290.2min多了88%,auc相差无几。说明本文分布式改造带来的通信开销还是挺大的，但是训练的准确性是有保证的

2）	在分布式-多进程模型下,随着worker数量的加倍，模型的训练时间呈现指数级下降趋势,但是下降的幅度逐步变小；同时auc的在worker=2/4/8时变化不大，但是在worker=16时有了明显下降，一般此时可以通过使用实时业务的庞大样本量或调整训练参数来弥补。

3）	多线程模式和多进程模型相比，训练时间相差不大（37.2min vs 37.2min）,但是test auc下降较多（0.638351 vs0.644146）

整体而言，分布式训练相比于单机训练，在auc损失不大的情况下，换来了训练时间的大幅度下降。


