#ifndef SRC_LAYER_TANH_H_
#define SRC_LAYER_TANH_H_

#include "layer.h"

class Tanh : public Layer {
 public:
  Tanh(int layer_idx) {layer_type = TANH;layer_index = layer_idx;}
  void forward();
  void backward();
};

#endif  // SRC_LAYER_TANH_H_
