#ifndef SRC_LAYER_SOFTMAX_H_
#define SRC_LAYER_SOFTMAX_H_

#include "layer.h"

class Softmax: public Layer {
 public:
  Softmax(int layer_idx) {layer_type = SOFTMAX;layer_index = layer_idx;}
  void forward();
  void backward();
};

#endif  // SRC_LAYER_SOFTMAX_H_
