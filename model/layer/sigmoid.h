#ifndef SRC_LAYER_SIGMOID_H_
#define SRC_LAYER_SIGMOID_H_

#include "layer.h"

class Sigmoid : public Layer {
 public:
  Sigmoid(int layer_idx) {layer_type = SIGMOID;layer_index = layer_idx;}
  void forward();
  void backward();
};

#endif  // SRC_LAYER_SIGMOID_H_
