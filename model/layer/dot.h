#ifndef SRC_LAYER_DOT_H_
#define SRC_LAYER_DOT_H_

#include "layer.h"

class Dot : public Layer {
  private:
    const Matrix* left_bottom;
    const Matrix* right_bottom;
    Matrix left_grad_bottom;
    Matrix right_grad_bottom;
  public:
    Dot(int layer_idx) {layer_type = DOT;layer_index = layer_idx;left_bottom = nullptr; right_bottom = nullptr;}
    void gather_inputs();
    void split_grad_bottom();
    void forward();
    void backward();
};

#endif  // SRC_LAYER_DOT_H_
