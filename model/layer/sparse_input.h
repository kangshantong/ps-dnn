/*
 * SparseInput.h
 */

#ifndef SRC_LAYER_SPARSEINPUT_H_
#define SRC_LAYER_SPARSEINPUT_H_

#include <vector>
#include<stdlib.h>
#include <map>
#include <unordered_map>
#include "layer.h"
#include "../emb/emb_dict.h"

class SparseInput : public Layer {
 private:
  const int dim_in;
  const int dim_out;
  const int dim_sparse;
  const int dim_sparse_emb;

  ParamMap grad_sparse_emb;  // gradient for sparse feature embedding

  void init();

 public:
  SparseInput(const int layer_idx,  const int dim_sparse, const int dim_sparse_emb) :
                 dim_sparse(dim_sparse), dim_sparse_emb(dim_sparse_emb),
                 dim_in(dim_sparse),dim_out(dim_sparse*dim_sparse_emb)
  {   
      layer_index = layer_idx;
      layer_type = SPARSEINPUT;
  }

  void forward(const Matrix3D& sparse_bottom);
  void backward(const Matrix3D& sparse_bottom);
  void update(Optimizer* opt);

  int output_dim() { return dim_out; }

  ParamInfo get_parameters(const Matrix3D& sparse_bottom);
  ParamMap get_gradient();
  void print_gradient();
};

#endif  // SRC_LAYER_SPARSEINPUT_H_
