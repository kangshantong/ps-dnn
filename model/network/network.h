#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include <stdlib.h>
#include <vector>
#include "../loss/loss.h"
#include "../optimizer/optimizer.h"
#include "../utils/utils.h"
#include "../layer/dense_input.h"
#include "../layer/sparse_input.h"
#include "../layer/fully_connected.h"
#include "../layer/relu.h"
#include "../layer/sigmoid.h"
#include "../layer/tanh.h"
#include "../layer/softmax.h"
#include "../layer/dot.h"
#include "../emb/emb_dict.h"
#include "ps/ps.h"

class Network {
 private:
  // dense layers
  DenseInput* dense_layer_user;
  DenseInput* dense_layer_ad;
  DenseInput* dense_layer_user_ad;
  // sparse layers
  SparseInput* sparse_layer_user;
  SparseInput* sparse_layer_ad;
  SparseInput* sparse_layer_user_ad;

  vector<Layer*> net_layers;  // net layer pointers without dense inputs and sparse inputs, ordered by toplogy 

  Loss* loss;  // loss
  Optimizer* opt; //优化器
  int max_layer_index; //当前已经分配的最大layer序号

 public:
  Network() : dense_layer_user(nullptr), dense_layer_ad(nullptr), dense_layer_user_ad(nullptr),
              sparse_layer_user(nullptr), sparse_layer_ad(nullptr), sparse_layer_user_ad(nullptr),
              loss(nullptr), opt(nullptr),max_layer_index(-1) {}
  ~Network() {}

  int get_layer_index() {max_layer_index += 1; return max_layer_index;}
  void connect_layer(Layer* input, Layer* output) {input->set_output(output);output->add_input(input);}
  void add_layer(Layer* layer) { net_layers.push_back(layer); }
  void add_loss(Loss* loss_in) { loss = loss_in; }
  void add_opt(Optimizer* opt_in) { opt = opt_in; }
  int do_build_network_fnn(vector<string> net_layer_confs);
  int do_build_network_dssm(vector<string> net_layer_confs);
  int build_network(int dense_dim_user, int dense_dim_ad, int dense_dim_user_ad, 
      int sparse_dim_user, int sparse_dim_ad, int sparse_dim_user_ad,
      int emb_dim, vector<string> net_layer_confs, string model);
  
  void forward(const Matrix& dense_user_input, const Matrix& dense_ad_input, const Matrix& dense_user_ad_input,
    const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input);
  void backward(const Matrix& dense_user_input, const Matrix& dense_ad_input, const Matrix& dense_user_ad_input,
    const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input,
    const Matrix& target); 
  void update();

  const Matrix& output() { return net_layers.back()->output(); }
  void print_layers_out() const;
  float get_loss() { return loss->output(); }
  // Get the layer parameters
  
  ParamMap get_fc_gradient();
  ParamMap get_sparse_gradient();
  ParamMap get_gradient();
  void print_gradient() const;

  ParamInfo get_fc_parameters() const;
  ParamInfo get_sparse_parameters(const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, 
    const Matrix3D& sparse_user_ad_input) const;
  ParamMap get_parameters_and_values();
  void set_fc_parameters_and_values(const ParamMap& param_and_values);
  void set_sparse_parameters_and_values(const ParamMap& param_and_values);
  void set_parameters_and_values(const ParamMap& param_and_values);
  void print_parameters_and_values() const;

  void save_model(string file_name);
  void load_model(string file_name);

};

#endif  // SRC_NETWORK_H_
