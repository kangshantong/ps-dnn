#!/usr/bin/python
# -*- coding: UTF-8 -*-
import random
import math

 
f = open("./data/train_y_is_x", "w")

for i in range(100000):
    x=random.randint(1,100)
    x1=random.randint(1,100)
    y=math.log(x)
    res=[str(i),str(y),str(x),str(x)]
    f.write("\t".join(res)+"\n")
  
f.close()

f = open("./data/test_y_is_x", "w")

for i in range(10000):
    x=random.randint(1,100)
    x1=random.randint(1,100)
    y=math.log(x)
    res=[str(i),str(y),str(x),str(x)]
    f.write("\t".join(res)+"\n")
  
f.close()

f = open("./data/train_y_is_xx", "w")

for i in range(100000):
    x=random.randint(1,100)
    x1=random.randint(1,100)
    y=math.log(x*x)
    res=[str(i),str(y),str(x),str(x)]
    f.write("\t".join(res)+"\n")
  
f.close()

f = open("./data/test_y_is_xx", "w")

for i in range(10000):
    x=random.randint(1,100)
    x1=random.randint(1,100)
    y=math.log(x*x)
    res=[str(i),str(y),str(x),str(x)]
    f.write("\t".join(res)+"\n")
  
f.close()

f = open("./data/test_x_is_bigger_than_500", "w")

for i in range(100000):
    x=random.randint(1,200)
    x1=random.randint(1,200)
    y=0
    if x>=100:
        y=1
    #res=[str(i),str(y),"0",str(x)+"|"+str(y)]
    res=[str(i),str(y),str(x1),str(x)]
    f.write("\t".join(res)+"\n")
  
f.close()
