#ifndef SRC_LAYER_FULLY_CONNECTED_H_
#define SRC_LAYER_FULLY_CONNECTED_H_

#include <vector>
#include "layer.h"

class FullyConnected : public Layer {
 private:
  const int dim_in;
  const int dim_out;

  Matrix weight;  // weight parameter
  Vector bias;  // bias paramter
  Matrix grad_weight;  // gradient w.r.t weight
  Vector grad_bias;  // gradient w.r.t bias

  string w_param = "w";
  string b_param = "b";

  void init();

 public:
  FullyConnected(const int layer_idx, const int dim_in, const int dim_out) :
                 dim_in(dim_in), dim_out(dim_out)
  { 
      layer_type = FULLYCONNECTED;
      layer_index = layer_idx;
      init(); 
  }

  void forward();
  void backward();
  void update(Optimizer* opt);
  int output_dim() { return dim_out; }
  ParamInfo get_parameters();
  ParamMap get_gradient();
  void print_gradient();
  ParamMap get_parameters_and_values();
  void set_parameters_and_values(const ParamMap& param_and_values);
  void print_parameters_and_values();
  string gen_fc_param(string mode, int index = 0);

};
#endif  // SRC_LAYER_FULLY_CONNECTED_H_
