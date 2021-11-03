/*
 * DenseInput.h
 */

#ifndef SRC_LAYER_DENSE_INPUT_H_
#define SRC_LAYER_DENSE_INPUT_H_

#include <vector>
#include <map>
#include "layer.h"

class DenseInput : public Layer {
 private:
  const int dim_in;
  const int dim_out;
  const int dim_dense;

 public:
  DenseInput(const int layer_idx,  const int dim_dense) :
             dim_in(dim_dense), dim_out(dim_dense), dim_dense(dim_dense)
  {   
      layer_index = layer_idx;
      layer_type = DENSEINPUT;
  }

  void forward(const Matrix& dense_bottom) 
  {
    debug << "DenseInput Forward: Start" << endl;
    top = dense_bottom;
    debug << "top:" << top << endl;
    debug << "DenseInput Forward: End" << endl;
  }

  int output_dim() { return dim_out; }
};

#endif  // SRC_LAYER_DENSE_INPUT_H_
