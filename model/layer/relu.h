#ifndef SRC_LAYER_RELU_H_
#define SRC_LAYER_RELU_H_

#include "layer.h"

class ReLU : public Layer {
 public:
  ReLU(int layer_idx) {layer_type = RELU;layer_index = layer_idx;}
  void forward();
  void backward();
};

#endif  // SRC_LAYER_RELU_H_
